#pragma once

#include <limits>
#include <set>
#include <vector>

#include "cppitertools/chain.hpp"

#include "database/graph_db.hpp"
#include "storage/record_accessor.hpp"
#include "storage/util.hpp"
#include "storage/vertex.hpp"

#include "storage/edge_accessor.hpp"

/**
 * Provides ways for the client programmer (i.e. code generated
 * by the compiler) to interact with a Vertex.
 *
 * This class indirectly inherits MVCC data structures and
 * takes care of MVCC versioning.
 */
class VertexAccessor : public RecordAccessor<Vertex> {
 public:
  using RecordAccessor::RecordAccessor;

  /**
   * Returns the number of outgoing edges.
   * @return
   */
  size_t out_degree() const;

  /**
   * Returns the number of incoming edges.
   * @return
   */
  size_t in_degree() const;

  /**
   * Adds a label to the Vertex. If the Vertex already
   * has that label the call has no effect.
   * @param label A label.
   * @return If or not a new Label was set on this Vertex.
   */
  bool add_label(GraphDbTypes::Label label);

  /**
   * Removes a label from the Vertex.
   * @param label  The label to remove.
   * @return The number of removed labels (can be 0 or 1).
   */
  size_t remove_label(GraphDbTypes::Label label);

  /**
   * Indicates if the Vertex has the given label.
   * @param label A label.
   * @return
   */
  bool has_label(GraphDbTypes::Label label) const;

  /**
   * Returns all the Labels of the Vertex.
   * @return
   */
  const std::vector<GraphDbTypes::Label> &labels() const;

  /**
   * Returns EdgeAccessors for all incoming edges.
   */
  auto in() const {
    return MakeAccessorIterator<EdgeAccessor>(
        current().in_.begin(), current().in_.end(), db_accessor());
  }

  /**
   * Returns EdgeAccessors for all incoming edges.
   *
   * @param dest - The destination vertex filter.
   * @param edge_types - Edge types filter. At least one be matched. If nullptr
   * or empty, the parameter is ignored.
   */
  auto in(
      const VertexAccessor &dest,
      const std::vector<GraphDbTypes::EdgeType> *edge_types = nullptr) const {
    return MakeAccessorIterator<EdgeAccessor>(
        current().in_.begin(dest.vlist_, edge_types), current().in_.end(),
        db_accessor());
  }

  /**
   * Returns EdgeAccessors for all incoming edges.
   *
   * @param edge_types - Edge types filter. At least one be matched. If nullptr
   * or empty, the parameter is ignored.
   */
  auto in(const std::vector<GraphDbTypes::EdgeType> *edge_types) const {
    return MakeAccessorIterator<EdgeAccessor>(
        current().in_.begin(nullptr, edge_types), current().in_.end(),
        db_accessor());
  }

  /**
   * Returns EdgeAccessors for all outgoing edges.
   */
  auto out() const {
    return MakeAccessorIterator<EdgeAccessor>(
        current().out_.begin(), current().out_.end(), db_accessor());
  }

  /**
   * Returns EdgeAccessors for all outgoing edges whose destination is the given
   * vertex.
   *
   * @param dest - The destination vertex filter.
   * @param edge_types - Edge types filter. At least one be matched. If nullptr
   * or empty, the parameter is ignored.
   */
  auto out(
      const VertexAccessor &dest,
      const std::vector<GraphDbTypes::EdgeType> *edge_types = nullptr) const {
    return MakeAccessorIterator<EdgeAccessor>(
        current().out_.begin(dest.vlist_, edge_types), current().out_.end(),
        db_accessor());
  }

  /**
   * Returns EdgeAccessors for all outgoing edges.
   *
   * @param edge_types - Edge types filter. At least one be matched. If nullptr
   * or empty, the parameter is ignored.
   */
  auto out(const std::vector<GraphDbTypes::EdgeType> *edge_types) const {
    return MakeAccessorIterator<EdgeAccessor>(
        current().out_.begin(nullptr, edge_types), current().out_.end(),
        db_accessor());
  }
};

std::ostream &operator<<(std::ostream &, const VertexAccessor &);

// hash function for the vertex accessor
namespace std {
template <>
struct hash<VertexAccessor> {
  size_t operator()(const VertexAccessor &v) const { return v.temporary_id(); };
};
}
