#include <stdlib.h>
#include "msg_t.h"

/**
 * Destroy a message
 *
 * @param msg
 */
void msg_destroy(msg_t* msg) {
  free(msg->content);
  free(msg);
}

/**
 * Copy a message
 *
 * @param msg
 * @return
 */
msg_t* msg_copy(msg_t* msg) {
  msg_t* result = malloc(sizeof(msg_t));
  // @TODO: potrebbe essere una buona idea copiare lo spazio di memoria, invece del riferimento.
  result->content = msg->content;
  return result;
}

/**
 * Init a message
 *
 * @param content
 * @return
 */
msg_t* msg_init(void* content) {
  msg_t* msg = malloc(sizeof(msg_t));
  msg->content = content;
  msg->msg_destroy = msg_destroy;
  msg->msg_copy = msg_copy;
  return msg;
}
