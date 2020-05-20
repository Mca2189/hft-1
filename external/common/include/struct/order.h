#ifndef ORDER_H_
#define ORDER_H_

#include "define.h"
#include "order_side.h"
#include "order_action.h"
#include "order_status.h"
#include "offset.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdint>
#include <cmath>
#include <sys/time.h>
#include <fstream>

struct Order {
  timeval shot_time;
  timeval send_time;
  char ticker[MAX_CONTRACT_LENGTH];
  double price;
  int32_t size;
  int32_t traded_size;
  OrderSide::Enum side;
  char order_ref[MAX_ORDERREF_SIZE];
  OrderAction::Enum action;
  OrderStatus::Enum status;
  Offset::Enum offset;
  double target_price;
  double shot_latency;
  char tbd[96];
  char exchange[32];

  Order()
    : 
      price(0.0),
      size(0),
      traded_size(0),
      offset(Offset::UNINITED),
      target_price(0.0),
      shot_latency(0.0) {
    snprintf(ticker, sizeof(ticker), "%s", "");
    snprintf(order_ref, sizeof(order_ref), "%s", "");
    snprintf(tbd, sizeof(tbd), "%s", "");
    snprintf(exchange, sizeof(exchange), "%s", "");
  }

  bool Valid() const {
    if (status == OrderStatus::SubmitNew || status == OrderStatus::New || status == OrderStatus::CancelRej) {
      return true;
    }
    return false;
  }

  inline bool Check() const {
    return shot_time.tv_sec >  1000000000 && shot_time.tv_sec < 2000000000 && strlen(ticker) > 0 && price > 0.0 && abs(size) > 0 && traded_size <= size && side != OrderSide::Unknown && strlen(order_ref) > 0 ;
  }

  void ShowCsv(FILE* stream) const {
    char shot_time_s[32];
    snprintf(shot_time_s, sizeof(shot_time_s), "%ld.%ld", shot_time.tv_sec, shot_time.tv_usec);
    double shot_time_sec = atof(shot_time_s);
    char send_time_s[32];
    snprintf(send_time_s, sizeof(send_time_s), "%ld.%ld", send_time.tv_sec, send_time.tv_usec);
    double send_time_sec = atof(send_time_s);
    fprintf(stream, "%lf,%lf,%s,%lf,%d,%d,%s,%s,%s,%s,%s,%.2f,%lf,%s,%s\n",shot_time_sec,send_time_sec,ticker,price,size,traded_size,OrderSide::ToString(side),order_ref,OrderAction::ToString(action),OrderStatus::ToString(status),Offset::ToString(offset), shot_latency, target_price, exchange, tbd);
  }

  void Show(std::ostream& stream) const {
    stream << send_time.tv_sec << " " << send_time.tv_usec << " ";
    stream << shot_time.tv_sec << " " << shot_time.tv_usec << " ";
    stream << "Order " << ticker << " " << price << "@" << size << " ";
    stream << traded_size << " " << OrderSide::ToString(side) << " " << order_ref << " ";
    stream << OrderAction::ToString(action) << " " << OrderStatus::ToString(status) << " ";
    stream << shot_latency << " " << target_price << " ";
    stream << Offset::ToString(offset) << " " << exchange << " " << tbd << std::endl;
  }

  void Show(FILE* stream) const {
    // timeval show_time;
    // gettimeofday(&show_time, NULL);
    fprintf(stream, "%ld %04ld %ld %04ld Order %s |",
            send_time.tv_sec, send_time.tv_usec, shot_time.tv_sec, shot_time.tv_usec, ticker);

      fprintf(stream, " %lf@%d %d %s %s %s %s %s %.2f %lf %s %s\n", price, size, traded_size, OrderSide::ToString(side), order_ref, OrderAction::ToString(action), OrderStatus::ToString(status), Offset::ToString(offset), shot_latency, target_price, exchange, tbd);
  }
};

#endif  //  ORDER_H_
