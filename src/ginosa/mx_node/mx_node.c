#include <stdlib.h>
#include <string.h>
#include <printf.h>
#include <stdio.h>
#include <math.h>
#include "mx_node.h"
#include "../mx_queue/mx_queue.h"

char* concat(const char *s1, const char *s2) {
  size_t lenS1 = strlen (s1);
  size_t lenS2 = strlen (s2);
  char *result = malloc(lenS1+lenS2);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

static int randomNumber(int lowerLimit, int upperLimit) {
  srand(time(NULL));
  return lowerLimit + rand() % (upperLimit - lowerLimit);
}

void createSubscriber(mx_node* this, char* address, unsigned short int port, void (*onMessageReceived)(struct mx_node*, msg_t*)) {
  char* packetReceived;
  this->queueSubscribe = _new_client_udp (address, port);
  this->queueSubscribe->openConnection(this->queueSubscribe);
  while(1) {
    packetReceived = this->queueSubscribe->sendMessage(this->queueSubscribe, "EXTRACT$$$");
    msg_t* msg = msg_init(packetReceived);
    if(strcmp(packetReceived, ERROR_RETRY_LATER) != 0) {
      onMessageReceived(this, msg);
    }
  }
}

void createPublisher(mx_node* this, char* address, unsigned short int port) {
  this->queuePublish = _new_client_udp (address, port);
  this->queuePublish->openConnection(this->queuePublish);
}

static int exponentialFunction(long x, int max) {
  double SPEED = 0.1;
  return (max/1000000*(1-pow(M_E, (-SPEED*x))))*1000000;
}

void publish(mx_node* this, msg_t* message) {
  char* content;
  unsigned long i = 0;
  int MAX_NUMBER_ATTEMPTS = 10;
  int MAX_TIMEOUT = 3000000;
  do {
    //printf("[x]");
    i = (i+1)%MAX_NUMBER_ATTEMPTS;
    usleep(exponentialFunction(i, MAX_TIMEOUT));
    char* msg = concat("INSERT$$$", message->content);
    content = this->queuePublish->sendMessage(this->queuePublish, msg);
  } while(strcmp(content, ERROR_RETRY_LATER) == 0);
}

mx_node* _new_mx_node(unsigned int bufferSize) {
  mx_node* this = malloc(sizeof(mx_node));
  this->buffer = buffer_init(bufferSize);
  this->createPublisher = createPublisher;
  this->createSubscriber = createSubscriber;
  this->publish = publish;
  return this;
}
