//
// Created by cristobal on 7/11/21.
//

#include "ResultTableIteratorBGP.hpp"
ResultTableIteratorBGP::ResultTableIteratorBGP(
    std::vector<std::unique_ptr<BGPOp>> &&bgp_ops,
    std::vector<unsigned long> &headers)
    : bgp_ops(std::move(bgp_ops)), headers(headers), next_available(false),
      tmp_holder(headers.size(), 0), s_i(0),
      finished_ops(headers.size(), false) {
  next_concrete();
}

bool ResultTableIteratorBGP::has_next() { return next_available; }

std::vector<unsigned long> ResultTableIteratorBGP::next() {
  return next_concrete();
}

void ResultTableIteratorBGP::ops_until_last() {
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
std::vector<unsigned long> &ResultTableIteratorBGP::get_headers() {
  return headers;
}

void ResultTableIteratorBGP::reset_iterator() {
  for (auto &bgp_op : bgp_ops) {
    bgp_op->reset_op();
  }
  next();
}
std::vector<unsigned long> ResultTableIteratorBGP::next_concrete() {
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
