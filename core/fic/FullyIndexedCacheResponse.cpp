//
// Created by cristobal on 3/5/22.
//

#include "FullyIndexedCacheResponse.hpp"
namespace k2cache {
bool FullyIndexedCacheResponse::exists() { return source != nullptr; }
I_CachedPredicateSource *FullyIndexedCacheResponse::get() { return source; }
FullyIndexedCacheResponse::FullyIndexedCacheResponse(
    I_CachedPredicateSource *source)
    : source(source) {}
} // namespace k2cache
