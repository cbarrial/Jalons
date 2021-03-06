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
  int iden ; // use for first Identification of the client with /nick
  char *name; // name of the client
  char *ip; // ip adress
  char *channel; // name of the channel the client joined
  int intochannel; // use for entry in channel
  char *chanel_creator; // if not empty, the client created a channel
  time_t date; // date

}client;

int do_socket(int domain, int type, int protocol);
ssize_t readline(int fd, char str[], size_t maxlen);
char *read_name(char tab1[],char tab2[]);
void error(const char *msg);
char *concat_string(char *s1,char *s2);
int send_list( char *msg, int conex, client *tabclient, int msg_size, int cactual);
int send_info(char *msg, client *tabclient, int msg_size, int nbclients, int cactual, char *portnb);
void ident(client *tabclient, int cactual, char *msg);
int broadcast(client *tabclient, int cactual,int i, int j, char *msg);
int unicast(client *tabclient, int cactual,int i, int j, char *msg, int conex);
int create_chanel(client *tabclient, int cactual,int i, int j, char *msg, char** tabchannel,int chanel_index);
int join(client *tabclient, char **tabchannel, int channel_index, char *msg, int actual);
int broadcast2(client *tabclient, int cactual,int i, char *j, char *msg);
int quit(char **tabchannel,int channel_index, char *msg, char *j2, int actual, int conex, client *tabclient);
int who_channel( char **tabchannel,int chanel_index, char *msg, int conex, client *tabclient, int msg_size, int cactual);
void server_send(char *filename, client *tabclient, int i);
void server_accept(char *filename, char *user, client *tabclient, int i, int conex);
void client_send(char *filename, int sock);
void client_accept(char *filename, char *msg_recv);
int isend(char *msg, client *tabclient, int conex, int i);
int answer(char *msg, int sockstock, client *tabclient, int cactual, int index);
char *NomFichier(char *fichier);
