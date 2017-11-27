#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <printf.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "server_udp.h"

static void die(char *s) {
  perror(s);
  exit(1);
}

/**
 * Close the connection
 *
 * @param this
 */
static void closeConnection(server_udp* this) {
  close(this->s);
}

/**
 * Listen connections
 *
 * @param this
 */
static void listenConnections(server_udp *this) {

  char* buf;

  //create a UDP socket
  if ((this->s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    die("socket");
  }

  // zero out the structure
  memset((char *) this->si_me, 0, sizeof(*this->si_me));

  this->si_me->sin_family = AF_INET;
  this->si_me->sin_port = htons(this->port);
  this->si_me->sin_addr.s_addr = htonl(INADDR_ANY);

  //bind socket to port
  if(bind(this->s , (struct sockaddr*)this->si_me, sizeof(*this->si_me) ) == -1) {
    die("bind");
  }
  //keep listening for data
  while(1) {
    buf = malloc(this->maxMessageSize);

		//try to receive some data, this is a blocking call
		if ((this->recv_len = (int)recvfrom(this->s, buf, this->maxMessageSize, 0, (struct sockaddr *) this->si_other, (socklen_t*)this->slen)) == -1) {
		  die("recvfrom()");
		}

		//print details of the client/peer and the data received
		char* response = this->onMessageReceived(inet_ntoa(this->si_other->sin_addr), ntohs(this->si_other->sin_port), buf, this->other);

		size_t responseSize = strlen(response)* sizeof(char);
		//now reply the client with the same data
		if (sendto(this->s, response, responseSize, 0, (struct sockaddr*) this->si_other, *(socklen_t*)this->slen) == -1) {
		  die("sendto()");
		}
  }
}


/**
 * Create a new udp server
 *
 * @param port
 * @param maxMessageSize
 * @return
 */
server_udp* _new_server_udp(
  unsigned short int port,
  unsigned short int maxMessageSize,
  char* (*onMessageReceived)(char*, unsigned short int, char*, void*),
  void* onMessageReceivedArgs) {
  server_udp* this = malloc (sizeof (server_udp));
  this->port = port;
  this->maxMessageSize = maxMessageSize;
  this->si_me = malloc (sizeof (struct sockaddr_in));
  this->si_other = malloc (sizeof (struct sockaddr_in));
  this->slen = malloc(sizeof (int));
  *this->slen = sizeof(*this->si_me);
  this->other = onMessageReceivedArgs;
  this->listenConnections = listenConnections;
  this->onMessageReceived = onMessageReceived;
  this->closeConnection = closeConnection;
  return this;
}