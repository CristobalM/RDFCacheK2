//
// Created by cristobal on 12-10-22.
//
#include <gtest/gtest.h>

#include <sstream>

#include "CacheContainerImpl.hpp"
#include "cache_test_util.hpp"
#include "fic/NoFIC.hpp"
#include "manager/PredicatesCacheManagerImpl.hpp"
#include "mock_structures/FHMock.hpp"
#include "mock_structures/MockClientReqHandler.hpp"
#include "replacement/NoCachingReplacement.hpp"
#include "server/tasks/ServerTask.hpp"
#include "updating/NoUpdate.hpp"
#include <google/protobuf/message_lite.h>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>
using namespace k2cache;

static std::unique_ptr<ClientReqHandler> mock_client_req_handler() {
  auto mcr = std::make_unique<MockClientReqHandler>([](const std::string &s) {

  });
  return mcr;
}


static std::unique_ptr<ServerTask> mock_server_task() {
  std::vector<TripleValue> triples;
  std::vector<unsigned long> nids;
  mock_cache_container()
  auto out = std::make_unique<ServerTask>(mock_client_req_handler(),

  );

  return out;
}

TEST(server_task_test, process_connection_end_test) {}