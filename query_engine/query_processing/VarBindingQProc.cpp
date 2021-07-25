//
// Created by cristobal on 21-07-21.
//

#include "VarBindingQProc.hpp"
bool VarBindingQProc::is_bound(unsigned long id) {
  return mapping.find(id) != mapping.end();
}
unsigned long VarBindingQProc::get_value(unsigned long id) {
  return mapping[id];
}
void VarBindingQProc::bind(unsigned long var_id, unsigned long value_id) {
  mapping[var_id] = value_id;
}
