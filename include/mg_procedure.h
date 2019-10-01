/// @file
/// Provides API for usage in custom openCypher procedures
#ifndef MG_PROCEDURE_H
#define MG_PROCEDURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/// @name Memory Allocation
///
/// These should be preferred compared to plain malloc calls as Memgraph's
/// execution will handle allocation and deallocation more efficiently. In
/// addition to efficiency, Memgraph can set the limit on allowed allocations
/// thus providing some safety with regards to memory usage. The allocated
/// memory is only valid during the execution of mgp_main. You must not allocate
/// global resources with these functions. None of the functions are
/// thread-safe, because we provide a single thread of execution when invoking a
/// custom procedure. This allows Memgraph to be more efficient as stated
/// before.
///@{

/// Allocate a block of memory with given size in bytes.
/// Unlike malloc, this function is not thread-safe.
/// `size_in_bytes` must be greater than 0.
/// The returned pointer must be freed with mgp_free.
/// NULL is returned if unable to serve the requested allocation.
void *mgp_alloc(size_t size_in_bytes);

/// Allocate an aligned block of memory with given size in bytes.
/// Unlike malloc and aligned_alloc, this function is not thread-safe.
/// `size_in_bytes` must be greater than 0.
/// `alignment` must be a power of 2 value.
/// The returned pointer must be freed with mgp_free.
/// NULL is returned if unable to serve the requested allocation.
void *mgp_aligned_alloc(size_t size_in_bytes, size_t alignment);

/// Deallocate an allocation from mgp_alloc or mgp_aligned_alloc.
/// Unlike free, this function is not thread-safe.
/// If `ptr` is NULL, this function does nothing.
/// The behavior is undefined if `ptr` is not a value returned from a prior
/// mgp_alloc or mgp_aligned_alloc call.
void mgp_free(void *ptr);
///@}

/// @name Operations on mgp_value
///
/// struct mgp_value is an immutable container of various values that may appear
/// as arguments and results of a custom procedure. The following functions and
/// types are used to work with mgp_value. Each function returning a non-const
/// mgp_value has allocated a new instance of the result, therefore such
/// mgp_value instances need to be deallocated using mgp_value_destroy.
///@{

/// Immutable container of various values that appear in the query language.
struct mgp_value;

/// List of mgp_value instances
struct mgp_list;

/// Map of character strings to mgp_value instances.
struct mgp_map;

/// Vertex in the graph database.
struct mgp_vertex;

/// Edge in the graph database.
struct mgp_edge;

/// Path containing mgp_vertex and mgp_edge instances.
struct mgp_path;

/// All available types that can be stored in a mgp_value
enum mgp_value_type {
  // NOTE: New types need to be appended, so as not to break ABI.
  MGP_VALUE_TYPE_NULL,
  MGP_VALUE_TYPE_BOOL,
  MGP_VALUE_TYPE_INT,
  MGP_VALUE_TYPE_DOUBLE,
  MGP_VALUE_TYPE_STRING,
  MGP_VALUE_TYPE_LIST,
  MGP_VALUE_TYPE_MAP,
  MGP_VALUE_TYPE_VERTEX,
  MGP_VALUE_TYPE_EDGE,
  MGP_VALUE_TYPE_PATH,
};

/// Free the memory used by the given mgp_value instance.
void mgp_value_destroy(struct mgp_value *val);

/// Construct a value representing `null` in openCypher.
/// You need to free the instance through mgp_value_destroy.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_null();

/// Construct a boolean value.
/// Non-zero values represent `true`, while zero represents `false`.
/// You need to free the instance through mgp_value_destroy.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_bool(int val);

/// Construct an integer value.
/// You need to free the instance through mgp_value_destroy.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_int(int64_t val);

/// Construct a double floating point value.
/// You need to free the instance through mgp_value_destroy.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_double(double val);

/// Construct a character string value from a NULL terminated string.
/// You need to free the instance through mgp_value_destroy.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_string(const char *val);

/// Create a mgp_value storing a mgp_list.
/// You need to free the instance through mgp_value_destroy. The ownership of
/// the list is given to the created mgp_value and destroying the mgp_value will
/// destroy the mgp_list. Therefore, if a mgp_value is successfully created
/// you must not call mgp_list_destroy on the given list.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_list(struct mgp_list *val);

/// Create a mgp_value storing a mgp_map.
/// You need to free the instance through mgp_value_destroy. The ownership of
/// the map is given to the created mgp_value and destroying the mgp_value will
/// destroy the mgp_map. Therefore, if a mgp_value is successfully created
/// you must not call mgp_map_destroy on the given map.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_map(struct mgp_map *val);

/// Create a mgp_value storing a mgp_vertex.
/// You need to free the instance through mgp_value_destroy. The ownership of
/// the vertex is given to the created mgp_value and destroying the mgp_value
/// will destroy the mgp_vertex. Therefore, if a mgp_value is successfully
/// created you must not call mgp_vertex_destroy on the given vertex.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_vertex(struct mgp_vertex *val);

/// Create a mgp_value storing a mgp_edge.
/// You need to free the instance through mgp_value_destroy. The ownership of
/// the edge is given to the created mgp_value and destroying the mgp_value will
/// destroy the mgp_edge. Therefore, if a mgp_value is successfully created you
/// must not call mgp_edge_destroy on the given edge.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_edge(struct mgp_edge *val);

/// Create a mgp_value storing a mgp_path.
/// You need to free the instance through mgp_value_destroy. The ownership of
/// the path is given to the created mgp_value and destroying the mgp_value will
/// destroy the mgp_path. Therefore, if a mgp_value is successfully created you
/// must not call mgp_path_destroy on the given path.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_value_make_path(struct mgp_path *val);

/// Return the type of the value contained in mgp_value.
enum mgp_value_type mgp_value_get_type(const struct mgp_value *val);

/// Return non-zero if the given mgp_value represents `null`.
int mgp_value_is_null(const struct mgp_value *val);

/// Return non-zero if the given mgp_value stores a boolean.
int mgp_value_is_bool(const struct mgp_value *val);

/// Return non-zero if the given mgp_value stores an integer.
int mgp_value_is_int(const struct mgp_value *val);

/// Return non-zero if the given mgp_value stores a double floating-point.
int mgp_value_is_double(const struct mgp_value *val);

/// Return non-zero if the given mgp_value stores a character string.
int mgp_value_is_string(const struct mgp_value *val);

/// Return non-zero if the given mgp_value stores a list of values.
int mgp_value_is_list(const struct mgp_value *val);

/// Return non-zero if the given mgp_value stores a map of values.
int mgp_value_is_map(const struct mgp_value *val);

/// Return non-zero if the given mgp_value stores a vertex.
int mgp_value_is_vertex(const struct mgp_value *val);

/// Return non-zero if the given mgp_value stores an edge.
int mgp_value_is_edge(const struct mgp_value *val);

/// Return non-zero if the given mgp_value stores a path.
int mgp_value_is_path(const struct mgp_value *val);

/// Return the contained boolean value.
/// Non-zero values represent `true`, while zero represents `false`.
/// The result is undefined if mgp_value does not contain the expected type.
int mgp_value_get_bool(const struct mgp_value *val);

/// Return the contained integer.
/// The result is undefined if mgp_value does not contain the expected type.
int64_t mgp_value_get_int(const struct mgp_value *val);

/// Return the contained double floating-point.
/// The result is undefined if mgp_value does not contain the expected type.
double mgp_value_get_double(const struct mgp_value *val);

/// Return the contained character string.
/// The result is undefined if mgp_value does not contain the expected type.
const char *mgp_value_get_string(const struct mgp_value *val);

/// Return the contained list of values.
/// The result is undefined if mgp_value does not contain the expected type.
const struct mgp_list *mgp_value_get_list(const struct mgp_value *val);

/// Return the contained map of values.
/// The result is undefined if mgp_value does not contain the expected type.
const struct mgp_map *mgp_value_get_map(const struct mgp_value *val);

/// Return the contained vertex.
/// The result is undefined if mgp_value does not contain the expected type.
const struct mgp_vertex *mgp_value_get_vertex(const struct mgp_value *val);

/// Return the contained edge.
/// The result is undefined if mgp_value does not contain the expected type.
const struct mgp_edge *mgp_value_get_edge(const struct mgp_value *val);

/// Return the contained path.
/// The result is undefined if mgp_value does not contain the expected type.
const struct mgp_path *mgp_value_get_path(const struct mgp_value *val);

/// Create an empty list with given capacity.
/// You need to free the created instance with mgp_list_destroy.
/// The created list will have allocated enough memory for `capacity` elements
/// of mgp_value, but it will not contain any elements. Therefore,
/// mgp_list_size will return 0.
/// NULL is returned if unable to allocate a new list.
struct mgp_list *mgp_list_make_empty(size_t capacity);

/// Free the memory used by the given mgp_list and contained elements.
void mgp_list_destroy(struct mgp_list *list);

/// Append a copy of mgp_value to mgp_list if capacity allows.
/// The list copies the given value and therefore does not take ownership of the
/// original value. You still need to call mgp_value_destroy to free the
/// original value.
/// Return non-zero on success, or 0 if there's no capacity or memory to append
/// the mgp_value to mgp_list.
int mgp_list_append(struct mgp_list *list, const struct mgp_value *val);

/// Append a copy of mgp_value to mgp_list increasing capacity if needed.
/// The list copies the given value and therefore does not take ownership of the
/// original value. You still need to call mgp_value_destroy to free the
/// original value.
/// In case of a capacity change, the previously contained elements will move in
/// memory and any references to them will be invalid.
/// Return non-zero on success, or 0 if there's no memory to append the
/// mgp_value to mgp_list.
int mgp_list_append_extend(struct mgp_list *list, const struct mgp_value *val);

/// Return the number of elements stored in mgp_list.
size_t mgp_list_size(const struct mgp_list *list);

/// Return the total number of elements for which there's already allocated
/// memory in mgp_list.
size_t mgp_list_capacity(const struct mgp_list *list);

/// Return the element in mgp_list at given position.
/// NULL is returned if the index is not within mgp_list_size.
const struct mgp_value *mgp_list_at(const struct mgp_list *list, size_t index);

/// Create an empty map of character strings to mgp_value instances.
/// You need to free the created instance with mgp_map_destroy.
/// NULL is returned if unable to allocate a new map.
struct mgp_map *mgp_map_make_empty();

/// Free the memory used by the given mgp_map and contained items.
void mgp_map_destroy(struct mgp_map *map);

/// Insert a new mapping from a NULL terminated character string to a value.
/// If a mapping with the same key already exists, it is *not* replaced.
/// In case of insertion, both the string and the value are copied into the map.
/// Therefore, the map does not take ownership of the original key nor value, so
/// you still need to free their memory explicitly.
/// Return non-zero on success, or 0 if there's no memory to insert a new
/// mapping or a previous mapping already exists.
int mgp_map_insert(struct mgp_map *map, const char *key,
                   const struct mgp_value *value);

/// Return the number of items stored in mgp_map.
size_t mgp_map_size(const struct mgp_map *map);

/// Return the mapped mgp_value to the given character string.
/// NULL is returned if no mapping exists.
const struct mgp_value *mgp_map_at(const struct mgp_map *map, const char *key);

/// An item in the mgp_map.
struct mgp_map_item;

/// Get the key of the mapped item.
const char *mgp_map_item_key(const struct mgp_map_item *item);

/// Get the value of the mapped item.
const struct mgp_value *mgp_map_item_value(const struct mgp_map_item *item);

/// An iterator over the items in mgp_map.
struct mgp_map_items_iterator;

/// Start iterating over items contained in the given map.
/// The returned mgp_map_items_iterator needs to be deallocated with
/// mgp_map_items_iterator_destroy.
/// NULL is returned if unable to allocate a new iterator.
struct mgp_map_items_iterator *mgp_map_iter_items(const struct mgp_map *map);

/// Deallocate memory used by mgp_map_items_iterator.
void mgp_map_items_iterator_destroy(struct mgp_map_items_iterator *it);

/// Get the current item pointed to by the iterator.
/// When the mgp_map_items_iterator_next is invoked, the returned pointer
/// to mgp_map_item becomes invalid. On the other hand, pointers obtained
/// with mgp_map_item_key and mgp_map_item_value remain valid
/// throughout the lifetime of a map. Therefore, you can store the key as well
/// as the value before, and use them after invoking
/// mgp_map_items_iterator_next.
/// NULL is returned if the end of the iteration has been reached.
const struct mgp_map_item *mgp_map_items_iterator_get(
    const struct mgp_map_items_iterator *it);

/// Advance the iterator to the next item stored in map and return it.
/// The previous pointer obtained through mgp_map_items_iterator_get will
/// be invalidated, but the pointers to key and value will remain valid.
/// NULL is returned if the end of the iteration has been reached.
const struct mgp_map_item *mgp_map_items_iterator_next(
    struct mgp_map_items_iterator *it);

/// Create a path with the copy of the given starting vertex.
/// You need to free the created instance with mgp_path_destroy.
/// NULL is returned if unable to allocate a path.
struct mgp_path *mgp_path_make_with_start(const struct mgp_vertex *vertex);

/// Free the memory used by the given mgp_path and contained vertices and edges.
void mgp_path_destroy(struct mgp_path *path);

/// Append an edge continuing from the last vertex on the path.
/// The edge is copied into the path. Therefore, the path does not take
/// ownership of the original edge, so you still need to free the edge memory
/// explicitly.
/// The last vertex on the path will become the other endpoint of the given
/// edge, as continued from the current last vertex.
/// Return non-zero on success, or 0 if the current last vertex in the path is
/// not part of the given edge. 0 is also returned if unable to allocate memory
/// for path extension.
int mgp_path_expand(struct mgp_path *path, const struct mgp_edge *edge);

/// Return the number of edges in a mgp_path.
size_t mgp_path_size(const struct mgp_path *path);

/// Return the vertex from a path at given index.
/// The valid index range is [0, mgp_path_size].
/// NULL is returned if index is out of range.
const struct mgp_vertex *mgp_path_vertex_at(const struct mgp_path *path,
                                            size_t index);

/// Return the edge from a path at given index.
/// The valid index range is [0, mgp_path_size - 1].
/// NULL is returned if index is out of range.
const struct mgp_edge *mgp_path_edge_at(const struct mgp_path *path,
                                        size_t index);
///@}

/// @name Procedure Result
/// These functions and types are used to set the result of your custom
/// procedure.
///@{

/// Stores either an error result or a list of mgp_result_record instances.
struct mgp_result;
/// Represents a record of resulting field values.
struct mgp_result_record;

/// Set the error as the result of the procedure.
/// If there's no memory for copying the error message, 0 is returned.
int mgp_result_set_error_msg(struct mgp_result *res, const char *error_msg);

/// Create a new record for results.
/// Return NULL if unable to allocate a mgp_result_record.
struct mgp_result_record *mgp_result_new_record(struct mgp_result *res);

/// Assign a value to a field in the given record.
/// Return 0 if there's no memory to copy the mgp_value to mgp_result_record or
/// if the combination of `field_name` and `val` does not satisfy the
/// procedure's result signature.
int mgp_result_record_insert(struct mgp_result_record *record,
                             const char *field_name,
                             const struct mgp_value *val);
///@}

/// @name Graph Constructs
///@{

/// Label of a vertex.
struct mgp_label {
  /// Name of the label as a NULL terminated character string.
  const char *name;
};

/// Type of an edge.
struct mgp_edge_type {
  /// Name of the type as a NULL terminated character string.
  const char *name;
};

/// Iterator over property values of a vertex or an edge.
struct mgp_properties_iterator;

/// Free the memory used by a mgp_properties_iterator.
void mgp_properties_iterator_destroy(struct mgp_properties_iterator *it);

/// Reference to a named property value.
struct mgp_property {
  /// Name (key) of a property as a NULL terminated string.
  const char *name;
  /// Value of the referenced property.
  const struct mgp_value *value;
};

/// Get the current property pointed to by the iterator.
/// When the mgp_properties_iterator_next is invoked, the previous
/// mgp_property is invalidated and its value must not be used.
/// NULL is returned if the end of the iteration has been reached.
const struct mgp_property *mgp_properties_iterator_get(
    const struct mgp_properties_iterator *it);

/// Advance the iterator to the next property and return it.
/// The previous mgp_property obtained through mgp_properties_iterator_get
/// will be invalidated, and you must not use its value.
/// NULL is returned if the end of the iteration has been reached.
const struct mgp_property *mgp_properties_iterator_next(
    struct mgp_properties_iterator *it);

/// Iterator over edges of a vertex.
struct mgp_edges_iterator;

/// Free the memory used by a mgp_edges_iterator.
void mgp_edges_iterator_destroy(struct mgp_edges_iterator *it);

/// Copy a mgp_vertex.
/// Returned pointer must be freed with mgp_vertex_destroy.
/// NULL is returned if unable to allocate a mgp_vertex.
struct mgp_vertex *mgp_vertex_copy(const struct mgp_vertex *v);

/// Free the memory used by a mgp_vertex.
void mgp_vertex_destroy(struct mgp_vertex *v);

/// Return non-zero if given vertices are equal, otherwise 0.
int mgp_vertex_equal(const struct mgp_vertex *v1, const struct mgp_vertex *v2);

/// Return the number of labels a given vertex has.
size_t mgp_vertex_labels_count(const struct mgp_vertex *v);

/// Return mgp_label in mgp_vertex at given index.
/// If the index is out of bounds, mgp_label.name is set to NULL.
struct mgp_label mgp_vertex_label_at(const struct mgp_vertex *v, size_t index);

/// Return non-zero if the given vertex has the given label.
int mgp_vertex_has_label(const struct mgp_vertex *v, struct mgp_label label);

/// Return non-zero if the given vertex has a label with given name.
int mgp_vertex_has_label_named(const struct mgp_vertex *v,
                               const char *label_name);

/// Get a copy of a vertex property mapped to a given name.
/// Returned value must be freed with mgp_value_destroy.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_vertex_get_property(const struct mgp_vertex *v,
                                          const char *property_name);

/// Start iterating over properties stored in the given vertex.
/// The returned mgp_properties_iterator needs to be deallocated with
/// mgp_properties_iterator_destroy.
/// NULL is returned if unable to allocate a new iterator.
struct mgp_properties_iterator *mgp_vertex_iter_properties(
    const struct mgp_vertex *v);

/// Start iterating over inbound edges of the given vertex.
/// The returned mgp_edges_iterator needs to be deallocated with
/// mgp_edges_iterator_destroy.
/// NULL is returned if unable to allocate a new iterator.
struct mgp_edges_iterator *mgp_vertex_iter_in_edges(const struct mgp_vertex *v);

/// Start iterating over outbound edges of the given vertex.
/// The returned mgp_edges_iterator needs to be deallocated with
/// mgp_edges_iterator_destroy.
/// NULL is returned if unable to allocate a new iterator.
struct mgp_edges_iterator *mgp_vertex_iter_out_edges(
    const struct mgp_vertex *v);

/// Get the current edge pointed to by the iterator.
/// When the mgp_edges_iterator_next is invoked, the previous
/// mgp_edge is invalidated and its value must not be used.
/// NULL is returned if the end of the iteration has been reached.
const struct mgp_edge *mgp_edges_iterator_get(
    const struct mgp_edges_iterator *it);

/// Advance the iterator to the next edge and return it.
/// The previous mgp_edge obtained through mgp_edges_iterator_get
/// will be invalidated, and you must not use its value.
/// NULL is returned if the end of the iteration has been reached.
const struct mgp_edge *mgp_edges_iterator_next(struct mgp_edges_iterator *it);

/// Copy a mgp_edge.
/// Returned pointer must be freed with mgp_edge_destroy.
/// NULL is returned if unable to allocate a mgp_edge.
struct mgp_edge *mgp_edge_copy(const struct mgp_edge *e);

/// Free the memory used by a mgp_edge.
void mgp_edge_destroy(struct mgp_edge *e);

/// Return the type of the given edge.
struct mgp_edge_type mgp_edge_get_type(const struct mgp_edge *e);

/// Return the source vertex of the given edge.
const struct mgp_vertex *mgp_edge_get_from(const struct mgp_edge *e);

/// Return the destination vertex of the given edge.
const struct mgp_vertex *mgp_edge_get_to(const struct mgp_edge *e);

/// Get a copy of a edge property mapped to a given name.
/// Returned value must be freed with mgp_value_destroy.
/// NULL is returned if unable to allocate a mgp_value.
struct mgp_value *mgp_edge_get_property(const struct mgp_edge *e,
                                        const char *property_name);

/// Start iterating over properties stored in the given edge.
/// The returned mgp_properties_iterator needs to be deallocated with
/// mgp_properties_iterator_destroy.
/// NULL is returned if unable to allocate a new iterator.
struct mgp_properties_iterator *mgp_edge_iter_properties(
    const struct mgp_edge *e);

/// State of the graph database.
struct mgp_graph;

/// Iterator over vertices.
struct mgp_vertices_iterator;

/// Free the memory used by a mgp_vertices_iterator.
void mgp_vertices_iterator_destroy(struct mgp_vertices_iterator *it);

/// Start iterating over vertices of the given graph.
/// The returned mgp_vertices_iterator needs to be deallocated with
/// mgp_vertices_iterator_destroy.
/// NULL is returned if unable to allocate a new iterator.
struct mgp_vertices_iterator *mgp_graph_iter_vertices(
    const struct mgp_graph *g);

/// Get the current vertex pointed to by the iterator.
/// When the mgp_vertices_iterator_next is invoked, the previous
/// mgp_vertex is invalidated and its value must not be used.
/// NULL is returned if the end of the iteration has been reached.
const struct mgp_vertex *mgp_vertices_iterator_get(
    const struct mgp_vertices_iterator *it);

/// Advance the iterator to the next vertex and return it.
/// The previous mgp_vertex obtained through mgp_vertices_iterator_get
/// will be invalidated, and you must not use its value.
/// NULL is returned if the end of the iteration has been reached.
const struct mgp_vertex *mgp_vertices_iterator_next(
    struct mgp_vertices_iterator *it);
///@}

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // MG_PROCEDURE_H
