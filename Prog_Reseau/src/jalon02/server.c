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
          int client[n];
          int csock;
          int conex=0;
          int i;


          for (i=0;i < n ;i++ ){
            client[i]=0;
          }
          client[0]=sock;
          conex=conex+1;

          for (;;){

            FD_ZERO(&lecture);
            FD_SET(sock,&lecture);

            int i;
            int max_sock=sock;
            for (i=0;i<n;i++){
              if (client[i]>0){
                FD_SET(client[i],&lecture);
              }
              if (client[i]>max_sock){
                max_sock=client[i];
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
                client[conex-1]=csock;
                if (conex-1>20){
                  write(client[conex-1], "Server cannot accept incoming connections anymore. Try again later.", sizeof(char)*60);
                  client[conex-1]=0;
                  conex=conex-1;

                }
              }
            }




            for (i=1;i<n;i++){
              memset(msg, 0, msg_size);
              if (FD_ISSET(client[i], &lecture)!=0){
                int size=readline(client[i],msg,msg_size);

                printf("Message received by client %d\n",client[i]-3);
                //we write back to the client
                if (strcmp(msg, "quit\n") == 0){
                  //write(client[i], ms, size);
                  int clos=client[i]-3;
                  close(client[i]);
                  client[i]=0;
                  conex=conex-1;
                  printf("Client %d is deconnected\n",clos);
                  if (conex<=1){
                    break;
                  }
                }

                write(client[i], msg, size);
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
