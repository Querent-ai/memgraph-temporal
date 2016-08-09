#pragma once

#include "data_structures/map/rh_hashmultimap.hpp"
#include "mvcc/version_list.hpp"

class EdgeMap
{
public:
    auto begin() { return edges.begin(); }
    auto begin() const { return edges.begin(); }
    auto cbegin() const { return edges.begin(); }

    auto end() { return edges.end(); }
    auto end() const { return edges.end(); }
    auto cend() const { return edges.end(); }

    size_t degree() const { return edges.size(); }

    void add(EdgeRecord *edge) { edges.add(edge); }

    void remove(EdgeRecord *edge)
    {
        // TODO
        throw std::bad_function_call::bad_function_call();
    }

    void clear() { edges.clear(); }

private:
    RhHashMultiMap<VertexRecord *, EdgeRecord> edges;
};
