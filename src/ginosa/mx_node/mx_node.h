#ifndef HC_C1_MX_NODE_H
#define HC_C1_MX_NODE_H

#include "../buffer_t/buffer_t.h"
#include "../client_udp/client_udp.h"

typedef struct mx_node {
  buffer_t *buffer;
  client_udp *queueSubscribe;
  client_udp *queuePublish;
  void (*createPublisher)(struct mx_node *, char *address, unsigned short int port);
  void (*publish)(struct mx_node *, msg_t *msg);
  void (*createSubscriber)(struct mx_node *,
                           char *address,
                           unsigned short int port,
                           void (*onMessageReceived)(struct mx_node *, msg_t *));
} mx_node;

mx_node *_new_mx_node(unsigned int bufferSize);

#endif
