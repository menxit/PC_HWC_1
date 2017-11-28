#ifndef _MSG_T
#define _MSG_T

typedef struct msg_t {
  void *content;
  void (*msg_destroy)(struct msg_t *);
  struct msg_t *(*msg_copy)(struct msg_t *);
} msg_t;

msg_t *msg_init(void *);

#endif
