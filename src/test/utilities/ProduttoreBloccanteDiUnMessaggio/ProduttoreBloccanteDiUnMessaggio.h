#ifndef HC_C1_PRODUTTOREBLOCCANTEDIUNMESSAGGIO_H
#define HC_C1_PRODUTTOREBLOCCANTEDIUNMESSAGGIO_H

typedef struct ProduttoreBloccanteDiUnMessaggio {
  buffer_t *buffer;
  pc_sem_t messageIsProduced;
  void *(*task)(void *args);
} ProduttoreBloccanteDiUnMessaggio;

ProduttoreBloccanteDiUnMessaggio *_new_ProduttoreBloccanteDiUnMessaggio(buffer_t *buffer);

#endif
