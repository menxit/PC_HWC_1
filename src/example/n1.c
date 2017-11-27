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
  msg_t** messages = malloc(sizeof(msg_t*)*3);
  messages[0] = msg_init("ciao, mondo");
  messages[1] = msg_init("hello, world");
  messages[2] = msg_init("こんにちは世界");

  mx_node* node = _new_mx_node(10);
  node->createPublisher(node, getAddressPublisher(), getPortPublisher());
  while(1) {
    sleep(getSleep());
    srand(time(NULL));
    int lowerLimit = 0;
    int upperLimit = 2;
    int r =  lowerLimit + rand() % (upperLimit - lowerLimit + 1);
    node->publish(node, messages[r]);
  }
}
