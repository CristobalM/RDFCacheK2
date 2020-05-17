//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_FEEDDATATRIPLEITERATOR_HPP
#define RDFCACHEK2_FEEDDATATRIPLEITERATOR_HPP

struct FeedDataTripleIterator {
  using value_type = const RDFTriple;
  using reference = const RDFTriple &;
  using pointer = const RDFTriple *;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;

  FeedDataTripleIterator(pointer ptr) : ptr_(ptr) {}

  FeedDataTripleIterator operator++() {
    FeedDataTripleIterator it = *this;
    ptr_++;
    return it;
  }

  const FeedDataTripleIterator operator++(int) {
    ptr_++;
    return *this;
  }

  reference operator*() { return *ptr_; }

  pointer operator->() { return ptr_; }

  bool operator==(const FeedDataTripleIterator &rhs) {
    return ptr_ == rhs.ptr_;
  }

  bool operator!=(const FeedDataTripleIterator &rhs) {
    return ptr_ != rhs.ptr_;
  }

private:
  pointer ptr_;
};

#endif // RDFCACHEK2_FEEDDATATRIPLEITERATOR_HPP
