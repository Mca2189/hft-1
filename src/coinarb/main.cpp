#include <string.h>
#include <stdio.h>
#include <unordered_map>

#include <iostream>
#include <thread>
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

int main() {
  std::string default_path = GetDefaultPath();

  libconfig::Config param_cfg;
  std::string config_path = default_path + "/hft/config/prod/coin_prod.config";
  param_cfg.readFile(config_path.c_str());

  std::string time_config_path = default_path + "/hft/config/prod/coin_time.config";
  TimeController tc(time_config_path);

  auto ui_sender = CreateSender<ZmqSender, MarketSnapshot>("ui");
  auto order_sender = CreateSender<ZmqSender, Order>("order");

  std::unordered_map<std::string, std::vector<BaseStrategy*> > ticker_strat_map;
  std::string contract_config_path = default_path + "/hft/config/contract/bk_contract.config";
  ContractWorker cw(contract_config_path);
  const libconfig::Setting & strategies = param_cfg.lookup("strategy");
  for (int i = 0; i < strategies.getLength(); i++) {
    const libconfig::Setting & param_setting = strategies[i];
    auto s = new Strategy(param_setting, &ticker_strat_map, ui_sender.get(), order_sender.get(), &tc, &cw, Dater::GetCurrentDate());
    s->Print();
  }

  StrategyContainer<ZmqRecver> sc(ticker_strat_map, false);
  sc.Start();
}
