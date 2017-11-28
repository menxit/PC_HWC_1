#ifndef _BUFFER_T
#define _BUFFER_T

#include "../sem.h"
#include "../msg_t/msg_t.h"
#define BUFFER_ERROR NULL

typedef struct buffer_t {
  unsigned int T;
  unsigned int D;
  unsigned int N;
  pc_sem_t PIENE;
  pc_sem_t VUOTE;
  pc_sem_t USO_D;
  pc_sem_t USO_T;
  msg_t **data;
} buffer_t;

buffer_t *buffer_init(unsigned int maxSize);

msg_t *put_bloccante(buffer_t *buffer, msg_t *msg);

/**
 * Inserimento non bloccante: restituisce BUFFER_ERROR se pieno,
 * altrimenti effettua l'inserimento e restituisce il messaggio
 * inserito. N.B. msg != null
 *
 * @param buffer
 * @param msg
 * @return
 */
msg_t *put_non_bloccante(buffer_t *buffer, msg_t *msg);

/**
 * Estrazione bloccante: sospende se vuoto, quindi
 * restituisce il valore estratto non appena disponibile
 *
 * @param buffer
 * @return
 */
msg_t *get_bloccante(buffer_t *buffer);

/**
 * Estrazione non bloccante: restituisce BUFFER_ERROR se vuoto
 * ed il valore estratto in caso contrario.
 *
 * @param buffer
 * @return
 */
msg_t *get_non_bloccante(buffer_t *buffer);

#endif
