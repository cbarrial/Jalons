#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#define SOCKET_ERROR -1
#define BIND_ERROR -1
#define LISTEN_ERROR -1
#define NO_WHO -1
#define NO_WHOIS -1
#define NO_ALL -1
#define NO_UNI -1
#define NO_CREATE -1
#define NO_JOIN -1
#define NO_QUIT -1
#include "functcom.h"


int main(int argc, char** argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }

    //Declaration
    struct sockaddr_in sin;
    int msg_size=1000;
    char msg[msg_size];
    int sock ;
    int bind_err;
    int list_err;
    char **tabchannel;
    tabchannel=malloc(sizeof(char)*100);



    //create the socket
    sock = do_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Check for the socket validity
    if (sock == SOCKET_ERROR){
      error("socket");
    }
    else {

      //init the sin structure


      //clean the sin structure
      memset(&sin, 0, sizeof(sin));
      //Bind to any IP from the host
      sin.sin_addr.s_addr = INADDR_ANY;
      //Internet family protocol
      sin.sin_family = AF_INET;
      //Bind on the TCP port
      sin.sin_port = htons(atoi(argv[1]));

      //perform the binding
      //we bind on the tcp port specified
      bind_err=bind(sock, (struct sockaddr*)&sin, sizeof(sin));
      if ( bind_err == BIND_ERROR){
        error("bind");
      }
      else {

        //specify the socket to be a server socket and listen for at most 20 concurrent client
        list_err=listen(sock, 20);
        if (list_err == LISTEN_ERROR){
          error("listen");
        }
        else {
          printf("Connection on the port %d\n", atoi(argv[1]));

          //init the fdset
          fd_set lecture;
          int n=20;

          client tabclient[n];
          int csock;
          int conex=0;
          int i;
          int chanel_index=0;


          //initialisation of elements
          for (i=0;i < n ;i++ ){

            tabclient[i].sockclient=0;
            tabclient[i].iden = 0;
            tabclient[i].name="";
            tabclient[i].channel="";
            tabclient[i].intochannel=1;
            tabclient[i].chanel_creator="";
          }
          tabclient[0].sockclient=sock;
          conex=conex+1;
          int index;
          int sockstock;
          char *filename;
          char *send;
          char *user;

          for (;;){

            FD_ZERO(&lecture);
            FD_SET(sock,&lecture);


            int i;
            int max_sock=sock;
            for (i=0;i<n;i++){
              if (tabclient[i].sockclient>0){
                FD_SET(tabclient[i].sockclient,&lecture);
              }
              if (tabclient[i].sockclient>max_sock){
                max_sock=tabclient[i].sockclient;
              }
            }

            int sel=select(max_sock+1,&lecture,NULL,NULL,NULL);
            if (sel==-1){
              error("select");
            }

            if (FD_ISSET(sock,&lecture)!=0){
              struct sockaddr_in csin;
              socklen_t taille = sizeof(csin);
              csock = accept(sock, (struct sockaddr*)&csin, &taille);
              if (csock == BIND_ERROR){
                error("accept");
              }
              else {
                printf("Client %d is connecting with the socket %d\n", csock-3,csock);
                conex=conex+1;
                tabclient[conex-1].sockclient=csock;
                tabclient[conex-1].date = time(NULL);
                tabclient[conex-1].ip = inet_ntoa(csin.sin_addr);
                if (conex-1>20){ //only 20 clients on the chat
                  write(tabclient[conex-1].sockclient, "Server cannot accept incoming connections anymore. Try again later.", sizeof(char)*60);
                  tabclient[conex-1].sockclient=0;
                  conex=conex-1;

                }

                }
              }


            for (i=1;i<n;i++){
              memset(msg, 0, msg_size);


              if (FD_ISSET(tabclient[i].sockclient, &lecture)!=0){
                int size=readline(tabclient[i].sockclient,msg,msg_size);
                int j=tabclient[i].sockclient;
                char *j2=tabclient[i].channel;

                if (index == 1){
                  int type_answer = answer(msg, sockstock, tabclient, i, index);
                  if (type_answer == 1){
                    //server_accept(filename, tabclient, i);
                    //server_send(filename, tabclient, i);
                  }
                }

                ident(tabclient, i, msg); //Identification of the client

                int list = send_list(msg, conex, tabclient, msg_size, i); // function /who

                int info = send_info(msg, tabclient, msg_size, conex, i, argv[1]); //function /whois

                int whocha=who_channel(tabchannel,chanel_index, msg, conex, tabclient, msg_size, i); //function /who_channel

                int isend_test= isend(msg, tabclient, conex, i);

                if ( isend_test == 0){
                  index = 1;
                  sockstock = tabclient[i].sockclient;
                  user = malloc(sizeof(char)*36);
                  send = malloc(sizeof(char)*36);
                  filename = malloc(sizeof(char)*36);
                  sscanf(msg, "%s %s %s", send, user, filename);
                }

                int msgall;
                int msgall2;
                int whojoin=-1;
                int k;
                int l;
                int z=1;


                  if (join(tabclient,tabchannel,chanel_index,msg,i)==0){ // if the client joint the channel, function /join
                    tabclient[i].intochannel =1;
                    z=0;
                  }


                int quittest = quit(tabchannel,chanel_index,msg, j2, i,  conex, tabclient); // quit the channel

                int uni=unicast(tabclient, i, k, j, msg, conex);// function /msg

                int create=create_chanel(tabclient, i, k, j, msg,tabchannel,chanel_index);// function /create

                if (create!=-1){
                  chanel_index++; //increase channel index
                }

                for (k=1;k<conex;k++){ //all connected clients
                  msgall=broadcast(tabclient, i, k, j, msg); //function /msgall
                  if (tabclient[i].intochannel==1){ //if client in a channel
                    msgall2=broadcast2(tabclient, i, k, j2, msg);
                  }
                }

                printf("Message received by client %s\n",tabclient[i].name);


                if (strcmp(msg, "quit\n") == 0){ // message starts with quit, client wants to quit the chat
                  int clos=tabclient[i].sockclient-3;
                  close(tabclient[i].sockclient); //close the client socket
                  tabclient[i].sockclient=0;
                  conex=conex-1; // decrease number of clients
                  printf("Client %d is deconnected\n",clos);
                  if (conex<=1){ //no more clients
                    break;
                  }
                }
                // specific cases of the functions
                if (list == NO_WHO && quittest==NO_QUIT && whojoin== NO_JOIN && whocha== NO_WHO && info == NO_WHOIS && msgall==NO_ALL && msgall2==NO_ALL && uni==NO_UNI && create==NO_CREATE && z==1){
                  //we write back to the client
                  write(tabclient[i].sockclient, msg, size);
                }

            }
          }

          if (strcmp(msg, "quit\n") == 0){
            printf("Server is closed\n");
            break;
          }
        }






        }
      }
    }

      //clean up server socket
      close(sock);
      return 0;
}
