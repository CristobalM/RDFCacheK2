//
// Created by Cristobal Miranda on 31-12-22.
//

#ifndef RDFCACHEK2_CUSTOM_ENDIAN_HPP
#define RDFCACHEK2_CUSTOM_ENDIAN_HPP

#include <netinet/in.h>

#include <cstdint>

#ifndef __APPLE__
#include <endian.h>
#endif

/*
namespace k2cache {
    static inline uint64_t host_to_net_u64(uint64_t value){
      auto lhs = static_cast<uint32_t> (value >> 32ULL);
      auto rhs = static_cast<uint32_t> (value & ((1ULL << 32ULL)-1ULL));
      auto lhs_tr = static_cast<uint64_t >(htonl(lhs));
      auto rhs_tr = static_cast<uint64_t >(htonl(rhs));
      return (rhs_tr << 32ULL) | lhs_tr ;
    }
    static inline uint64_t net_to_host_u64(uint64_t value){
      auto lhs = static_cast<uint32_t> (value >> 32ULL);
      auto rhs = static_cast<uint32_t> (value & ((1ULL << 32ULL)-1ULL));
      auto lhs_tr = static_cast<uint64_t >(ntohl(lhs));
      auto rhs_tr = static_cast<uint64_t >(ntohl(rhs));
      return (rhs_tr << 32ULL) | lhs_tr ;
    }
}
*/
#ifndef htobe64
//#define htobe64(value) k2cache::host_to_net_u64((value))
#define htobe64(value) htonll(value)
#endif

#ifndef be64toh
#define be64toh(value) ntohll(value)
#endif


#endif //RDFCACHEK2_CUSTOM_ENDIAN_HPP
