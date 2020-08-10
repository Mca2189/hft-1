#include <zmq.hpp>
#include "struct/order.h"

int main() {
    zmq::context_t context(1);
    zmq::socket_t sub(context, ZMQ_XSUB);
    sub.bind("ipc://order_sender");
    zmq::socket_t pub(context, ZMQ_XPUB);
    pub.bind("ipc://order_recver");
    zmq::proxy(sub, pub, NULL);
    return 0;
}
