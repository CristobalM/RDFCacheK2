#ifndef RDFCACHEK2_MAPOFQUEUES_HPP
#define RDFCACHEK2_MAPOFQUEUES_HPP

#include <cassert>
#include <cstddef>
#include <deque>
#include <mutex>
#include <unordered_map>

template <typename T> class MapOfQueues {
public:
  using map_t = std::unordered_map<unsigned long, std::deque<T>>;

private:
  map_t map;
  std::mutex m;

  using lg_t = std::lock_guard<std::mutex>;

  size_t total_size;

public:
  MapOfQueues() : total_size(0) {}

  map_t &get_map() { return map; }

  void push_back(T item, unsigned long id) {
    lg_t lg(m);
    create_queue_if_not_exist(id);
    map[id].push_back(item);
    total_size++;
  }

  void push_front(T item, unsigned long id) {
    lg_t lg(m);
    create_queue_if_not_exist(id);
    map[id].push_front(item);
    total_size++;
  }

  T pop_back(unsigned long id) {
    lg_t lg(m);
    assert(!_empty_at(id));
    auto result = map[id].back();
    map[id].pop_back();
    total_size--;
    return result;
  }

  T pop_front(unsigned long id) {
    lg_t lg(m);
    assert(!_empty_at(id));
    auto result = map[id].back();
    map[id].pop_front();
    total_size--;
    return result;
  }

  bool empty() {
    lg_t lg(m);
    return total_size == 0;
  }

  size_t size() {
    lg_t lg(m);
    return total_size;
  }

  bool empty_at(unsigned long id) {
    lg_t lg(m);
    return _empty_at(id);
  }

  size_t size_at(unsigned long id) {
    lg_t lg(m);
    return _size_at(id);
  }

  void clear() {
    lg_t lg(m);
    map.clear();
    total_size = 0;
  }

private:
  bool _empty_at(unsigned long id) const {
    if (!queue_exist(id))
      return true;
    return map.at(id).empty();
  }

  size_t _size_at(unsigned long id) const {
    if (!queue_exist(id))
      return 0;
    return map.at(id).size();
  }

  void create_queue_if_not_exist(unsigned long id) {
    if (!queue_exist(id))
      map[id] = std::deque<T>();
  }

  bool queue_exist(unsigned long id) const { return map.find(id) != map.end(); }
};

#endif