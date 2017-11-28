#ifndef HC_C1_MX_QUEUE_H
#define HC_C1_MX_QUEUE_H

#include "../server_udp/server_udp.h"
#include "../buffer_t/buffer_t.h"

#define EXTRACT_PATTERN "EXTRACT$$$"
#define INSERT_PATTERN "INSERT$$$"
#define ERROR_PATTERN_NOT_FOUND "ERROR_PATTERN_NOT_FOUND"
#define ERROR_RETRY_LATER "ERROR_RETRY_LATER"
#define MESSAGE_INSERTED "MESSAGE_INSERTED"

typedef struct mx_queue {
  server_udp *server;
  buffer_t *buffer;
  char *name;
  void (*start)(struct mx_queue *this);
  char *extractedMessage;
  char *(*onTryInsertMessage)(struct mx_queue *this, msg_t *message);
  char *(*onExtractMessage)(struct mx_queue *this);
} mx_queue;

mx_queue *_new_mx_queue(unsigned short port, unsigned int maxSize, char *name);

#endif
