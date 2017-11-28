#include <stdlib.h>
#include <msg_t/msg_t.h>
#include <buffer_t/buffer_t.h>
#include "Factory.h"

/**
 * Crea e restituisce un messaggio
 *
 * @return
 */
msg_t *factory_createExpectedMessage() {
  int *content = malloc(sizeof(int));
  *content = EXPECTED_MSG;
  msg_t *msg = msg_init(content);
  return msg;
}

/**
 * Crea un buffer di dimensione sizeBuffer vuoto
 *
 * @param sizeBuffer
 * @return
 */
buffer_t *factory_createEmptyBuffer(unsigned int sizeBuffer) {
  buffer_t *b = buffer_init(sizeBuffer);
  return b;
}

/**
 * Crea un buffer di dimensione sizeBuffer pieno
 *
 * @param sizeBuffer
 * @return
 */
buffer_t *factory_createFullBuffer(unsigned int sizeBuffer) {
  buffer_t *b = buffer_init(sizeBuffer);
  int i = 0;
  while (i < sizeBuffer) {
    msg_t *msg = factory_createExpectedMessage();
    put_bloccante(b, msg);
    i++;
  }
  return b;
}

/**
 * Crea un buffer di dimensione sizeBuffer mezzo vuoto
 *
 * @param sizeBuffer
 * @return
 */
buffer_t *factory_createHalfFullBuffer(unsigned int sizeBuffer) {
  buffer_t *b = buffer_init(sizeBuffer);
  int i = 0;
  while (i < (sizeBuffer / 2)) {
    msg_t *msg = factory_createExpectedMessage();
    put_bloccante(b, msg);
    i++;
  }
  return b;
}

/**
 * Crea un consumatore bloccante di un messaggio
 *
 * @param buffer
 * @return
 */
ConsumatoreBloccanteDiUnMessaggio *factory_createConsumatoreBloccanteDiUnMessaggio(buffer_t *buffer) {
  return _new_ConsumatoreBloccanteDiUnMessaggio(buffer);
}

/**
 * Crea un produttore bloccante di un messaggio
 *
 * @param buffer
 * @return
 */
ProduttoreBloccanteDiUnMessaggio *factory_createProduttoreBloccanteDiUnMessaggio(buffer_t *buffer) {
  return _new_ProduttoreBloccanteDiUnMessaggio(buffer);
}

unsigned int factory_createRandomNumber(unsigned int min, unsigned int max) {
  return (rand() % (max + 1 - min)) + min;
}
