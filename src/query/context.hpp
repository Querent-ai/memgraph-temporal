#pragma once

#include "query/common.hpp"
#include "query/frontend/semantic/symbol_table.hpp"
#include "query/parameters.hpp"
#include "query/plan/profile.hpp"
#include "utils/tsc.hpp"

namespace query {

struct EvaluationContext {
  /// Memory for allocations during evaluation of a *single* Pull call.
  ///
  /// Although the assigned memory may live longer than the duration of a Pull
  /// (e.g. memory is the same as the whole execution memory), you have to treat
  /// it as if the lifetime is only valid during the Pull.
  utils::MemoryResource *memory{utils::NewDeleteResource()};
  int64_t timestamp{-1};
  Parameters parameters;
  /// All properties indexable via PropertyIx
  std::vector<storage::PropertyId> properties;
  /// All labels indexable via LabelIx
  std::vector<storage::LabelId> labels;
  /// All counters generated by `counter` function, mutable because the function
  /// modifies the values
  mutable std::unordered_map<std::string, int64_t> counters;
};

inline std::vector<storage::PropertyId> NamesToProperties(
    const std::vector<std::string> &property_names, DbAccessor *dba) {
  std::vector<storage::PropertyId> properties;
  properties.reserve(property_names.size());
  for (const auto &name : property_names) {
    properties.push_back(dba->NameToProperty(name));
  }
  return properties;
}

inline std::vector<storage::LabelId> NamesToLabels(
    const std::vector<std::string> &label_names, DbAccessor *dba) {
  std::vector<storage::LabelId> labels;
  labels.reserve(label_names.size());
  for (const auto &name : label_names) {
    labels.push_back(dba->NameToLabel(name));
  }
  return labels;
}

struct ExecutionContext {
  DbAccessor *db_accessor{nullptr};
  SymbolTable symbol_table;
  EvaluationContext evaluation_context;
  utils::TSCTimer execution_tsc_timer;
  double max_execution_time_sec{0.0};
  std::atomic<bool> *is_shutting_down{nullptr};
  bool is_profile_query{false};
  std::chrono::duration<double> profile_execution_time;
  plan::ProfilingStats stats;
  plan::ProfilingStats *stats_root{nullptr};
};

inline bool MustAbort(const ExecutionContext &context) {
  return (context.is_shutting_down &&
          context.is_shutting_down->load(std::memory_order_acquire)) ||
         (context.max_execution_time_sec > 0 &&
          context.execution_tsc_timer.Elapsed() >=
              context.max_execution_time_sec);
}

}  // namespace query
