#include <stdlib.h>
#include <pthread.h>
#include <buffer_t/buffer_t.h>
#include <printf.h>
#include "ConsumatoreBloccanteDiUnMessaggio.h"

static void *task(void *args) {
  ConsumatoreBloccanteDiUnMessaggio *this = (ConsumatoreBloccanteDiUnMessaggio *) args;
  msg_t *msg = get_bloccante(this->buffer);
  pc_sem_post(&this->messageIsConsumed);
  msg->msg_destroy(msg);
  pthread_exit(0);
}

ConsumatoreBloccanteDiUnMessaggio *_new_ConsumatoreBloccanteDiUnMessaggio(buffer_t *buffer) {
  ConsumatoreBloccanteDiUnMessaggio *c = malloc(sizeof(ConsumatoreBloccanteDiUnMessaggio));
  c->buffer = buffer;
  pc_sem_init(&c->messageIsConsumed, 0);
  pthread_t thread;
  pthread_create(&thread, NULL, task, c);
  pthread_detach(thread);
  return c;
}
