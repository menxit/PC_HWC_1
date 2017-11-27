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

unsigned short getSleep() {
  return (unsigned short) strtol(getenv("SLEEP"), (char **)NULL, 10);
}

int main(void) {
  msg_t* m1 = msg_init("a message");
  mx_node* node = _new_mx_node(10);
  node->createPublisher(node, getAddressPublisher(), getPortPublisher());
  while(1) {
    sleep(getSleep());
    node->publish(node, m1);
  }
}
