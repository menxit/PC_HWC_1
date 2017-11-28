#include <mx_node/mx_node.h>
#include <printf.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

char* getAddressSubscriber() {
  return getenv("SUBSCRIBER_ADDRESS");
}

unsigned short getPortSubscriber() {
  return (unsigned short) strtol(getenv("SUBSCRIBER_PORT"), (char **)NULL, 10);
}

unsigned short getSleep() {
  return (unsigned short) strtol(getenv("SLEEP"), (char **)NULL, 10);
}

void onMessageReceived(mx_node* this, msg_t* msg) {
  sleep(getSleep());
  printf ("RICEVUTO: %s\n", (char*)msg->content);
}

int main(void) {
  mx_node* node = _new_mx_node(10);
  node->createSubscriber(node, getAddressSubscriber(), getPortSubscriber(), onMessageReceived);
}
