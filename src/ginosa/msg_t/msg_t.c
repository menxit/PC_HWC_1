#include <stdlib.h>
#include "msg_t.h"

/**
 * Distruggi un messaggio
 *
 * @param msg
 */
void msg_destroy(msg_t *msg) {
  free(msg->content);
  free(msg);
}

/**
 * Copia un messaggio
 *
 * @param msg
 * @return
 */
msg_t *msg_copy(msg_t *msg) {
  msg_t *result = malloc(sizeof(msg_t));
  result->content = msg->content;
  return result;
}

/**
 * Costruttore di un messaggio
 *
 * @param content
 * @return
 */
msg_t *msg_init(void *content) {
  msg_t *msg = malloc(sizeof(msg_t));
  msg->content = content;
  msg->msg_destroy = msg_destroy;
  msg->msg_copy = msg_copy;
  return msg;
}
