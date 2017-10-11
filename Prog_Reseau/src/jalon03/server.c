#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define SOCKET_ERROR -1
#define BIND_ERROR -1
#define LISTEN_ERROR -1

typedef struct {
  int sockclient;
  int iden ;
  char *name;
}client;

char *read_name(char tab1[],char tab2[]){
  int j=0;
  int i=0;
  char msg[1000];
  while (i<=strlen(tab1)){
    if (tab1[i]==tab2[i]){
      i++;
    }
    else {
      msg[j]= tab1[i];
      i++;
      j++;
    }

  }
  return msg;
}

ssize_t readline(int fd, char str[], size_t maxlen){
  int i, a;
  char caract, *tab;
  tab = str;
  int j=0;

  for (i=0 ; i<maxlen ; i++){

    a= read(fd, &caract, 1);

    if (a==1){
      tab[j++]=caract;
      if (caract == '\n'){
        tab[j]='\0';
        return i+1;
      }
    }
    else {
      break;
    }
  }

  return i;
}


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

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


    //create the socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
        //listen()
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


          for (i=0;i < n ;i++ ){

            tabclient[i].sockclient=0;
            tabclient[i].name = "";
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
                tabclient[conex-1].iden = 0;
                /*if (tabclient[conex-1].name[50] = ""){
                  printf("bonjour\n");
                  char *introduce="[SERVER] please introduce yourself by using /nick <your pseudo>\n";
                  write(tabclient[conex-1].name[50], introduce, strlen(introduce));
                }*/
                }
              }



            for (i=1;i<n;i++){
              memset(msg, 0, msg_size);
              if (FD_ISSET(tabclient[i].sockclient, &lecture)!=0){
                int size=readline(tabclient[i].sockclient,msg,msg_size);
                char *nick = "/nick ";
                if (tabclient[i].iden == 0){
                  if (strncmp(msg, nick, strlen(nick)) == 0){
                    tabclient[i].name=read_name(msg,"/nick ");
                    tabclient[i].iden++;
                    printf("Identification of %s\n", tabclient[i].name);
                  }
                  else{
                    printf("Identification failed\n");
                  }
                }
                else{
                  printf("Message received by client %d\n",tabclient[i].sockclient-3);

                  /*bool b=contains("/nick", msg);
                  if (b){

                  }*/
                  //we write back to the client
                  if (strcmp(msg, "quit\n") == 0){
                    //write(client[i], ms, size);
                    int clos=tabclient[i].sockclient-3;
                    close(tabclient[i].sockclient);
                    tabclient[i].sockclient=0;
                    conex=conex-1;
                    printf("Client %d is deconnected\n",clos);
                    if (conex<=1){
                      break;
                    }
                  }
                }



                write(tabclient[i].sockclient, msg, size);
              }
            }
            if (strcmp(msg, "quit\n") == 0){
              printf("Server is closed\n");
              break;
            }



            //add new client's socket


            // From the standart input


          }
        }
      }
    }

      //clean up server socket
      close(sock);
      return 0;
}
