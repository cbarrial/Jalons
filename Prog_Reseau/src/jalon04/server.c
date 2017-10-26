#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define SOCKET_ERROR -1
#define BIND_ERROR -1
#define LISTEN_ERROR -1
#define NO_WHO -1
#define NO_WHOIS -1
#define NO_ALL -1
#define NO_UNI -1
#define NO_CREATE -1
#include "functcom.h"


int main(int argc, char** argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }


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
                if (conex-1>20){
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

                ident(tabclient, i, msg);

                int list = send_list(msg, conex, tabclient, msg_size, i);

                int info = send_info(msg, tabclient, msg_size, conex, i, argv[1]);

                int msgall;
                int msgall2;
                int k;
                int l;
                int z=1;


                  if (join(tabclient,tabchannel,chanel_index,msg,l,i)==0){
                    tabclient[i].intochannel =1;
                    z=0;
                  }


                int quittest = quit(msg, j2, i,  conex, tabclient);
                printf("%d\n", quittest);

                int uni=unicast(tabclient, i, k, j, msg, conex);

                int create=create_chanel(tabclient, i, k, j, msg,tabchannel,chanel_index);
                if (create!=-1){
                  chanel_index++;
                }


                for (k=1;k<conex;k++){

                  msgall=broadcast(tabclient, i, k, j, msg);
                  if (tabclient[i].intochannel==1){
                    msgall2=broadcast2(tabclient, i, k, j2, msg);
                  }

                }

                printf("Message received by client %s\n",tabclient[i].name);


                if (strcmp(msg, "quit\n") == 0){
                  int clos=tabclient[i].sockclient-3;
                  close(tabclient[i].sockclient);
                  tabclient[i].sockclient=0;
                  conex=conex-1;
                  printf("Client %d is deconnected\n",clos);
                  if (conex<=1){
                    break;
                  }
                }

                if (list == NO_WHO && info == NO_WHOIS && msgall==NO_ALL && msgall2==NO_ALL && uni==NO_UNI && create==NO_CREATE && z==1 && quittest == -1){
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
