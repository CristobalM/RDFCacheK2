#ifndef RDFCACHEK2_QUICK_SORT_STOPPABLE_HPP_
#define RDFCACHEK2_QUICK_SORT_STOPPABLE_HPP_

#include <algorithm>
#include <random>
#include <vector>

namespace {

template <typename T, typename Comparator, typename TC>
static int inplace_quicksort_partition(std::vector<T> &data,
                                       Comparator &comparator, int start,
                                       int end, TC &time_control) {
  int left = start - 1;
  auto &pivot_value = data[end - 1];
  for (int right = start; right < end - 1; right++) {
    if (!time_control.tick())
      return left + 1;
    if (comparator(data[right], pivot_value)) {
      left++;
      std::swap(data[left], data[right]);
    }
  }
  std::swap(pivot_value, data[left + 1]);
  return left + 1;
}

template <typename T, typename Comparator, typename TC>
static int inplace_quicksort_partition_random(
    std::vector<T> &data, Comparator &comparator, int start, int end,
    std::uniform_int_distribution<int> &random_distr, std::mt19937 &generator,
    TC &time_control) {
  int pivot = (random_distr(generator) % (end - start)) + start;
  std::swap(data[pivot], data[end - 1]);
  return inplace_quicksort_partition(data, comparator, start, end,
                                     time_control);
}

template <typename T, typename Comparator, typename TC>
void inplace_quicksort(std::vector<T> &data, Comparator &comparator, int start,
                       int end,
                       std::uniform_int_distribution<int> &random_distr,
                       std::mt19937 &generator, TC &time_control) {
  if (start >= end)
    return;
  if (!time_control.tick())
    return;
  int pivot = inplace_quicksort_partition_random(
      data, comparator, start, end, random_distr, generator, time_control);
  if (!time_control.tick())
    return;
  inplace_quicksort(data, comparator, start, pivot, random_distr, generator,
                    time_control);
  if (!time_control.tick())
    return;
  inplace_quicksort(data, comparator, pivot + 1, end, random_distr, generator,
                    time_control);
  if (!time_control.tick())
    return;
}

} // namespace

template <typename T, typename Comparator, typename TC>
void inplace_quicksort_stoppable(std::vector<T> &data, Comparator &comparator,
                                 int start, int end, TC &time_control) {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution random_distr(start, end);
  return inplace_quicksort(data, comparator, start, end, random_distr,
                           generator, time_control);
}

template <typename T, typename Comparator, typename TC>
void inplace_quicksort_stoppable(std::vector<T> &data, Comparator comparator,
                                 TC &time_control) {
  inplace_quicksort_stoppable(data, comparator, 0, data.size(), time_control);
}

#endif /* RDFCACHEK2_QUICK_SORT_STOPPABLE_HPP_ */