#ifndef RDFCACHEK2_FOUR_SIDED_QUEUE_HPP
#define RDFCACHEK2_FOUR_SIDED_QUEUE_HPP

#include <cassert>
#include <cstddef>
#include <mutex>
#include <unordered_map>

template <typename T> struct FSNode {
  T item;
  FSNode *top, *down;
  FSNode *left, *right;
  unsigned long id;
  FSNode(T item, unsigned long id)
      : item(item), top(nullptr), down(nullptr), left(nullptr), right(nullptr),
        id(id) {}
};

template <typename T> struct VerticalQueue {
  using node_t = FSNode<T>;
  node_t *tail, *head;
  size_t vsize;
  VerticalQueue() : tail(nullptr), head(nullptr), vsize(0) {}
};

template <typename T> class FourSidedQueue {
  using node_t = FSNode<T>;

  node_t *tail, *head;
  size_t qsize;

  using vm_t = std::unordered_map<unsigned long,
                                  VerticalQueue<T>>; // vertical mapping type
  using lg_t = std::lock_guard<std::mutex>;
  vm_t vertical_q;
  std::mutex m;

public:
  FourSidedQueue() : tail(nullptr), head(nullptr), qsize(0) {}
  FourSidedQueue(const FourSidedQueue &other) = delete;

  FourSidedQueue(FourSidedQueue &&other) {
    auto lg = acquire_other(other);
    std::swap(tail, other.tail);
    std::swap(head, other.head);
    qsize = 0;
    std::swap(qsize, other.size);
    std::swap(vertical_q, other.vertical_q);
  }

  ~FourSidedQueue() {
    node_t *current = tail;
    while (current) {
      node_t *next = current->right;
      delete current;
      current = next;
    }
  }

  FourSidedQueue &operator=(FourSidedQueue &&rhs) {
    auto lg = acquire_other(rhs);
    std::swap(tail, rhs.tail);
    std::swap(head, rhs.head);
    qsize = 0;
    std::swap(qsize, rhs.size);
    std::swap(vertical_q, rhs.vertical_q);
  }

  bool is_vertical_empty(unsigned long id) {
    lg_t lg(m);
    return _is_vertical_empty(id);
  }

  size_t vertical_size(unsigned long id) {
    lg_t lg(m);
    if (!has_vertical_q(id))
      return 0;
    auto &vq = vertical_q[id];
    return vq.vsize;
  }

  T pop_next(unsigned long id) {
    lg_t lg(m);
    return _pop_next(id);
  }

  T pop_front_vertical(unsigned long id) {
    lg_t lg(m);
    return _pop_front_vertical(id);
  }

  T &front_vertical(unsigned long id) {
    lg_t lg(m);
    assert(has_vertical_q(id));
    auto &vq = vertical_q[id];
    return vq.head.item;
  }

  T pop_back_vertical(unsigned long id) {
    lg_t lg(m);
    assert(has_vertical_q(id));
    auto &vq = vertical_q[id];
    assert(vq.tail);
    T result = vq.tail->item;
    node_t *next_tail = vq.tail->top;
    if (next_tail)
      next_tail->down = nullptr;
    remove_from_horizontal_queue(vq.tail);
    delete vq.tail;
    vq.tail = next_tail;
    if (!next_tail)
      vq.head = nullptr;
    qsize--;
    vq.vsize--;
    return result;
  }

  T &back_vertical(unsigned long id) {
    lg_t lg(m);
    assert(has_vertical_q(id));
    auto &vq = vertical_q[id];
    return vq.tail.item;
  }

  void push_front(T item, unsigned long id) {
    lg_t lg(m);
    node_t *node = new node_t(item, id);
    horizontal_push_front(node);
    vertical_push_front(node);
    qsize++;
  }

  T pop_front() {
    lg_t lg(m);
    return _pop_front();
  }

  T &front() {
    lg_t lg(m);
    assert(head != nullptr);
    T result = head->item;
    return result;
  }

  void push_back(T item, unsigned long id) {
    lg_t lg(m);
    node_t *node = new node_t(item, id);
    horizontal_push_back(node);
    vertical_push_back(node);
    qsize++;
  }

  T pop_back() {
    lg_t lg(m);
    T result = tail->item;
    node_t *next_tail = tail->right;
    if (next_tail)
      next_tail->left = nullptr;
    remove_from_vertical_queue(tail);
    delete tail;
    tail = next_tail;
    if (!next_tail)
      head = nullptr;
    qsize--;
    return result;
  }

  T &back() {
    lg_t lg(m);
    T result = tail->item;
    return result;
  }

  size_t size() {
    lg_t lg(m);
    return qsize;
  }

  bool empty() {
    lg_t lg(m);
    return qsize == 0;
  }

  lg_t acquire_other(FourSidedQueue &other) { return std::lock_guard(other.m); }

private:
  T _pop_next(unsigned long id) {
    if (!_is_vertical_empty(id))
      return _pop_front_vertical(id);
    return _pop_front();
  }

  bool _is_vertical_empty(unsigned long id) {
    if (!has_vertical_q(id))
      return true;
    auto &vq = vertical_q[id];
    return !vq.head || !vq.tail;
  }

  T _pop_front_vertical(unsigned long id) {
    assert(has_vertical_q(id));
    auto &vq = vertical_q[id];
    assert(vq.head);
    T result = vq.head->item;
    node_t *next_head = vq.head->down;
    if (next_head)
      next_head->top = nullptr;
    remove_from_horizontal_queue(vq.head);
    delete vq.head;
    vq.head = next_head;
    if (!next_head)
      vq.tail = nullptr;
    qsize--;
    vq.vsize--;
    return result;
  }

  T _pop_front() {
    assert(head);
    T result = head->item;
    node_t *next_head = head->left;
    if (next_head)
      next_head->right = nullptr;
    remove_from_vertical_queue(head);
    delete head;
    head = next_head;
    if (!next_head)
      tail = nullptr;
    qsize--;
    return result;
  }

  void create_vertical_queue_if_not_exist(unsigned long id) {
    if (!has_vertical_q(id)) {
      vertical_q[id] = VerticalQueue<T>();
    }
  }

  void remove_from_vertical_queue(node_t *node) {
    node_t *top = node->top;
    node_t *down = node->down;
    assert(has_vertical_q(node->id));
    auto &vq = vertical_q[node->id];
    vq.vsize--;
    if (top)
      top->down = node->down;
    else {
      vq.head = down;
    }
    if (down)
      down->top = node->top;
    else {
      vq.tail = top;
    }
  }

  void remove_from_horizontal_queue(node_t *node) {
    node_t *left = node->left;
    node_t *right = node->right;
    if (left)
      left->right = right;
    else {
      tail = right;
    }
    if (right)
      right->left = left;
    else {
      head = left;
    }
  }

  void vertical_push_front(node_t *node) {
    create_vertical_queue_if_not_exist(node->id);
    VerticalQueue<T> &vq = vertical_q[node->id];
    vq.vsize++;
    if (!vq.tail)
      vq.tail = node;
    if (!vq.head)
      vq.head = node;
    else {
      node->down = vq.head;
      vq.head->top = node;
      vq.head = node;
    }
  }

  void vertical_push_back(node_t *node) {
    create_vertical_queue_if_not_exist(node->id);
    VerticalQueue<T> &vq = vertical_q[node->id];
    vq.vsize++;
    if (!vq.tail)
      vq.tail = node;
    else {
      node->top = vq.tail;
      vq.tail->down = node;
      vq.tail = node;
    }
    if (!vq.head)
      vq.head = node;
  }

  void horizontal_push_front(node_t *node) {
    if (!tail)
      tail = node;
    if (!head)
      head = node;
    else {
      node->left = head;
      head->right = node;
      head = node;
    }
  }

  void horizontal_push_back(node_t *node) {
    if (!tail)
      tail = node;
    else {
      node->right = tail;
      tail->left = node;
      tail = node;
    }
    if (!head)
      head = node;
  }

  bool has_vertical_q(unsigned long id) {
    return vertical_q.find(id) != vertical_q.end();
  }
};

#endif
