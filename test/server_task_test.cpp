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
#include "mock_structures/TestingTaskProcessor.hpp"
#include "replacement/NoCachingReplacement.hpp"
#include "server/tasks/ServerTask.hpp"
#include "updating/NoUpdate.hpp"
#include <fisher_yates.hpp>
#include <google/protobuf/message_lite.h>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>

using namespace k2cache;

static std::unique_ptr<MockClientReqHandler> mock_client_req_handler() {
  return std::make_unique<MockClientReqHandler>();
}

struct ServerTaskWrapper {
  std::unique_ptr<TDWrapper> td_wrapper;
  std::unique_ptr<ServerTask> server_task;
  std::unique_ptr<TestingTaskProcessor> testing_task_processor;
  MockClientReqHandler *mock_req_handler;
};

static std::unique_ptr<ServerTaskWrapper>
mock_server_task(const std::vector<TripleValue> &triples,
                 const std::vector<unsigned long> &nids) {
  auto out = std::make_unique<ServerTaskWrapper>();

  auto td_wrapper = mock_cache_container(triples, nids);

  auto tp =
      std::make_unique<TestingTaskProcessor>(*td_wrapper->cache_container);

  auto req_handler = mock_client_req_handler();

  out->mock_req_handler = req_handler.get();

  auto server_task = std::make_unique<ServerTask>(
      std::move(req_handler), *(td_wrapper->cache_container), *tp);

  out->td_wrapper = std::move(td_wrapper);
  out->server_task = std::move(server_task);
  out->testing_task_processor = std::move(tp);

  return out;
}
struct TriplesNidsSet {
  std::vector<TripleValue> triples;
  std::vector<unsigned long> node_ids;
};

static std::vector<TripleValue>
random_triples_from_nids(unsigned long total_triples,
                         const std::vector<unsigned long> &nids) {

  auto fy1 = fisher_yates(total_triples, nids.size());
  auto fy2 = fisher_yates(total_triples, nids.size());
  auto fy3 = fisher_yates(total_triples, nids.size());

  std::vector<TripleValue> out;
  out.reserve(total_triples);
  for (auto i = 0UL; i < nids.size(); i++) {
    out.emplace_back(fy1[i], fy2[i], fy3[i]);
  }
  return out;
}

static TriplesNidsSet random_triples_nids() {
  TriplesNidsSet out;

  auto fy = fisher_yates(10000, 1UL << 32UL);
  std::sort(fy.begin(), fy.end());
  auto triples = random_triples_from_nids(10000, fy);

  out.node_ids = std::move(fy);
  out.triples = std::move(triples);

  return out;
}


static proto_msg::CacheResponse read_response(const std::string &s){
  std::istringstream iss(s);
  auto sz = read_u64(iss);
  std::array<char, 16> hash{};
  iss.read(hash.data(), hash.size());
  std::cout << "sz is: " << sz << std::endl;
  proto_msg::CacheResponse resp;
  resp.ParseFromIstream(&iss);
  return resp;
}

TEST(server_task_test, process_connection_end_test) {
  auto tn = random_triples_nids();
  auto mst = mock_server_task(tn.triples, tn.node_ids);

  proto_msg::CacheRequest req;
  req.set_request_type(proto_msg::MessageType::CONNECTION_END);
  req.mutable_connection_end_request();
  auto sreq = req.SerializeAsString();

  auto msg = std::make_unique<Message>(sreq.data(), sreq.size());

  mst->mock_req_handler->set_next_req_msg(std::move(msg));
  mst->server_task->process_next();
  auto resp_msg = mst->mock_req_handler->get_last_resp_msg();
  auto resp = read_response(resp_msg);

  ASSERT_EQ(proto_msg::MessageType::CONNECTION_END, resp.response_type());
}
TEST(server_task_test, process_req_triple_stream) {
  auto tn = random_triples_nids();
  auto mst = mock_server_task(tn.triples, tn.node_ids);

  auto pred_first = tn.triples[0].second;
  auto pred_real = tn.node_ids[pred_first];

  std::set<std::pair<unsigned long, unsigned long>> matches;
  for(auto t : tn.triples){
    if(t.second == pred_first){
      matches.insert({t.first, t.third});
    }
  }

  {

    proto_msg::CacheRequest req;
    req.set_request_type(proto_msg::MessageType::CACHE_REQUEST_SEPARATE_PREDICATES);
    auto *sep = req.mutable_cache_request_separate_predicates();
    sep->add_predicates()->set_encoded_data(pred_real);
    auto sreq = req.SerializeAsString();
    auto msg = std::make_unique<Message>(sreq.data(), sreq.size());
    mst->mock_req_handler->set_next_req_msg(std::move(msg));
    mst->server_task->process_next();
    auto resp_s = mst->mock_req_handler->get_last_resp_msg();
    auto resp = read_response(resp_s);
    ASSERT_EQ(proto_msg::MessageType::EXTRACT_SEPARATE_PREDICATES_RESPONSE, resp.response_type());
  }

  proto_msg::CacheRequest req;
  req.set_request_type(proto_msg::MessageType::STREAM_REQUEST_TRIPLE_PATTERN);
  auto *req_trip = req.mutable_stream_request_triple_pattern();
  auto *tnode = req_trip->mutable_triple_node();
  tnode->mutable_subject()->set_encoded_data(-9);
  tnode->mutable_predicate()->set_encoded_data(pred_real);
  tnode->mutable_object()->set_encoded_data(-9);



  auto sreq = req.SerializeAsString();

  auto msg = std::make_unique<Message>(sreq.data(), sreq.size());

  mst->mock_req_handler->set_next_req_msg(std::move(msg));
  mst->server_task->process_next();
  auto resp_msg = mst->mock_req_handler->get_last_resp_msg();
  auto resp = read_response(resp_msg);

  ASSERT_EQ(proto_msg::MessageType::CONNECTION_END, resp.response_type());
}