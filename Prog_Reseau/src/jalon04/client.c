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
    fd_set fd_set_read;

    memset(msg_sent, 0, msg_size);
    memset(msg_recv, 0, msg_size);

    //get address info from the server
    struct hostent *res;
    struct in_addr* addr;

    char *hostname = "localhost";
    res=gethostbyname(hostname);
    addr = (struct in_addr*) res->h_addr_list[0];



    sock_host.sin_addr = *addr;
    sock_host.sin_family = AF_INET;
    sock_host.sin_port = htons(atoi(argv[2]));


    //get the socket
    sock= do_socket(AF_INET, SOCK_STREAM, 0);

    //connect to remote socket
    int connection = connect(sock, (struct sockaddr*)&sock_host, sizeof(sock_host));
    if (connection==CONNECT_ERROR) {
      printf("Connection failed\n");
      error("connection");
    }

    int salon = 0;
    int i=0;
    char *channel;
    //get user input
    for (;;) {


            //send message to the server
            //    while (1){

              if (i==0){
                printf("\n[SERVER] please introduce yourself by using /nick <your pseudo>\n");
                fflush(stdout);
                FD_ZERO(&fd_set_read);
                FD_SET(sock, &fd_set_read);
                FD_SET(fileno(stdin), &fd_set_read);
                int max_fd =sock+1;
                int sel=0;
                sel = select(max_fd, &fd_set_read, NULL, NULL, NULL);

                int a;
                for ( a = 0 ; a<max_fd && sel>=0; a++){
                  //memset(msg_sent, '\0', msg_size);
                  if (FD_ISSET(a, &fd_set_read)){
                    if (a == fileno(stdin)){
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
                        printf("[Server] : Welcome to the chat " );
                        fflush(stdout);
                        write(1,name,strlen(name));
                        printf("\n");
                        i++;
                      }
                    }
                    sel--;
                    }
                  }
                }


                else if (salon>0){

                  printf("\n[%s] : ", channel);
                  fflush(stdout);
                  FD_ZERO(&fd_set_read);
                  FD_SET(sock, &fd_set_read);
                  FD_SET(fileno(stdin), &fd_set_read);
                  int max_fd =sock+1;
                  int sel=0;
                  sel = select(max_fd, &fd_set_read, NULL, NULL, NULL);

                  int a;
                  for ( a = 0 ; a<max_fd && sel>=0; a++){
                    if (FD_ISSET(a, &fd_set_read)){
                      if (a == fileno(stdin)){
                        readline(0,msg_sent,msg_size);
                        char *msg_sent2;
                        msg_sent2 = malloc(sizeof(char)*36);
                        msg_sent2 = concat_string("/msg2all ", msg_sent);
                        write(sock,msg_sent2,strlen(msg_sent2));

                        char *quit;
                        quit = malloc(sizeof(char)*36);
                        quit = concat_string("/quit ", channel);

                        if (strncmp(msg_sent, quit, strlen(quit)) == 0){
                          memset(msg_recv, '\0', msg_size);
                          readline(sock, msg_recv, msg_size);
                          printf("%d\n", r);
                          write(1,msg_recv,strlen(msg_recv));
                          salon=0;
                          break;
                        }



                        //handle_client_message()

                        if (strncmp(msg_sent, "/join", strlen("/join")) == 0){
                          printf("[%s] : First you have to quit the channel %s (use /quit)\n", channel, channel);
                        }

                        else {
                          memset(msg_recv, '\0', msg_size);
                          readline(sock, msg_recv, msg_size);
                          fflush(stdout);

                        }

                      }

                  else {
                      memset(msg_recv, '\0', msg_size);
                      read(sock, msg_recv, msg_size);
                      fflush(stdout);
                      write(1,msg_recv,strlen(msg_recv));
                      printf("\n");

                  }
                  sel --;
                }
              }

            }
              else{

                printf("\nEnter your message :\n");
                fflush(stdout);
                FD_ZERO(&fd_set_read);
                FD_SET(sock, &fd_set_read);
                FD_SET(fileno(stdin), &fd_set_read);
                int max_fd =sock+1;
                int sel=0;
                sel = select(max_fd, &fd_set_read, NULL, NULL, NULL);

                int a;
                for ( a = 0 ; a<max_fd && sel>=0; a++){
                  //memset(msg_sent, '\0', msg_size);
                  if (FD_ISSET(a, &fd_set_read)){
                    if (a == fileno(stdin)){
                      readline(0,msg_sent,msg_size);
                      write(sock,msg_sent,strlen(msg_sent));



                      if (strcmp(msg_sent, "quit\n") == 0)
                        exit(1);

                      //handle_client_message()

                      if (strcmp(msg_sent, "/who\n") == 0 ){
                          printf("\nList of user:\n");
                          memset(msg_recv, '\0', msg_size);
                          read(sock, msg_recv, msg_size);
                          write(1,msg_recv,strlen(msg_recv));
                      }


                      else if (strncmp(msg_sent, "/whois", strlen("/whois")) == 0){
                        memset(msg_recv, '\0', msg_size);
                        read(sock, msg_recv, msg_size);
                        write(1,msg_recv,strlen(msg_recv));
                        printf("\n");
                      }

                      else if (strncmp(msg_sent, "/msgall", strlen("/msgall")) == 0){
                        printf("[Server] : Message sent to all\n");
                      }

                      else if (strncmp(msg_sent, "/msg", strlen("/msg")) == 0){
                        printf("[Server] : Message sent\n");
                      }

                      else if (strncmp(msg_sent, "/whois", strlen("/whois")) == 0){
                        memset(msg_recv, '\0', msg_size);
                        read(sock, msg_recv, msg_size);
                        write(1,msg_recv,strlen(msg_recv));
                        printf("\n");
                      }

                      else if (strncmp(msg_sent, "/join", strlen("/join")) == 0){
                        salon++;
                        channel = malloc(sizeof(char)*36);
                        channel = read_name(msg_sent, "/join ");
                        channel[strlen(channel)-1]='\0';
                        read(sock, msg_recv, msg_size);
                        if (strncmp(msg_recv, "This channel doesn't exists", strlen("This channel doesn't exists"))==0){
                          salon--;
                        }
                        write(1,msg_recv,strlen(msg_recv));
                        printf("\n");
                      }

                      else {
                        memset(msg_recv, '\0', msg_size);
                        read(sock, msg_recv, msg_size);
                        printf("[Server] : ");
                        fflush(stdout);
                        write(1,msg_recv,strlen(msg_recv));
                        printf("\n");
                      }

                    }
                else {

                  memset(msg_recv, '\0', msg_size);
                  read(sock, msg_recv, msg_size);
                  fflush(stdout);
                  write(1,msg_recv,strlen(msg_recv));
                  printf("\n");
                }
                sel --;
              }
            }
          }

    }

      close(sock);
      return 0;


}
