#include "snapshot/snapshot_engine.hpp"

#include "config/config.hpp"
#include "database/db_accessor.hpp"
#include "logging/default.hpp"
#include "snapshot/snapshot_decoder.hpp"
#include "snapshot/snapshot_encoder.hpp"
#include "storage/indexes/indexes.hpp"
#include "threading/thread.hpp"
#include "utils/sys.hpp"

SnapshotEngine::SnapshotEngine(Db &db, std::string const &name)
    : snapshot_folder(CONFIG(config::SNAPSHOTS_PATH)), db(db),
      logger(logging::log->logger("SnapshotEngine db[" + name + "]"))
{
}

bool SnapshotEngine::make_snapshot()
{
    std::lock_guard<std::mutex> lock(guard);
    std::time_t now = std::time(nullptr);
    return make_snapshot(now, "full");
}

bool SnapshotEngine::make_snapshot(std::time_t now, const char *type)
{
    bool success = false;

    auto snapshot_file_name = snapshot_file(now, type);

    logger.info("Writing {} snapshot to file \"{}\"", type, snapshot_file_name);

    DbTransaction t(db);

    try {
        std::ofstream snapshot_file(snapshot_file_name,
                                    std::fstream::binary | std::fstream::trunc);

        SnapshotEncoder snap(snapshot_file);

        auto old_trans = tx::TransactionRead(db.tx_engine);
        snapshot(t, snap, old_trans);

        auto res = sys::flush_file_to_disk(snapshot_file);
        if (res == 0) {
            t.trans.commit();
            success = true;
        } else {
            logger.error("Error {} occured while flushing snapshot file", res);
            t.trans.abort();
        }

    } catch (const std::exception &e) {
        logger.error("Exception occured while creating {} snapshot", type);
        logger.error("{}", e.what());

        t.trans.abort();
    }

    if (success) {
        std::ofstream commit_file(snapshot_commit_file(), std::fstream::app);

        commit_file << snapshot_file_name << std::endl;

        auto res = sys::flush_file_to_disk(commit_file);
        if (res == 0) {
            commit_file.close();
            snapshoted_no_v.fetch_add(1);
        } else {
            logger.error("Error {} occured while flushing commit file", res);
        }
    }

    return success;
}

bool SnapshotEngine::import()
{
    std::lock_guard<std::mutex> lock(guard);

    logger.info("Started import");
    bool success = false;

    try {

        std::ifstream commit_file(snapshot_commit_file());

        std::vector<std::string> snapshots;
        std::string line;
        while (std::getline(commit_file, line)) {
            snapshots.push_back(line);
        }

        while (snapshots.size() > 0) {
            logger.info("Importing data from snapshot \"{}\"",
                        snapshots.back());

            DbTransaction t(db);

            try {
                std::ifstream snapshot_file(snapshots.back(),
                                            std::fstream::binary);
                SnapshotDecoder decoder(snapshot_file);

                if (snapshot_load(t, decoder)) {
                    t.trans.commit();
                    logger.info("Succesfully imported snapshot \"{}\"",
                                snapshots.back());
                    success = true;
                    break;
                } else {
                    t.trans.abort();
                    logger.info("Unuccesfully tryed to import snapshot \"{}\"",
                                snapshots.back());
                }

            } catch (const std::exception &e) {
                logger.error("Error occured while importing snapshot \"{}\"",
                             snapshots.back());
                logger.error("{}", e.what());
                t.trans.abort();
            }

            snapshots.pop_back();
        }

    } catch (const std::exception &e) {
        logger.error("Error occured while importing snapshot");
        logger.error("{}", e.what());
    }

    logger.info("Finished import");

    return success;
}

void SnapshotEngine::snapshot(DbTransaction const &dt, SnapshotEncoder &snap,
                              tx::TransactionRead const &old_trans)
{
    Db &db = dt.db;
    DbAccessor t(db, dt.trans);

    // Anounce property names
    for (auto &family : db.graph.vertices.property_family_access()) {
        snap.property_name_init(family.first);
    }
    for (auto &family : db.graph.edges.property_family_access()) {
        snap.property_name_init(family.first);
    }

    // Anounce label names
    for (auto &labels : db.graph.label_store.access()) {
        snap.label_name_init(labels.first.to_string());
    }

    // Anounce edge_type names
    for (auto &et : db.graph.edge_type_store.access()) {
        snap.edge_type_name_init(et.first.to_string());
    }

    // Store vertices
    snap.start_vertices();
    t.vertex_access()
        .fill()
        .filter([&](auto va) { return !va.is_visble_to(old_trans); })
        .for_all([&](auto va) { serialization::serialize_vertex(va, snap); });

    // Store edges
    snap.start_edges();
    t.edge_access()
        .fill()
        .filter([&](auto va) { return !va.is_visble_to(old_trans); })
        .for_all([&](auto ea) { serialization::serialize_edge(ea, snap); });

    // Store info on existing indexes.
    snap.start_indexes();
    db.indexes().vertex_indexes([&](auto &i) { snap.index(i.definition()); });
    db.indexes().edge_indexes([&](auto &i) { snap.index(i.definition()); });

    snap.end();
}

bool SnapshotEngine::snapshot_load(DbTransaction const &dt,
                                   SnapshotDecoder &snap)
{
    std::unordered_map<uint64_t, VertexAccessor> vertices;

    Db &db = dt.db;
    DbAccessor t(db, dt.trans);

    // Load names
    snap.load_init();

    // Load vertices
    snap.begin_vertices();
    while (!snap.end_vertices()) {
        vertices.insert(serialization::deserialize_vertex(t, snap));
    }

    // Load edges
    snap.begin_edges();
    while (!snap.end_edges()) {
        serialization::deserialize_edge(t, snap, vertices);
    }

    // Load indexes
    snap.start_indexes();
    auto indexs = db.indexes();
    while (!snap.end()) {
        // This will add index. It is alright for now to ignore if add_index
        // return false.
        indexs.add_index(snap.load_index());
    }

    return true;
}

std::string SnapshotEngine::snapshot_file(std::time_t const &now,
                                          const char *type)
{
    return snapshot_db_dir() + "/" + std::to_string(now) + "_" + type;
}

std::string SnapshotEngine::snapshot_commit_file()
{
    return snapshot_db_dir() + "/snapshot_commit.txt";
}

std::string SnapshotEngine::snapshot_db_dir()
{
    if (!sys::ensure_directory_exists(snapshot_folder)) {
        logger.error("Error while creating directory \"{}\"", snapshot_folder);
    }
    auto db_path = snapshot_folder + "/" + db.name();
    if (!sys::ensure_directory_exists(db_path)) {
        logger.error("Error while creating directory \"{}\"", db_path);
    }
    return db_path;
}
