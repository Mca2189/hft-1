#ifndef BASE_STRATEGY_H_
#define BASE_STRATEGY_H_

#include "util/base_sender.hpp"
#include "util/base_sender.hpp"
#include "util/dater.h"
#include "util/common_tools.h"
#include "util/time_controller.h"
#include "util/history_worker.h"
#include "util/contract_worker.h"
#include "define.h"
#include "struct/command.h"
#include "struct/exchange_info.h"
#include "struct/order_status.h"
#include "struct/strategy_mode.h"
#include "struct/market_snapshot.h"
#include "struct/order.h"
#include "struct/strategy_status.h"
#include <libconfig.h++>
#include <unordered_map>

#include <cmath>
#include <vector>
#include <string>
#include <unistd.h>
#include <memory>
#include <mutex>

class BaseStrategy {
 public:

  // constructor and deconstructor
  BaseStrategy() = default;
  // forbid copy constructor and operator =
  BaseStrategy(const BaseStrategy&) = delete;
  BaseStrategy& operator=(const BaseStrategy&) = delete;
  virtual ~BaseStrategy() {}

  // interface function
  void UpdateData(const MarketSnapshot& shot);
  void UpdateData(const MarketSnapshot& this_shot, const MarketSnapshot& next_shot);
  void UpdateExchangeInfo(const ExchangeInfo& info);

  // utility function
  void SendPlainText(const std::string & flag, const std::string & s);
  void Print() const {}
  virtual void HandleCommand(const Command& shot) {};

 protected:
  // order operation
  std::string GenUniqueId();
  Order* NewOrder(const std::string & ticker, OrderSide::Enum side, int size, bool control_price, bool sleep_order, const std::string & tbd, bool no_today = false);
  Order* PlaceOrder(const std::string & ticker, double price, int size, bool no_today = false, const std::string & orderinfo = "");
  void ModOrder(Order* o, bool sleep=false);
  void ReplaceOrder(Order* o);
  void CancelAll(const std::string & ticker);
  void CancelAll();
  void CancelOrder(Order* o);
  void DelOrder(const std::string & ref);
  void DelSleepOrder(const std::string & ref);
  void ClearAll();
  void Wakeup();
  void Wakeup(Order* o);

  void RequestQryPos();
  // automatic control function
  void CheckStatus(const MarketSnapshot& shot);
  void UpdatePos(Order* o, const ExchangeInfo& info);
  void UpdateAvgCost(const std::string & ticker, double trade_price, int size);
  bool TimeUp() const;

  // set mode
  void SetStrategyMode(StrategyMode::Enum mode, std::ofstream* exchange_file);

  // automatic generate simulate trade info
  virtual void SimulateTrade(Order* o);

  // utility function
  double GetMid(const std::string & ticker);

  // variable with default value
  bool m_position_ready = false;
  int m_ref_num = 0;
  long int m_build_position_time = MAX_UNIX_TIME;
  int m_max_holding_sec = 36000;
  bool m_init_ticker = false;
  StrategyMode::Enum m_mode = StrategyMode::Real;
  std::ofstream* m_sim_exchange_file_ = nullptr;
  StrategyStatus::Enum m_ss = StrategyStatus::Init;

  // communication worker
  BaseSender<Order>* m_order_sender;
  BaseSender<MarketSnapshot>* m_ui_sender;
  unordered_map<std::string, MarketSnapshot> m_shot_map;
  unordered_map<std::string, MarketSnapshot> m_next_shot_map;
  unordered_map<std::string, Order*> m_order_map;
  unordered_map<std::string, Order*> m_sleep_order_map;
  unordered_map<std::string, int> m_position_map;
  unordered_map<std::string, double> m_avgcost_map;
  std::unordered_map<std::string, int> m_cancel_map;

  // mutex
  std::mutex m_cancel_mutex;
  std::mutex m_order_ref_mutex;
  std::mutex m_mod_mutex;

  // for gen unique id
  std::string m_strat_name;
  TimeController* m_tc;
  ContractWorker * m_cw;
  HistoryWorker* m_hw;

  // not use now, save for future
  int m_ticker_size;
  MarketSnapshot m_last_shot;

  // private virtual function = public virtual function
 private:
  virtual bool PriceChange(double current_price, double reasonable_price, OrderSide::Enum side, double edurance);

  // status control function
  virtual void Start() {}
  virtual void Stop() {}
  virtual void Resume() {}
  virtual void Pause() {}
  virtual void Train() {}
  virtual void Flatting() {}
  virtual void ForceFlat() {}

  // order control
  virtual void ModerateOrders(const std::string & ticker) {}

  // interface
  virtual void DoOperationAfterUpdatePos(Order* o, const ExchangeInfo& info) {}
  virtual void DoOperationAfterUpdateData(const MarketSnapshot& shot) {}
  virtual void DoOperationAfterFilled(Order* o, const ExchangeInfo& info) {}
  virtual void DoOperationAfterCancelled(Order* o) {}

  // pure virtual function, must be implemented
  virtual double OrderPrice(const std::string & ticker, OrderSide::Enum side, bool control_price) = 0;
  virtual void Run() = 0;
  virtual bool Ready() = 0;
};

#endif  // BASE_STRATEGY_H_
