//
// Created by cristobal on 28-05-23.
//

#ifndef RDFCACHEK2_BGPSTREAMER_HPP
#define RDFCACHEK2_BGPSTREAMER_HPP

#include "CacheContainer.hpp"
#include "I_BgpStreamer.hpp"
#include "messages/BgpMessage.hpp"
namespace k2cache {
class BgpStreamer : public I_BgpStreamer {
  BgpMessage message;
  CacheContainer &cache;
public:
  BgpStreamer(BgpMessage message, CacheContainer &cache);
  proto_msg::CacheResponse get_next_message() override;
};
}

#endif // RDFCACHEK2_BGPSTREAMER_HPP
