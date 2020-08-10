#include <string.h>
#include <stdio.h>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <zmq.hpp>
#include "util/zmq_recver.hpp"
#include "util/zmq_sender.hpp"
#include "struct/market_snapshot.h"

int main() {
  ZmqRecver<MarketSnapshot> recver("data_source");
  ZmqSender<MarketSnapshot> sender("data_sender", "connect");
  while (true) {
    MarketSnapshot shot;
    recver.Recv(shot);
    sender.Send(shot);
    // shot.Show(stdout);
  }
}
