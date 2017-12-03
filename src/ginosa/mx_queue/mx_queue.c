#include <stdlib.h>
#include <string.h>
#include <printf.h>
#include "mx_queue.h"
#include <stdio.h>

/**
 * Rimuovi una sottostringa in una stringa
 *
 * @param s
 * @param toremove
 */
static void removeSubstring(char *s, const char *toremove) {
  while (s = strstr(s, toremove))
    memmove(s, s + strlen(toremove), 1 + strlen(s + strlen(toremove)));
}

/**
 * Questa funzione vede ciascun pacchetto ricevuto sulla coda. Se è un pacchetto di tipo EXTRACT
 * richiama la funzione onExtractmessage. Se invece è un pacchetto di tipo INSERT, richiama la funzione
 * di tipo onTryInsertMessage.
 *
 * @param fromAddress
 * @param port
 * @param message
 * @param queue
 * @return
 */
char *onMessageReceived(char *fromAddress, unsigned short int port, char *message, void *queue) {
  mx_queue *this = (mx_queue *) queue;
  if (strstr(message, EXTRACT_PATTERN) != NULL) {
    removeSubstring(message, EXTRACT_PATTERN);
    return this->onExtractMessage(this);
  }
  if (strstr(message, INSERT_PATTERN) != NULL) {
    removeSubstring(message, INSERT_PATTERN);
    char *content = malloc(sizeof(char) * strlen(message));
    memcpy(content, message, strlen(message));
    msg_t *msg = msg_init(content);
    return this->onTryInsertMessage(this, msg);
  }
  return ERROR_PATTERN_NOT_FOUND;
}

/**
 * Questa funzione viene richiamata quando qualcuno tenta di inserire un messaggio nella coda.
 * Se il buffer è può accogliere un nuovo messaggio, allora verrà inserito, altrimenti verrà restituito
 * un errore di tipo ERROR_RETRY_LATER.
 *
 * @param this
 * @param message
 * @return
 */
char *onTryInsertMessage(mx_queue *this, msg_t *message) {
  if (put_non_bloccante(this->buffer, message) == BUFFER_ERROR) {
    return ERROR_RETRY_LATER;
  }
  return MESSAGE_INSERTED;
}

/**
 * Questa funzione viene richiamata quando qualcuno tenta di estrarre un messaggio dalla coda.
 * Se non c'è nessun messaggio sulla coda, allora verrà restituito un ERROR_RETRY_LATER.
 *
 * @param this
 * @param message
 * @return
 */
char *onExtractMessage(mx_queue *this) {
  msg_t *msg = get_non_bloccante(this->buffer);
  if (msg == BUFFER_ERROR) {
    return ERROR_RETRY_LATER;
  }
  this->extractedMessage = malloc(strlen(msg->content));
  memcpy(this->extractedMessage, msg->content, strlen(msg->content));
  msg->msg_destroy(msg);
  return this->extractedMessage;
}

/**
 * La coda accetta estrazione e inserimento di messaggi
 *
 * @param this
 */
void start(mx_queue *this) {
  printf("=> '%s' è in ascolto.\n", this->name);
  this->server->listenConnections(this->server);
}

mx_queue *_new_mx_queue(unsigned short port, unsigned int maxSize, char *name) {
  mx_queue *this = malloc(sizeof(mx_queue));
  this->name = name;
  this->buffer = buffer_init(maxSize);
  this->server = _new_server_udp(port, maxSize, onMessageReceived, this);
  this->onTryInsertMessage = onTryInsertMessage;
  this->onExtractMessage = onExtractMessage;
  this->start = start;
  return this;
}
