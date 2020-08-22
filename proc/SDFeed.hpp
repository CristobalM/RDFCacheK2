//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SDFEED_HPP
#define RDFCACHEK2_SDFEED_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

static constexpr char FINISHED_VAL[] = "||<<<FINISHED>>>||";

class SDFeed {
  std::mutex m;
  std::condition_variable cv;

  std::queue<std::string> q;

  bool finished;

public:
  SDFeed();

  void push(const std::string &value);
  std::string pop();
  std::string top();

  bool empty();
  size_t size();

  std::string get_string();
  bool has_finished();
  void set_finished(bool new_value);

private:
  std::string _pop_nolock();
};

#endif // RDFCACHEK2_SDFEED_HPP
