#ifndef PROJECT_CLIENT_UDP_H
#define PROJECT_CLIENT_UDP_H

typedef struct client_udp {
  char *host;
  unsigned short int port;
  unsigned short int maxMessageSize;
  struct sockaddr_in *si_other;
  int *slen;
  int s;
  int i;
  char *buf;
  char *message;
  void (*openConnection)(struct client_udp *);
  char *(*sendMessage)(struct client_udp *, char *);
  void (*closeConnection)(struct client_udp *);
} client_udp;

client_udp *_new_client_udp(char *serverHost, unsigned short int serverPort);

#endif
