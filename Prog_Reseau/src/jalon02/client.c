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
    //readline()
    for (;;) {

<<<<<<< HEAD
      FD_ZERO(&fd_set_read);
      FD_SET(sock, &fd_set_read);
      FD_SET(fileno(stdin), &fd_set_read);
      int max_fd = sock+1;

      int sel= select(max_fd, &fd_set_read, NULL, NULL, NULL );
      int i;

      for (i=0; 0<max_fd && sel>0; i++){
        if (FD_ISSET(i, &fd_set_read)){
          if (i == STDIN_FILENO){
            while (1){
              printf("\nEnter your message:\n");
              fflush(stdout);
              readline(0,msg_sent,msg_size);
              write(sock,msg_sent,strlen(msg_sent));
              if (strcmp(msg_sent, "quit\n") == 0)
                exit(1);
              }
            }
            else{
              while (1){
                memset(msg_recv, '\0', msg_size);
                readline(sock,msg_recv,msg_size);
                printf("Message from server:\n");
                fflush(stdout);
                write(1,msg_recv,strlen(msg_recv));
              }
            }
            sel--;
=======
      //send message to the server
      while (1){
        printf("\nEnter your message:\n");
        fflush(stdout);
        readline(0,msg_sent,msg_size);
        write(sock,msg_sent,strlen(msg_sent));
        if (strcmp(msg_sent, "quit\n") == 0)
          exit(1);

        //handle_client_message()
        memset(msg_recv, '\0', msg_size);
        readline(sock,msg_recv,msg_size);
        printf("[Server]: ");
        fflush(stdout);
        write(1,msg_recv,strlen(msg_recv));
        printf("\n");


>>>>>>> b2735058229c4bf904e1545aebdd1106d5ab9ad5
        }
      }
      close(sock);

      getchar();

      return 0;


}
