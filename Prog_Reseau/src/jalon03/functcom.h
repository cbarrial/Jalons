#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#define CONNECT_ERROR -1
#define REC_ERROR -1


typedef struct {
  int sockclient;
  int iden ;
  char *name;
  char *ip;
  time_t date;

}client;

ssize_t readline(int fd, char str[], size_t maxlen);
char *read_name(char tab1[],char tab2[]);
void error(const char *msg);
char *concat_string(char *s1,char *s2);
int send_list( char *msg, int conex, client *tabclient, int msg_size, int cactual);
int send_info(char *msg, client *tabclient, int msg_size, int nbclients, int cactual, char *portnb);
void ident(client *tabclient, int cactual, char *msg);
