//
// Created by cristobal on 5/31/21.
//

#include "OrderNodeProcessor.hpp"
#include <query_processing/expr/StringHandlingUtil.hpp>

std::shared_ptr<ResultTable> OrderNodeProcessor::execute() {

  auto permutation = get_permutation();

  current_table->data.sort([this, &permutation](const ResultTable::vul_t &lhs,
                                                const ResultTable::vul_t &rhs) {
    for (int i = 0; i < node.sort_conditions_size(); i++) {
      auto col_pos = permutation[i];
      auto left_id = lhs[col_pos];
      auto right_id = rhs[col_pos];
      auto left_res = cm.extract_resource(left_id);
      auto right_res = cm.extract_resource(right_id);

      auto left_cmp_str = extract_cmp_str_from_resource(left_res);
      auto right_cmp_str = extract_cmp_str_from_resource(right_res);

      const auto &sort_condition = node.sort_conditions(i);
      int cmp = std::strcmp(left_cmp_str.c_str(), right_cmp_str.c_str());
      if (cmp != 0) {
        if (sort_condition.direction() == proto_msg::SortDirection::ASCENDING) {
          return cmp < 0;
        } else if (sort_condition.direction() ==
                   proto_msg::SortDirection::DESCENDING) {
          return cmp > 0;
        }
      }
    }
    return false;
  });

  return current_table;
}

OrderNodeProcessor::OrderNodeProcessor(std::shared_ptr<ResultTable> input_table,
                                       const proto_msg::OrderNode &node,
                                       const PredicatesCacheManager &cm,
                                       VarIndexManager &vim)
    : current_table(std::move(input_table)), node(node), cm(cm), vim(vim) {}
std::vector<int> OrderNodeProcessor::get_permutation() {
  std::vector<int> permutation(node.sort_conditions_size(), 0);

  for (int i = 0; i < node.sort_conditions_size(); i++) {
    const auto &sort_condition = node.sort_conditions(i);
    auto var_index = vim.var_indexes[sort_condition.var()];
    for (size_t j = 0; j < current_table->headers.size(); j++) {
      if (current_table->headers[j] == var_index) {
        permutation[i] = j;
      }
    }
  }

  return permutation;
}
std::string
OrderNodeProcessor::extract_cmp_str_from_resource(const RDFResource &resource) {
  if (resource.resource_type == RDFResourceType::RDF_TYPE_IRI)
    return resource.value;
  if (resource.resource_type == RDFResourceType::RDF_TYPE_BLANK)
    return resource.value;
  return StringHandlingUtil::extract_literal_data_from_rdf_resource(resource)
      .value;
}
