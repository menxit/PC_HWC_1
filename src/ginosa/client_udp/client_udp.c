#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "client_udp.h"

/**
 * It's been a pleasure to meet you
 *
 * @param s
 */
static void die(char *s) {
  perror(s);
  exit(1);
}

/**
 * Open UDP connection
 *
 * @param this
 */
static void openConnection(client_udp* this) {
  if ((this->s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    die("socket");
  }
  memset((char *) this->si_other, 0, sizeof(*this->si_other));
  this->si_other->sin_family = AF_INET;
  this->si_other->sin_port = htons(this->port);
  struct hostent* hp = gethostbyname(this->host);
  if(hp == NULL) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }
  memcpy(&this->si_other->sin_addr, hp->h_addr, hp->h_length);
}

/**
 * Send a message on UDP connection
 *
 * @param this
 * @param message
 */
static char* sendMessage(struct client_udp* this, char* message) {

  char* result = malloc(this->maxMessageSize);

  //send the message
  if (sendto(this->s, message, strlen(message) + 1, 0, (struct sockaddr *) this->si_other, *(socklen_t *)this->slen) == -1) {
    die("sendto()");
  }

  //try to receive some data, this is a blocking call
  if (recvfrom(this->s, result, this->maxMessageSize, 0, (struct sockaddr *) this->si_other, (socklen_t*)this->slen) == -1) {
    die("recvfrom()");
  }

  return result;
}

/**
 * Close connection
 *
 * @param this
 */
void closeConnection(client_udp* this) {
  close(this->s);
}

/**
 * Initialize the UDP client
 *
 * @param serverHost
 * @param serverPort
 * @param maxMessageSize
 * @return
 */
client_udp *_new_client_udp (char *serverHost, unsigned short int serverPort)
{
  client_udp* this = malloc (sizeof (client_udp));
  this->host = serverHost;
  this->port = serverPort;
  this->maxMessageSize = 512;
  this->si_other = malloc (sizeof (struct sockaddr_in));
  this->slen = malloc (sizeof (int));
  *this->slen = sizeof(*this->si_other);
  this->openConnection = openConnection;
  this->sendMessage = sendMessage;
  this->closeConnection = closeConnection;
  return this;
}
