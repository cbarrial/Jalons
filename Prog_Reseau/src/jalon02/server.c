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
      printf("Socket %d\n", sock);

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
        printf("Port %d\n", atoi(argv[1]));

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
          int client[20];
          int n=0;

          while (1){

            FD_ZERO(&lecture);
            FD_SET(STDIN_FILENO,&lecture);
            FD_SET(sock,&lecture);

            select(sock+1,&lecture,NULL,NULL,NULL);


            for (int i=0;i < n ;i++ ){
              FD_SET(client[i], &lecture);
            }

            if (FD_ISSET(STDIN_FILENO,&lecture)!=0){
              break;
            }
            else if (FD_ISSET(sock, &lecture)!=0){
                struct sockaddr_in csin;
                socklen_t taille = sizeof(csin);
                int csock;

                //accept connection from client
                csock = accept(sock, (struct sockaddr*)&csin, &taille);
                if (csock == BIND_ERROR){
                  perror("accept");
                }
                else {
                  printf("A client is connecting with the socket %d\n", csock);

                  while(1){

                    //clean msg
                    memset(msg, 0, msg_size);

                    //read what the client has to say
                    int size=readline(csock,msg,msg_size);

                    printf("Message received\n");
                    //we write back to the client

                    write(csock, msg, size);



                  }

                client[n]=csock;
                n++;

            }



        }


      }
      for (int i=0; i<n; i++){
        close(client[i]);
      }

}
}
}



      //clean up server socket
      close(sock);
      return 0;
}
