#include <string.h>
#include <stdio.h>
#include <unordered_map>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>

#include <zmq.hpp>
#include "struct/order.h"
#include "util/zmq_recver.hpp"
#include "util/zmq_sender.hpp"
#include "struct/market_snapshot.h"
#include "util/common_tools.h"
#include "core/base_strategy.h"

#include "./strategy.h"

void HandleLeft() {
}

void PrintResult() {
}


void* RunExchangeListener(void *param) {
  std::unordered_map<std::string, std::vector<BaseStrategy*> > * sv_map = reinterpret_cast<std::unordered_map<std::string, std::vector<BaseStrategy*> >* >(param);
  ZmqRecver<ExchangeInfo> recver("exchange_info");
  while (true) {
    ExchangeInfo info;
    recver.Recv(info);
    std::vector<BaseStrategy*> sv = (*sv_map)[info.ticker];
    for (auto v : sv) {
      v->UpdateExchangeInfo(info);
    }
    /*
    for (unsigned int i = 0; i < s_v->size(); i++) {
      (*s_v)[i]->UpdateExchangeInfo(info);
    }
    */
  }
  return NULL;
}

int main() {
  std::unordered_map<std::string, std::vector<BaseStrategy*> > ticker_strat_map;
  TimeController tc("/root/hft/config/prod/time.config");

  ZmqRecver<MarketSnapshot> data_recver("data_recver");
  std::vector<BaseStrategy*> sv;
  sv.emplace_back(new Strategy("ni1905", "ni1903", 5, 10, tc, 1, "ni", &ticker_strat_map));
  sv.emplace_back(new Strategy("cu1903", "cu1902", 5, 10, tc, 5, "cu", &ticker_strat_map));
  sv.emplace_back(new Strategy("fu1909", "fu1905", 5, 1, tc, 10, "fu", &ticker_strat_map));
  sv.emplace_back(new Strategy("MA905", "MA909", 5, 10, tc, 1, "MA", &ticker_strat_map));
  sv.emplace_back(new Strategy("zn1902", "zn1903", 5, 5, tc, 5, "zn", &ticker_strat_map));
  sv.emplace_back(new Strategy("rb1905", "rb1909", 5, 1, tc, 10, "rb", &ticker_strat_map));
  sv.emplace_back(new Strategy("m1905", "m1903", 5, 1, tc, 10, "m", &ticker_strat_map));
  sv.emplace_back(new Strategy("i1905", "i1909", 5, 0.5, tc, 100, "i", &ticker_strat_map));

  pthread_t exchange_thread;
  if (pthread_create(&exchange_thread,
                     NULL,
                     &RunExchangeListener,
                     &ticker_strat_map) != 0) {
    perror("pthread_create");
    exit(1);
  }
  sleep(3);
  while (true) {
    MarketSnapshot shot;
    data_recver.Recv(shot);
    shot.is_initialized = true;
    std::vector<BaseStrategy*> sv = ticker_strat_map[shot.ticker];
    for (auto v : sv) {
      v->UpdateData(shot);
    }
  }
  HandleLeft();
  PrintResult();
}
