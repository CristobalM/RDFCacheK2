//
// Created by cristobal on 26-06-22.
//

#include "CacheContainerFactory.hpp"
#include "CacheContainerImpl.hpp"
namespace k2cache {
std::unique_ptr<CacheContainer>
CacheContainerFactory::create(const CacheArgs &args) {
  return std::make_unique<CacheContainerImpl>(args);
}
} // namespace k2cache
