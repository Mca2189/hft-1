#ifndef MARKET_SNAPSHOT_H_
#define MARKET_SNAPSHOT_H_

#include <stdio.h>
#include <sys/time.h>
#include "define.h"

#include <algorithm>
#include <iostream>
#include <fstream>

struct MarketSnapshot {
  char ticker[MAX_TICKER_LENGTH];
  double bids[MARKET_DATA_DEPTH];
  double asks[MARKET_DATA_DEPTH];
  int bid_sizes[MARKET_DATA_DEPTH];
  int ask_sizes[MARKET_DATA_DEPTH];
  double last_trade;
  int last_trade_size;
  int volume;
  double turnover;
  double open_interest;
  bool is_trade_update;  // true if updated caused by trade
  timeval time;

  bool is_initialized;

  MarketSnapshot()
      : last_trade(0.0),
        last_trade_size(0),
        volume(0),
        turnover(0.0),
        open_interest(0.0),
        is_trade_update(false),
        time(),
        is_initialized(false) {
    ticker[0] = 0;
    for (int i = 0; i < MARKET_DATA_DEPTH; ++i) {
      bids[i] = 0.0;
      asks[i] = 0.0;
      bid_sizes[i] = 0;
      ask_sizes[i] = 0;
    }
  }

  void ShowCsv(FILE* stream, int depth = MARKET_DATA_DEPTH) const {
    char time_s[32];
    snprintf(time_s, sizeof(time_s), "%ld.%ld", time.tv_sec, time.tv_usec);
    double time_sec = atof(time_s);
    // double time_sec = TransTime(time);
    fprintf(stream, "%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%lf,%d,%d,%lf,%lf,%d,%lf,%d\n", ticker,bids[0],bids[1],bids[2],bids[3],bids[4],
                   asks[0],asks[1],asks[2],asks[3],asks[4],
                   bid_sizes[0],bid_sizes[1],bid_sizes[2],bid_sizes[3],bid_sizes[4],
                   ask_sizes[0],ask_sizes[1],ask_sizes[2],ask_sizes[3],ask_sizes[4],
                   last_trade,last_trade_size,volume,turnover,open_interest,
                   is_trade_update,time_sec,is_initialized);
  }

  void Show(FILE* stream, int depth = MARKET_DATA_DEPTH) const {
    fprintf(stream, "%ld %04ld SNAPSHOT %s |",
            time.tv_sec, time.tv_usec, ticker);

    int n = std::min(depth, MARKET_DATA_DEPTH);
    for (int i = 0; i < n; ++i) {
      fprintf(stream, " %.12g %.12g | %3d x %3d |",
              bids[i], asks[i], bid_sizes[i], ask_sizes[i]);
    }

    if (is_trade_update) {
      // T for trade update
      fprintf(stream, " %.12g %d %d T\n", last_trade, last_trade_size, volume);
    } else {
      // M for market update
      fprintf(stream, " %.12g %d %d M %.12g %.12g\n", last_trade, last_trade_size, volume, turnover, open_interest);
    }
  }

  std::string ToCsv(int depth = MARKET_DATA_DEPTH, bool with_ln = true) const {
    char temp[1024];
    char time_s[32];
    snprintf(time_s, sizeof(time_s), "%ld.%ld", time.tv_sec, time.tv_usec);
    double time_sec = atof(time_s);
    if (with_ln) {
      snprintf(temp, sizeof(temp), "%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%lf,%d,%d,%lf,%lf,%d,%lf,%d\n", ticker,bids[0],bids[1],bids[2],bids[3],bids[4],
                     asks[0],asks[1],asks[2],asks[3],asks[4],
                     bid_sizes[0],bid_sizes[1],bid_sizes[2],bid_sizes[3],bid_sizes[4],
                     ask_sizes[0],ask_sizes[1],ask_sizes[2],ask_sizes[3],ask_sizes[4],
                     last_trade,last_trade_size,volume,turnover,open_interest,
                     is_trade_update,time_sec,is_initialized);
      return temp;
    } else {
      snprintf(temp, sizeof(temp), "%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%lf,%d,%d,%lf,%lf,%d,%lf,%d", ticker,bids[0],bids[1],bids[2],bids[3],bids[4],
                     asks[0],asks[1],asks[2],asks[3],asks[4],
                     bid_sizes[0],bid_sizes[1],bid_sizes[2],bid_sizes[3],bid_sizes[4],
                     ask_sizes[0],ask_sizes[1],ask_sizes[2],ask_sizes[3],ask_sizes[4],
                     last_trade,last_trade_size,volume,turnover,open_interest,
                     is_trade_update,time_sec,is_initialized);
      return temp;
    }
  }

  std::string ToString(int depth = MARKET_DATA_DEPTH) const {
    std::string s;
    char temp[1024];
    snprintf(temp, sizeof(temp),  "%ld %04ld SNAPSHOT %s |",
            time.tv_sec, time.tv_usec, ticker);
    s += temp;

    int n = std::min(depth, MARKET_DATA_DEPTH);
    for (int i = 0; i < n; ++i) {
      snprintf(temp, sizeof(temp), " %.12g %.12g | %3d x %3d |",
              bids[i], asks[i], bid_sizes[i], ask_sizes[i]);
      s += temp;
    }

    if (is_trade_update) {
      // T for trade update
      snprintf(temp, sizeof(temp), "%.12g %d %d T\n", last_trade, last_trade_size, volume);
    } else {
      // M for market update
      snprintf(temp, sizeof(temp), "%.12g %d %d M %.12g %.12g\n", last_trade, last_trade_size, volume, turnover, open_interest);
    }
    s += temp;
    return s;
  }

  bool IsGood() const {
    if (bids[0] < 0.001 || asks[0] < 0.001 || ask_sizes[0] <=0 || bid_sizes[0] <= 0) {
      return false;
    }
    return true;
  }
};

#endif // MARKET_SNAPSHOT_H_
