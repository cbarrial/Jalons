#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#define CONNECT_ERROR -1
#define REC_ERROR -1
#include "functcom.h"


int main(int argc,char** argv)
{


    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    struct sockaddr_in sock_host;
    int sock;
    int msg_size=1000;
    char msg_recv[msg_size];
    char msg_sent[msg_size];

    memset(msg_sent, 0, msg_size);
    memset(msg_recv, 0, msg_size);

    //get address info from the server
    //get_addr_info()
    struct hostent *res;
    struct in_addr* addr;

    char *hostname = "localhost";
    res=gethostbyname(hostname);
    addr = (struct in_addr*) res->h_addr_list[0];



    sock_host.sin_addr = *addr;
    sock_host.sin_family = AF_INET;
    sock_host.sin_port = htons(atoi(argv[2]));


    //get the socket
    sock= socket(AF_INET, SOCK_STREAM, 0);

    //connect to remote socket
    int connection = connect(sock, (struct sockaddr*)&sock_host, sizeof(sock_host));
    if (connection==CONNECT_ERROR) {
      printf("Connection failed\n");
      error("connection");
    }



    //get user input
    for (;;) {

      int i=0;
      //send message to the server
      while (1){

        if (i==0){
          printf("\n[SERVER] please introduce yourself by using /nick <your pseudo>\n");
          fflush(stdout);
          readline(0,msg_sent,msg_size);
          write(sock,msg_sent,strlen(msg_sent));

          if (strcmp(msg_sent, "quit\n") == 0)
            exit(1);

          //handle_client_message()
          memset(msg_recv, '\0', msg_size);
          readline(sock,msg_recv,msg_size);
          char *nick = "/nick ";

          if (strncmp(msg_recv, nick, strlen(nick)) != 0){
              printf("[Server] : Wrong syntaxe\n");
              break;
          }

          else {
            char *name=read_name(msg_recv,"/nick ");
            printf("[Server]: Welcome to the chat " );
            fflush(stdout);
            write(1,name,strlen(name));
            printf("\n");
            i++;
          }
        }



        else{
          printf("\nEnter your message :\n");
          fflush(stdout);
          readline(0,msg_sent,msg_size);
          write(sock,msg_sent,strlen(msg_sent));


          if (strcmp(msg_sent, "quit\n") == 0)
            exit(1);

          //handle_client_message()

          if (strcmp(msg_sent, "/who\n") == 0 ){
              memset(msg_recv, '\0', msg_size);
              printf("\nList of user:\n");

              read(sock, msg_recv, msg_size);
              write(1,msg_recv,strlen(msg_recv));
          }


          else if (strncmp(msg_sent, "/whois", strlen("/whois")) == 0){
            memset(msg_recv, '\0', msg_size);
            printf("[Server1]: ");
            read(sock, msg_recv, msg_size);
            write(1,msg_recv,strlen(msg_recv));
            printf("\n");
          }


          else {
            memset(msg_recv, '\0', msg_size);
            readline(sock, msg_recv, msg_size);
            printf("[Server2]: ");
            fflush(stdout);
            write(1,msg_recv,strlen(msg_recv));
            printf("\n");
          }

        }
        }
      }
      close(sock);



      return 0;


}
