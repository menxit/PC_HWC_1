#include <mx_node/mx_node.h>
#include <printf.h>
#include <stdlib.h>
#include <stdio.h>

char* getAddressPublisher() {
  return getenv("PUBLISHER_ADDRESS");
}

unsigned short getPortPublisher() {
  return (unsigned short) strtol(getenv("PUBLISHER_PORT"), (char **)NULL, 10);
}

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
  this->publish(this, msg);
}

int main(void) {
  msg_t* m1 = msg_init("another message");
  mx_node* node = _new_mx_node(10);
  node->createPublisher(node, getAddressPublisher(), getPortPublisher());
  node->createSubscriber(node, getAddressSubscriber(), getPortSubscriber(), onMessageReceived);
}
