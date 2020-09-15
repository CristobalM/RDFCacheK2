
#include <cstddef>
#include <mutex>
#include <unordered_map>

template <typename T> struct FSNode {
  T *item;
  FSNode *top, *down;
  FSNode *left, *right;
  FSNode(T *item)
      : item(item), top(nullptr), down(nullptr), left(nullptr), right(nullptr) {
  }
};

template <typename T> struct VerticalQueue{
  using node_t = FSNode<T>;
  node_t *tail, *head;
  VerticalQueue() : tail(nullptr), head(nullptr) {}
};

template <typename T> class FourSidedQueue {
  using node_t = FSNode<T>;

  node_t *tail, *head;
  size_t qsize;

  using vm_t = std::unordered_map<unsigned long, VerticalQueue>; // vertical mapping type
  vm_t vertical_q;
  std::mutex m;

public:
  FourSidedQueue() : tail(nullptr), head(nullptr), qsize(0) {}
  FourSidedQueue(const FourSidedQueue &other) = delete;
  FourSidedQueue &operator=(const FourSidedQueue &rhs) = delete;

  FourSidedQueue(FourSidedQueue &&other) { auto lg = acquire_other(other); }
  ~FourSidedQueue() {}
  
  FourSidedQueue &operator=(FourSidedQueue &&rhs) {
    auto lg = acquire_other(rhs);
  }

  T *pop_front_vertical(unsigned long id){
    std::lock_guard lg(m);
    

  }

  T *pop_front() {
    std::lock_guard lg(m);
    T *result = head->item;
    node_t *next_head = head->left;
    if (next_head)
      next_head->right = nullptr;
    remove_from_vertical_queue(head);
    delete head;
    head = next_head;
    return result;
  }

  T &front() {
    std::lock_guard lg(m);
    T *result = head->item;
    return *item;
  }

  void push_front(T *item, unsigned long id) {
    std::lock_guard lg(m);
    node_t *node = new node_t(item);
    horizontal_push_front(node);
    vertical_push_front(node, id);
  }

  void horizontal_push_front(node_t *node){
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

  void create_vertical_queue_if_not_exist(unsigned long id){
    if(vertical_q.find(id) == vertical_q.end()){
      vertical_q[id] = vm_t();
    }
  }

  void vertical_push_front(node_t *node, unsigned long id){
    create_vertical_queue_if_not_exist(id);
    vm_t &vq = vertical_q[id];
    if(!vq.tail) vq.tail = node;
    if(!vq.head) vq.head = node;
    else{
      node->down = vq.head;
      vq.head->top = node;
      vq.head = node;
    }
  }


  T *pop_back() {
    std::lock_guard lg(m);
    T *result = tail->item;
    node_t *next_tail = tail->right;
    if (next_tail)
      next_tail->left = nullptr;
    remove_from_vertical_queue(tail);
    delete tail;
    tail = next_tail;
    return result;
  }

  T &back() {
    std::lock_guard lg(m);
    T *result = tail->item;
    return *result;
  }

  void push_back(T *item, unsigned long id) {
    std::lock_guard lg(m);
    node_t *node = new node_t(item);
    horizontal_push_back(node);
    vertical_push_back(node, id);
  }

  void horizontal_push_back(node_t *node){
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


  void vertical_push_back(node_t *node, unsigned long id){
    create_vertical_queue_if_not_exist(id);
    vm_t &vq = vertical_q[id];
    if(!vq.tail) vq.tail = node;
    else{
      node->top = vq.tail;
      vq.tail->down = node;
      vq.tail = node;
    }
    if(!vq.head) vq.head = node;
  }


  size_t size() {
    std::lock_guard lg(m);
    return qsize;
  }

  std::lock_guard acquire_other(FourSidedQueue &other) {
    return std::lock_guard(other.m);
  }

  void remove_from_vertical_queue(node_t *node) {
    node_t *top = node->top;
    node_t *down = node->down;
    if (top)
      top->down = node->down;
    if (down)
      down->top = node->top;
  }
};