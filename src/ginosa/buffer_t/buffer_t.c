#include <stdlib.h>
#include "buffer_t.h"

/**
 * Incrementa l'indice D di 1
 *
 * @param buffer
 * @return
 */
static void incrementD(buffer_t *buffer) {
  unsigned int N = buffer->N;
  unsigned int D = buffer->D;
  unsigned int result = (D + 1) % N;
  buffer->D = result;
}

/**
 * Incrementa l'indice T di 1
 *
 * @param buffer
 * @return
 */
static void incrementT(buffer_t *buffer) {
  unsigned int N = buffer->N;
  unsigned int T = buffer->T;
  buffer->T = (T + 1) % N;
}

/**
 * Inserimento non bloccante: restituisce BUFFER_ERROR se pieno,
 * altrimenti effettua l’inserimento e restituisce il messaggio
 * inserito; N.B.: msg!=null
 *
 * @param buffer
 * @param msg
 * @return
 */
msg_t *put_non_bloccante(buffer_t *buffer, msg_t *msg) {
  if (pc_sem_trywait(&buffer->VUOTE) != 0 || pc_sem_trywait(&buffer->USO_D) != 0) {
    return BUFFER_ERROR;
  }

  buffer->data[buffer->D] = msg;
  incrementD(buffer);

  pc_sem_post(&buffer->USO_D);
  pc_sem_post(&buffer->PIENE);
  return msg;
}

/**
 * Inserimento bloccante
 *
 * @param buffer
 * @param msg
 * @return
 */
msg_t *put_bloccante(buffer_t *buffer, msg_t *msg) {
  if (pc_sem_wait(&buffer->VUOTE) != 0 || pc_sem_wait(&buffer->USO_D) != 0) {
    return BUFFER_ERROR;
  }

  buffer->data[buffer->D] = msg;
  incrementD(buffer);

  pc_sem_post(&buffer->USO_D);
  pc_sem_post(&buffer->PIENE);
  return msg;
}

/**
 * Get non bloccate
 *
 * @param buffer
 * @return
 */
msg_t *get_non_bloccante(buffer_t *buffer) {
  if (pc_sem_trywait(&buffer->PIENE) != 0 || pc_sem_trywait(&buffer->USO_T) != 0) {
    return BUFFER_ERROR;
  }

  msg_t *msg = buffer->data[buffer->T];
  incrementT(buffer);

  pc_sem_post(&buffer->USO_T);
  pc_sem_post(&buffer->VUOTE);
  return msg;
}

/**
 * Get bloccante
 *
 * @param buffer
 * @return
 */
msg_t *get_bloccante(buffer_t *buffer) {
  if (pc_sem_wait(&buffer->PIENE) != 0 || pc_sem_wait(&buffer->USO_T) != 0) {
    return BUFFER_ERROR;
  }

  msg_t *msg = buffer->data[buffer->T];
  incrementT(buffer);

  pc_sem_post(&buffer->USO_T);
  pc_sem_post(&buffer->VUOTE);
  return msg;
}

/**
 * Creazione di un buffer vuoto di dimensione max nota
 *
 * @param maxSize
 * @return
 */
buffer_t *buffer_init(unsigned int maxSize) {
  buffer_t *buffer = malloc(sizeof(buffer_t));
  buffer->D = 0;
  buffer->T = 0;
  buffer->N = maxSize;
  buffer->data = malloc(sizeof(struct msg_t *) * maxSize);

  // inizializzo semafori in stile cooperativo
  pc_sem_init(&buffer->PIENE, 0);
  pc_sem_init(&buffer->VUOTE, maxSize);

  // inizializzo semafori in stile competitivo
  pc_sem_init(&buffer->USO_D, 1);
  pc_sem_init(&buffer->USO_T, 1);

  return buffer;
}
