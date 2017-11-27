#include <stdio.h>
#include <mx_queue/mx_queue.h>
#include <stdlib.h>
#include <stdio.h>

unsigned short getPort() {
  return (unsigned short) strtol(getenv("PORT"), (char **)NULL, 10);
}

char* getQueueName() {
  return getenv("QUEUE_NAME");
}

int main () {
  mx_queue* queue = _new_mx_queue(getPort(), 10, getQueueName());
  queue->start(queue);
  return 0;
}
