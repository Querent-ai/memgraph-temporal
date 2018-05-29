#include "storage/dynamic_graph_partitioner/dgp.hpp"

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "database/graph_db_accessor.hpp"
#include "distributed/updates_rpc_clients.hpp"
#include "query/exceptions.hpp"
#include "storage/dynamic_graph_partitioner/vertex_migrator.hpp"
#include "utils/flag_validation.hpp"

DEFINE_VALIDATED_int32(
    dgp_improvement_threshold, 10,
    "How much better should specific node score be to consider "
    "a migration to another worker. This represents the minimal difference "
    "between new score that the vertex will have when migrated and the old one "
    "such that it's migrated.",
    FLAG_IN_RANGE(1, 100));
DEFINE_VALIDATED_int32(dgp_max_batch_size, 2000,
                       "Maximal amount of vertices which should be migrated in "
                       "one dynamic graph partitioner step.",
                       FLAG_IN_RANGE(1, std::numeric_limits<int32_t>::max()));

DynamicGraphPartitioner::DynamicGraphPartitioner(database::GraphDb *db)
    : db_(db) {}

void DynamicGraphPartitioner::Run() {
  database::GraphDbAccessor dba(*db_);
  VLOG(21) << "Starting DynamicGraphPartitioner in tx: "
           << dba.transaction().id_;

  auto migrations = FindMigrations(dba);

  try {
    VertexMigrator migrator(&dba);
    for (auto &migration : migrations) {
      migrator.MigrateVertex(migration.first, migration.second);
    }

    auto apply_futures = db_->updates_clients().UpdateApplyAll(
        db_->WorkerId(), dba.transaction().id_);

    for (auto &future : apply_futures) {
      switch (future.get()) {
        case distributed::UpdateResult::SERIALIZATION_ERROR:
          throw mvcc::SerializationError(
              "Failed to relocate vertex due to SerializationError");
        case distributed::UpdateResult::UNABLE_TO_DELETE_VERTEX_ERROR:
          throw query::RemoveAttachedVertexException();
        case distributed::UpdateResult::UPDATE_DELETED_ERROR:
          throw query::QueryRuntimeException(
              "Failed to apply deferred updates due to RecordDeletedError");
        case distributed::UpdateResult::LOCK_TIMEOUT_ERROR:
          throw LockTimeoutException(
              "Failed to apply deferred update due to LockTimeoutException");
        case distributed::UpdateResult::DONE:
          break;
      }
    }

    dba.Commit();
    VLOG(21) << "Sucesfully migrated " << migrations.size() << " vertices..";
  } catch (const utils::BasicException &e) {
    VLOG(21) << "Didn't succeed in relocating; " << e.what();
    dba.Abort();
  }
}

std::vector<std::pair<VertexAccessor, int>>
DynamicGraphPartitioner::FindMigrations(database::GraphDbAccessor &dba) {
  // Find workers vertex count
  std::unordered_map<int, int64_t> worker_vertex_count =
      db_->data_clients().VertexCounts(dba.transaction().id_);

  int64_t total_vertex_count = 0;
  for (auto worker_vertex_count_pair : worker_vertex_count) {
    total_vertex_count += worker_vertex_count_pair.second;
  }

  double average_vertex_count =
      total_vertex_count * 1.0 / worker_vertex_count.size();

  // Considers all migrations which maximally improve single vertex score
  std::vector<std::pair<VertexAccessor, int>> migrations;
  for (const auto &vertex : dba.Vertices(false)) {
    auto label_counts = CountLabels(vertex);
    std::unordered_map<int, double> per_label_score;
    size_t degree = vertex.in_degree() + vertex.out_degree();

    for (auto worker_vertex_count_pair : worker_vertex_count) {
      int worker = worker_vertex_count_pair.first;
      int64_t worker_vertex_count = worker_vertex_count_pair.second;
      per_label_score[worker] =
          label_counts[worker] * 1.0 / degree -
          worker_vertex_count * 1.0 / average_vertex_count;
    }

    auto label_cmp = [](const std::pair<int, double> &p1,
                        const std::pair<int, double> &p2) {
      return p1.second < p2.second;
    };
    auto best_label = std::max_element(per_label_score.begin(),
                                       per_label_score.end(), label_cmp);

    // Consider as a migration only if the improvement is high enough
    if (best_label != per_label_score.end() &&
        best_label->first != db_->WorkerId() &&
        per_label_score[best_label->first] -
                FLAGS_dgp_improvement_threshold / 100.0 >=
            per_label_score[db_->WorkerId()]) {
      migrations.emplace_back(vertex, best_label->first);
    }

    if (migrations.size() >= FLAGS_dgp_max_batch_size) break;
  }

  return migrations;
}

std::unordered_map<int, int64_t> DynamicGraphPartitioner::CountLabels(
    const VertexAccessor &vertex) const {
  std::unordered_map<int, int64_t> label_count;
  for (auto edge : vertex.in()) {
    auto address = edge.from().address();
    auto label = address.is_remote() ? address.worker_id() : db_->WorkerId();
    label_count[label]++;
  }
  for (auto edge : vertex.out()) {
    auto address = edge.to().address();
    auto label = address.is_remote() ? address.worker_id() : db_->WorkerId();
    label_count[label]++;
  }
  return label_count;
}
