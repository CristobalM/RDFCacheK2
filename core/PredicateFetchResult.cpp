//
// Created by cristobal on 5/17/21.
//
#include "PredicateFetchResult.hpp"
PredicateFetchResult::PredicateFetchResult(bool does_exist, K2TreeMixed *k2tree)
    : does_exist(does_exist), result(k2tree) {}

const K2TreeMixed &PredicateFetchResult::get() const { return *result; }
bool PredicateFetchResult::exists() const { return does_exist; }
K2TreeMixed &PredicateFetchResult::get_mutable() { return *result; }
K2TreeMixed *PredicateFetchResult::get_ptr() { return result; }
