#include <stdio.h>
#include <zmq.hpp>
#include <ThostFtdcTraderApi.h>
#include <unordered_map>

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// #include "util/dater.h"
// #include "util/contract_worker.h"
// #include "util/zmq_recver.hpp"
// #include "util/zmq_sender.hpp"
#include "./message_sender.h"
#include "./listener.h"
#include "./token_manager.h"

FILE* order_file;
bool enable_stdout = true;
bool enable_file = true;

std::unordered_map<std::string, std::string> RegisterExchange() {
  std::unordered_map<std::string, std::vector<std::string> > exchange_ticker;
  exchange_ticker["SHFE"] = {"cu", "ni", "au"};
  exchange_ticker["CFFEX"] = {"IH", "IC", "IF", "T"};
  exchange_ticker["CZCE"] = {};
  exchange_ticker["DCE"] = {};
  std::unordered_map<std::string, std::string> ticker_exchange;
  for (auto i : exchange_ticker) {
    std::vector<std::string> c_v = i.second;
    for (auto j : c_v) {
      ticker_exchange[j] = i.first;
    }
  }
  return ticker_exchange;
}

/*
void* RunOrderCommandListener(void *param) {
  MessageSender* message_sender = reinterpret_cast<MessageSender*>(param);
  auto r = new ZmqRecver<Order>("order_recver");
  std::shared_ptr<ZmqSender<Order> > sender(new ZmqSender<Order>("*:33335", "bind", "tcp"));
  while (true) {
    Order o;
    r->Recv(o);
    sender.get()->Send(o);
    if (enable_stdout) {
      o.Show(stdout);
    }
    if (enable_file) {
      o.Show(order_file);
    }
    // check order's correct
    if (!message_sender->Handle(o)) {
      printf("Handle Order %s failed!\n", o.order_ref);
      // handle error
    }
  }
  return NULL;
}
*/

int main() {
  enable_file = true;
  enable_stdout = true;
  if (enable_file) {
    order_file = fopen("ctporder_order.txt", "w");
  }
  CThostFtdcTraderApi* user_api = CThostFtdcTraderApi::CreateFtdcTraderApi();

  std::string broker = "1025";
  std::string username = "160197";
  std::string password = "qw78963";
  /*
  std::string broker = "9999";
  std::string username = "116909";
  std::string password = "yifeng";
  std::string broker = "9999";
  std::string username = "115686";
  std::string password = "fz567789";
  */
  ::unordered_map<int, int> order_id_map;

  TokenManager tm;
  std::unordered_map<std::string, std::string> exchange_map = RegisterExchange();
  MessageSender message_sender(user_api,
                               broker,
                               username,
                               password,
                               false,
                               &order_id_map,
                               &tm,
                               exchange_map);

  // std::string default_path = GetDefaultPath();
  std::string contract_config_path = "/root/hft/config/contract/bk_contract.config";
  // ContractWorker cw(contract_config_path);
  Listener listener("exchange_info",
                    &message_sender,
                    "error_list",
                    &order_id_map,
                    &tm,
                    // &cw,
                    enable_stdout,
                    enable_file);

  /*
  pthread_t order_thread;
  if (pthread_create(&order_thread,
                     NULL,
                     &RunOrderCommandListener,
                     &message_sender) != 0) {
    perror("pthread_create");
    exit(1);
  }
  */

  user_api->RegisterSpi(&listener);
  printf("register spi sent\n");

  user_api->SubscribePrivateTopic(THOST_TERT_QUICK);
  user_api->SubscribePublicTopic(THOST_TERT_QUICK);
  std::string counterparty_host = "tcp://123.138.59.245:51216";  // simnow
  user_api->RegisterFront(const_cast<char*>(counterparty_host.c_str()));
  user_api->Init();
  if (enable_file) {
    fclose(order_file);
  }
  while (true) {
  }
  user_api->Release();
}
