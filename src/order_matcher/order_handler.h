#ifndef SRC_ORDER_MATCHER_ORDER_HANDLER_H_
#define SRC_ORDER_MATCHER_ORDER_HANDLER_H_

#include <unordered_map>

#include <stdexcept>
#include <string>

#include <libconfig.h++>

#include "struct/order_side.h"
#include "struct/order.h"
#include "util/common_tools.h"

class OrderHandler {
 public:
  OrderHandler();

  bool Handle(const Order & order);

 private:
  void HandleConfig(std::string s);
  void HandleNew(const Order & order);
  void HandleCancel(const Order & order);
  void HandleMod(const Order & order);
  void GenReport();
  std::unordered_map<std::string, double> fozen_capital_map;
  std::unordered_map<std::string, double> avgcost_map;
  std::unordered_map<std::string, double> realized_pnl_map;
  std::unordered_map<std::string, int> current_pos_map;
  int closed_size;
  std::unordered_map<std::string, std::string> ticker_strat_map;
  std::unordered_map<std::string, double> strat_pnl_map;
};

#endif  // SRC_ORDER_MATCHER_ORDER_HANDLER_H_
