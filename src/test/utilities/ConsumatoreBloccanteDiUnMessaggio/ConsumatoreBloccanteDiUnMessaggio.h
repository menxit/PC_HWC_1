#ifndef HC_C1_CONSUMATOREBLOCCANTEDIUNMESSAGGIO_H
#define HC_C1_CONSUMATOREBLOCCANTEDIUNMESSAGGIO_H

typedef struct ConsumatoreBloccanteDiUnMessaggio {
  buffer_t *buffer;
  pc_sem_t messageIsConsumed;
  void *(*task)(void *args);
} ConsumatoreBloccanteDiUnMessaggio;

ConsumatoreBloccanteDiUnMessaggio *_new_ConsumatoreBloccanteDiUnMessaggio(buffer_t *buffer);

#endif
