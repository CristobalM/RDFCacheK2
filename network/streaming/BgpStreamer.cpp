//
// Created by cristobal on 28-05-23.
//

#include "BgpStreamer.hpp"

namespace k2cache {
proto_msg::CacheResponse BgpStreamer::get_next_message() {


  return proto_msg::CacheResponse();
}
BgpStreamer::BgpStreamer(BgpMessage message, CacheContainer &cache)
    : message(std::move(message)), cache(cache) {
}

} // namespace k2cache