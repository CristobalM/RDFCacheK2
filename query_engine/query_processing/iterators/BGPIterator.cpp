//
// Created by cristobal on 7/11/21.
//

#include "BGPIterator.hpp"
#include <TimeControl.hpp>
namespace k2cache {

BGPIterator::BGPIterator(std::vector<std::unique_ptr<BGPOp>> &&bgp_ops,
                         std::vector<unsigned long> &headers,
                         TimeControl &time_control)
    : time_control(time_control), bgp_ops(std::move(bgp_ops)), headers(headers),
      next_available(false), tmp_holder(headers.size(), 0), s_i(0),
      finished_ops((this->bgp_ops).size(), false) {
  next_concrete();
}

bool BGPIterator::has_next() { return next_available; }

std::vector<unsigned long> BGPIterator::next() { return next_concrete(); }

void BGPIterator::ops_until_last() {
  if (!time_control.tick())
    return;
  BGPOp::RunResult run_result{};
  run_result.valid_value = false;
  while (s_i < (long)(bgp_ops.size() - 1) && s_i >= 0) {
    while (!run_result.valid_value) {
      auto &bgp_op = bgp_ops[s_i];

      if (finished_ops[s_i]) {
        bgp_op->reset_op();
        finished_ops[s_i] = false;
        s_i--;
        break;
      }

      run_result = bgp_op->run(tmp_holder);
      if (!time_control.tick())
        return;

      if (run_result.scan_done) {
        finished_ops[s_i] = true;
      }

      if (run_result.valid_value) {
        s_i++;
        run_result.valid_value = false;
        break;
      }
    }
  }
}

std::vector<unsigned long> &BGPIterator::get_headers() { return headers; }

void BGPIterator::reset_iterator() {
  for (auto &bgp_op : bgp_ops) {
    bgp_op->reset_op();
  }
  next_concrete();
}

std::vector<unsigned long> BGPIterator::next_concrete() {
  if (!time_control.tick())
    return next_value;
  auto result = next_value;
  next_available = false;

  BGPOp::RunResult run_result{};
  run_result.valid_value = false;
  while (!run_result.valid_value) {
    ops_until_last();
    if (s_i < 0) {
      next_available = false;
      return result;
    }

    auto &last_op = bgp_ops[s_i];
    if (finished_ops[s_i]) {
      last_op->reset_op();
      finished_ops[s_i] = false;
      s_i--;
      continue;
    }

    run_result = last_op->run(tmp_holder);
    if (!time_control.tick())
      return result;

    if (run_result.scan_done) {
      finished_ops[s_i] = true;
    }
    if (run_result.valid_value) {
      next_value = tmp_holder;
      next_available = true;
      return result;
    }
  }

  return result;
}

}

