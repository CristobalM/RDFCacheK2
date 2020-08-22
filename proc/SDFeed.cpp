//
// Created by Cristobal Miranda, 2020
//

#include "SDFeed.hpp"

void SDFeed::push(const std::string &value) {
  {
    std::lock_guard<std::mutex> lg(m);
    q.push(value);
  }
  cv.notify_all();
}

std::string SDFeed::pop() {
  std::lock_guard<std::mutex> lg(m);
  return _pop_nolock();
}

bool SDFeed::empty() {
  std::lock_guard<std::mutex> lg(m);
  return q.empty();
}

size_t SDFeed::size() {
  std::lock_guard<std::mutex> lg(m);
  return q.size();
}

std::string SDFeed::top() {
  std::lock_guard<std::mutex> lg(m);
  return q.front();
}

SDFeed::SDFeed() : finished(false) {}

std::string SDFeed::get_string() {
  std::unique_lock<std::mutex> lk(m);
  cv.wait(lk, [this] { return !q.empty() || finished; });

  if (finished && q.empty()) {
    return FINISHED_VAL;
  }

  auto result = _pop_nolock();
  lk.unlock();
  // cv.notify_all();
  return result;
}

bool SDFeed::has_finished() {
  std::unique_lock<std::mutex> lk(m);
  cv.wait(lk, [this] { return !q.empty() || finished; });
  return this->q.empty() && finished;
}

std::string SDFeed::_pop_nolock() {
  auto result = q.front();
  q.pop();
  return result;
}

void SDFeed::set_finished(bool new_value) {
  {
    std::lock_guard<std::mutex> lg(m);
    finished = new_value;
  }
  cv.notify_all();
}
