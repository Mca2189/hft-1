#ifndef EXCHANGE_INFO_H_
#define EXCHANGE_INFO_H_

#include <sys/time.h>
#include <fstream>
#include <stdio.h>
#include "define.h"
#include "info_type.h"
#include "order_side.h"
// #include "wrapstruct.h"

struct ExchangeInfo {
  timeval show_time;
  timeval shot_time;
  InfoType::Enum type;
  char ticker[MAX_CONTRACT_LENGTH];
  char order_ref[MAX_ORDERREF_SIZE];
  int trade_size;
  double trade_price;
  OrderSide::Enum side;
  char reason[EXCHANGE_INFO_SIZE];

  ExchangeInfo()
    : trade_size(0),
      trade_price(-1) {
    snprintf(reason, sizeof(reason), "%s", "");
  }

  void ShowCsv(FILE* stream) const {
    char time_s[32];
    snprintf(time_s, sizeof(time_s), "%ld.%06ld %ld.%06ld", show_time.tv_sec, show_time.tv_usec, shot_time.tv_sec, shot_time.tv_usec);
    double time_sec = atof(time_s);
    fprintf(stream, "%lf,%s,%s,%s,%d,%lf,%s,%s\n", time_sec, InfoType::ToString(type),ticker,order_ref,trade_size,trade_price,reason,OrderSide::ToString(side));
  }

  void Show(std::ostream& stream) const {
    stream << show_time.tv_sec << " " << show_time.tv_usec << " ";
    stream << shot_time.tv_sec << " " << shot_time.tv_usec << " ";
    stream << "Exchangeinfo |" << order_ref  << " " << trade_price << "@" << trade_size << " ";
    stream << InfoType::ToString(type) << " " << ticker << " " << OrderSide::ToString(side) << std::endl;
  }

  void Show(FILE* stream) const {
    fprintf(stream, "%ld %06ld %ld %06ld exchangeinfo %s |",
            show_time.tv_sec, show_time.tv_usec, shot_time.tv_sec, shot_time.tv_usec, order_ref);
    fprintf(stream, " %lf@%d %s %s %s\n", trade_price, trade_size, InfoType::ToString(type), ticker, OrderSide::ToString(side));
  }
};

#endif  //  EXCHANGE_INFO_H_
