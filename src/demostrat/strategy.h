#ifndef SRC_DEMOSTRAT_STRATEGY_H_
#define SRC_DEMOSTRAT_STRATEGY_H_

#include <struct/market_snapshot.h>
#include <util/time_controller.h>
#include <struct/order.h>
#include <util/zmq_sender.hpp>
#include <struct/exchange_info.h>
#include <struct/order_status.h>
#include <util/common_tools.h>
#include <core/base_strategy.h>
#include <unordered_map>

#include <cmath>
#include <vector>
#include <string>


class Strategy : public BaseStrategy {
 public:
  explicit Strategy(std::unordered_map<std::string, std::vector<BaseStrategy*> >*ticker_strat_map, ZmqSender<Order>* ordersender, TimeController* tc);
  ~Strategy();

 private:
  // not must realize, but usually, it should
  void DoOperationAfterUpdatePos(Order* o, const ExchangeInfo& info) override;
  void DoOperationAfterUpdateData(const MarketSnapshot& shot) override;
  void DoOperationAfterFilled(Order* o, const ExchangeInfo& info) override;
  void DoOperationAfterCancelled(Order* o) override;

  // not must
  void ModerateOrders(const std::string & contract) override;

  void Start() override;
  bool Ready() override;
  void Run() override;

  // must realize, define the order price logic when send an order
  double OrderPrice(const std::string & contract, OrderSide::Enum side, bool control_price) override;

  std::string main_ticker;
  std::string hedge_ticker;
};

#endif  // SRC_DEMOSTRAT_STRATEGY_H_
