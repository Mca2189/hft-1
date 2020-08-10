#include <stdio.h>

#include <thread>
#include <iostream>
#include <string>

#include "struct/order.h"
#include "struct/exchange_info.h"
#include "struct/order_side.h"
#include "struct/offset.h"
#include "struct/order_action.h"

#include "util/zmq_sender.hpp"
#include "util/zmq_recver.hpp"

void RunSend(ZmqSender<Order> * sender) {
  int count = 0;
  std::string ticker;
  std::string buffer;
  std::string action;
  while (action != "quit") {
    std::cout << "action:";
    std::getline(std::cin, action);
    if (action == "new") {
      std::cout << "ticker:";
      std::getline(std::cin, buffer);
      ticker = buffer;
      std::cout << "price:";
      std::getline(std::cin, buffer);
      double price = atof(buffer.c_str());
      std::cout << "size:";
      std::getline(std::cin, buffer);
      int size = atoi(buffer.c_str());
      std::cout << "side:(1-buy, 2-sell)";
      std::getline(std::cin, buffer);
      int side_int = atoi(buffer.c_str());
      if (side_int != 1 && side_int != 2) {
        cout << "wrong side, 1->buy 2->sell!" << endl;
        continue;
      }
      OrderSide::Enum side = (side_int == 1) ? OrderSide::Buy : OrderSide::Sell;
      char buf[1024];
      snprintf(buf, sizeof(buf), "Confirm OrderInfo:%s %lf@%d; 1-confirmed other-quit:", ticker.c_str(), price, size);
      std::cout << buf;
      std::getline(std::cin, buffer);
      int confirmed = atoi(buffer.c_str());
      if (confirmed == 1) {
        Order* o = new Order;
        o->action = OrderAction::NewOrder;
        snprintf(o->ticker, sizeof(o->ticker), "%s", ticker.c_str());
        o->price = price;
        o->side = side;
        o->size = size;
        snprintf(o->order_ref, sizeof(o->order_ref), "%s%d", ticker.c_str(), count++);
        sender->Send(*o);
        o->Show(stdout);
        std::cout << "*****************" <<  o->order_ref << " sent*****************\n";
        sleep(2);
      } else {
        std::cout << "*****************Order Quit!*****************" << std::endl;
        continue;
      }
    } else if (action == "cancel") {
      std::cout << "order_ref:";
      std::string order_ref;
      std::getline(std::cin, order_ref);
      Order* o = new Order;
      o->action = OrderAction::CancelOrder;
      snprintf(o->order_ref, sizeof(o->order_ref), "%s", order_ref.c_str());
      snprintf(o->ticker, sizeof(o->ticker), "%s", ticker.c_str());
      gettimeofday(&o->send_time, nullptr);
      sender->Send(*o);
      o->Show(stdout);
      std::cout << "*****************" <<  o->order_ref << "Cancel Sent*****************\n";
      sleep(2);
    } else {
      std::cout << "invaild action\n";
    }
  }
  exit(1);
}

void RunRecv(ZmqRecver<ExchangeInfo> * recver) {
  ExchangeInfo info;
  while (true) {
    recver->Recv(info);
    info.Show(stdout);
  }
}

int main() {
  std::unique_ptr<ZmqRecver<ExchangeInfo> > exchange_recver(new ZmqRecver<ExchangeInfo>("exchange_info"));
  std::thread recv_thread(RunRecv, exchange_recver.get());
  std::unique_ptr<ZmqSender<Order> > order_sender(new ZmqSender<Order>("order_recver"));
  RunSend(order_sender.get());
  recv_thread.join();
}
