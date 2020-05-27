#include <libconfig.h++>
#include <unordered_map>
#include <map>
#include <utility>
#include <string>
#include <vector>

#include "core/backtester.h"
#include "util/ThreadPool.h"
#include "util/time_controller.h"
#include "util/zmq_sender.hpp"
#include "util/zmq_recver.hpp"
#include "util/dater.h"
#include "util/history_worker.h"
#include "util/contract_worker.h"
#include "util/common_tools.h"
#include "struct/market_snapshot.h"
#include "./strategy.h"

// std::unique_ptr<Sender<MarketSnapshot> > ui_sender(new ZmqSender<MarketSnapshot>("*:33333", "bind", "tcp", "mid.dat"));
// std::unique_ptr<Sender<Order> > order_sender(new ZmqSender<Order>("order_sender", "connect", "ipc", "order.dat"));
// std::unique_ptr<Sender<Order> > order_sender(new ZmqSender<Order>("order_sender", 100000, "order.dat"));

struct BTConfig {
  std::string fixed_path;
  std::string backtest_outputdir;
  std::string start_date;
  int period;
  std::string test_mode;
  // std::vector<const libconfig::Setting> strats;
  ContractWorker* strat_cw;
  ContractWorker* cw;
  TimeController* tc;
  inline std::tuple<ZmqSender<MarketSnapshot> *, ZmqSender<Order> *, std::ofstream*> GenSender(const std::string& date) {
    std::string ui_address = "backtest_ui_" + date;
    std::string order_address = "order_sender_" + date;
    std::string ui_file = backtest_outputdir + "/mid_" + date + ".dat";
    std::string order_file = backtest_outputdir + "/order_" + date + ".dat";
    return std::make_tuple(new ZmqSender<MarketSnapshot>(ui_address, "bind", "ipc", ui_file), new ZmqSender<Order>(order_address, "connect",  "ipc", order_file), new std::ofstream(backtest_outputdir + "/exchange_" + date + ".dat", ios::out | ios::binary));
  }
  inline HistoryWorker* GenHw(const std::string & date) {
    return new HistoryWorker(Dater::FindOneValid(date, -20, fixed_path));
  }
} bt_config;

void LoadConfig() {
  std::string default_path = GetDefaultPath();
  libconfig::Config param_cfg;
  std::string config_path = default_path + "/hft/config/backtest/backtest.config";
  std::string contract_config_path = default_path + "/hft/config/contract/bk_contract.config";
  param_cfg.readFile(config_path.c_str());
  try {
    std::string fixed_path = param_cfg.lookup("fixed_path");
    std::string start_date = param_cfg.lookup("start_date");
    std::string backtest_outputdir = param_cfg.lookup("backtest_outputdir");
    std::string test_mode = param_cfg.lookup("test_mode");
    EnsureDir(backtest_outputdir);
    Dater dt;
    if (start_date == "today") {
      start_date = dt.GetDate();
    }
    if (start_date == "yesterday") {
      start_date = dt.GetDate("", -1);
    }
    int period = param_cfg.lookup("period");
    bt_config.backtest_outputdir = backtest_outputdir;
    bt_config.fixed_path = fixed_path;
    bt_config.start_date = start_date;
    bt_config.period = period;
    bt_config.test_mode = test_mode;
  } catch(const libconfig::SettingNotFoundException &nfex) {
    printf("Setting '%s' is missing", nfex.getPath());
    exit(1);
  } catch(const libconfig::SettingTypeException &tex) {
    printf("Setting '%s' has the wrong type", tex.getPath());
    exit(1);
  } catch (const std::exception& ex) {
    printf("EXCEPTION: %s\n", ex.what());
    exit(1);
  }

  std::string time_config_path = default_path + "/hft/config/prod/time.config";
  bt_config.tc = new TimeController(time_config_path);
  bt_config.cw = new ContractWorker(contract_config_path);
  bt_config.strat_cw = new ContractWorker(config_path, "strategy");
}

std::map<std::string, std::string> GetBacktestFile() {
  auto dt = Dater();
  return dt.GetValidMap(bt_config.start_date, bt_config.period, bt_config.fixed_path);
}

std::unordered_map<std::string, std::vector<BaseStrategy*> > GetStratMap(std::string date) {
  std::unordered_map<std::string, std::vector<BaseStrategy*> > ticker_strat_map;
  ZmqSender<MarketSnapshot> * data_sender;
  ZmqSender<Order> * order_sender;
  std::ofstream* f;
  StrategyMode::Enum mode;
  if (bt_config.test_mode == "test") {
    mode = StrategyMode::PlainTest;
  } else if (bt_config.test_mode == "nexttest") {
    mode = StrategyMode::NextTest;
  } else {
    throw std::invalid_argument(bt_config.test_mode.c_str());
  }
  std::tie(data_sender, order_sender, f) = bt_config.GenSender(date);
  for (auto ticker : bt_config.strat_cw->GetTicker()) {
    const libconfig::Setting & p = bt_config.strat_cw->Lookup(ticker);
    auto s = new Strategy(p, &ticker_strat_map, data_sender, order_sender, bt_config.tc, bt_config.cw, bt_config.GenHw(date), date, mode, f);
    s->Print();
  }
  return ticker_strat_map;
}

void RunBacktest(const std::string& date, const std::string& f) {
  TimeController tc;
  tc.StartTimer();
  auto tsm = GetStratMap(date);
  Backtester bt(tsm);
  bt.LoadData(f);
  tc.EndTimer("Run@" + date);
}

int main() {
  LoadConfig();
  auto file_v = GetBacktestFile();
  PrintMap(file_v);
  ThreadPool pool(6);
  for (auto i: file_v) {
    pool.enqueue(RunBacktest, i.first, i.second);
  }
}
