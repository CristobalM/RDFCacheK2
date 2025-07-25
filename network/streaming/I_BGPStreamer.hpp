//
// Created by cristobal on 28-05-23.
//

#ifndef RDFCACHEK2_I_BGPSTREAMER_HPP
#define RDFCACHEK2_I_BGPSTREAMER_HPP

#include "response_msg.pb.h"
namespace k2cache {
struct I_BGPStreamer {
  virtual ~I_BGPStreamer() = default;
  virtual proto_msg::CacheResponse get_next_message() = 0;
  virtual int get_channel_id() = 0;
  virtual void cancel_query() = 0;
};
}
#endif // RDFCACHEK2_I_BGPSTREAMER_HPP
