#pragma once

#include "data_structures/concurrent/common.hpp"
#include "data_structures/concurrent/skiplist.hpp"

using std::pair;

template <typename K, typename T>
class ConcurrentMap
{
  typedef Item<K, T> item_t;
  typedef SkipList<item_t> list;
  typedef typename SkipList<item_t>::Iterator list_it;
  typedef typename SkipList<item_t>::ConstIterator list_it_con;

  public:
  ConcurrentMap() {}

  class Accessor : public AccessorBase<item_t>
  {
    friend class ConcurrentMap;

    using AccessorBase<item_t>::AccessorBase;

private:
    using AccessorBase<item_t>::accessor;

public:
    std::pair<list_it, bool> insert(const K &key, const T &data)
    {
      return accessor.insert(item_t(key, data));
    }

    std::pair<list_it, bool> insert(const K &key, T &&data)
    {
      return accessor.insert(item_t(key, std::forward<T>(data)));
    }

    std::pair<list_it, bool> insert(K &&key, T &&data)
    {
      return accessor.insert(
          item_t(std::forward<K>(key), std::forward<T>(data)));
    }

    list_it_con find(const K &key) const { return accessor.find(key); }

    list_it find(const K &key) { return accessor.find(key); }

    bool contains(const K &key) const { return this->find(key) != this->end(); }

    bool remove(const K &key) { return accessor.remove(key); }
  };

  Accessor access() { return Accessor(&skiplist); }

  const Accessor access() const { return Accessor(&skiplist); }

  private:
  list skiplist;
};
