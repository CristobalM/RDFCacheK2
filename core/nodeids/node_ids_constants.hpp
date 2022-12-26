//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODE_IDS_CONSTANTS_HPP
#define RDFCACHEK2_NODE_IDS_CONSTANTS_HPP
namespace k2cache {
enum NodeIdsImplIds {
  STD_MAP_NI_IMPL = 0,
};

const long NOT_FOUND_NODEID = -1L;

enum class NidsErrCode {
  NOT_FOUND_ERR_CODE = -1,
  SUCCESS_ERR_CODE = 0,
};

} // namespace k2cache
#endif // RDFCACHEK2_NODE_IDS_CONSTANTS_HPP
