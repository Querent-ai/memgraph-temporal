#include "storage/vertices.hpp"
#include "utils/iterator/iterator.hpp"

Vertices::vertices_t::Accessor Vertices::access() { return vertices.access(); }

Option<const Vertex::Accessor> Vertices::find(DbTransaction &t, const Id &id)
{
    auto vertices_accessor = vertices.access();
    auto vertices_iterator = vertices_accessor.find(id);

    if (vertices_iterator == vertices_accessor.end())
        return make_option<const Vertex::Accessor>();

    return make_option_const(Vertex::Accessor(&vertices_iterator->second, t));
}

Vertex::Accessor Vertices::insert(DbTransaction &t)
{
    // get next vertex id
    auto next = counter.next();

    // create new vertex record
    VertexRecord vertex_record(next);
    // vertex_record.id(next);

    // insert the new vertex record into the vertex store
    auto vertices_accessor = vertices.access();
    auto result = vertices_accessor.insert(next, std::move(vertex_record));

    // create new vertex
    auto inserted_vertex_record = result.first;
    auto vertex = inserted_vertex_record->second.insert(t.trans);

    return Vertex::Accessor(vertex, &inserted_vertex_record->second, t);
}

PropertyFamily &
Vertices::property_family_find_or_create(const std::string &name)
{
    auto acc = prop_familys.access();
    auto it = acc.find(name);
    if (it == acc.end()) {
        auto family = std::unique_ptr<PropertyFamily>(new PropertyFamily(name));
        auto res = acc.insert(name, std::move(family));
        it = res.first;
    }
    return *(it->second);
}
