//
// Created by cristobal on 7/23/21.
//

#ifndef RDFCACHEK2_I_QRSTREAMER_HPP
#define RDFCACHEK2_I_QRSTREAMER_HPP

#include <response_msg.pb.h>
class I_QRStreamer {
public:
  virtual proto_msg::CacheResponse get_next_response() = 0;
  virtual ~I_QRStreamer() = default;
  virtual int get_id() = 0;
  virtual bool all_sent() = 0;
  virtual const std::vector<unsigned long> &get_predicates_in_use() = 0;
};

#endif // RDFCACHEK2_I_QRSTREAMER_HPP
