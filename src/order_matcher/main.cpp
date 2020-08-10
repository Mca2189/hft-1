#include <stdio.h>
#include <unordered_map>

#include <iostream>
#include <string>

#include <zmq.hpp>

#include "util/zmq_recver.hpp"

#include "./order_handler.h"

void* RunOrderCommandListener(void *param) {
  OrderHandler* order_handler = reinterpret_cast<OrderHandler*>(param);
  ZmqRecver<Order>* r = new ZmqRecver<Order>("order_recver");
  while (true) {
    Order o;
    r->Recv(o);
    if (!order_handler->Handle(o)) {
      // handle error
    }
  }
  return NULL;
}

int main() {
  OrderHandler order_handler;

  pthread_t order_thread;
  if (pthread_create(&order_thread,
                     NULL,
                     &RunOrderCommandListener,
                     &order_handler) != 0) {
    perror("pthread_create");
    exit(1);
  }
  while (true) {
  }
}
