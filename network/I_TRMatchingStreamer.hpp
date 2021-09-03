//
// Created by cristobal on 8/31/21.
//

#ifndef RDFCACHEK2_I_TRMATCHINGSTREAMER_HPP
#define RDFCACHEK2_I_TRMATCHINGSTREAMER_HPP
#include <response_msg.pb.h>

class I_TRMatchingStreamer {
public:
  virtual proto_msg::CacheResponse get_next_response() = 0;
  virtual int get_pattern_channel_id() = 0;
  virtual int get_channel_id() = 0;
  virtual bool all_sent() = 0;

  virtual ~I_TRMatchingStreamer() = default;
};

#endif // RDFCACHEK2_I_TRMATCHINGSTREAMER_HPP
