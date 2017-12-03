#include <stdlib.h>
#include <pthread.h>
#include <msg_t/msg_t.h>
#include <buffer_t/buffer_t.h>
#include "ProduttoreBloccanteDiUnMessaggio.h"
#include "../Factory/Factory.h"

static void *task(void *args) {
  ProduttoreBloccanteDiUnMessaggio *this = (ProduttoreBloccanteDiUnMessaggio *) args;
  msg_t *msg = factory_createExpectedMessage();
  put_bloccante(this->buffer, msg);
  pc_sem_post(&this->messageIsProduced);
  pthread_exit(0);
}

ProduttoreBloccanteDiUnMessaggio *_new_ProduttoreBloccanteDiUnMessaggio(buffer_t *buffer) {
  ProduttoreBloccanteDiUnMessaggio *c = malloc(sizeof(ProduttoreBloccanteDiUnMessaggio));
  c->buffer = buffer;
  pc_sem_init(&c->messageIsProduced, 0);
  pthread_t thread;
  pthread_create(&thread, NULL, task, c);
  pthread_detach(thread);
  return c;
}
