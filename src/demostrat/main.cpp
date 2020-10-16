#include <string.h>
#include <stdio.h>
#include <unordered_map>

#include <thread>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>

#include <zmq.hpp>
#include "struct/order.h"
#include "struct/market_snapshot.h"
#include "core/strategy_container.hpp"
#include "util/common_tools.h"
#include "core/base_strategy.h"
#include "util/history_worker.h"
#include "util/dater.h"
#include "util/zmq_recver.hpp"
#include "util/zmq_sender.hpp"
#include "./strategy.h"

void HandleLeft() {
}

void PrintResult() {
}

int main() {
  std::unique_ptr<ZmqSender<Order> > order_sender(new ZmqSender<Order>("order_sender", "connect", "ipc", "order.dat"));
  std::string default_path = GetDefaultPath();
  std::unordered_map<std::string, std::vector<BaseStrategy*> > ticker_strat_map;
  std::string time_config_path = default_path + "/hft/config/prod/time.config";
  TimeController tc(time_config_path);
  auto s = new Strategy(&ticker_strat_map, order_sender.get(), &tc);
  s->Print();
  StrategyContainer<ZmqRecver> sc(ticker_strat_map);
  sc.Start();
  HandleLeft();
  PrintResult();
}
