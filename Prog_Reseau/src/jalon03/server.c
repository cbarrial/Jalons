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
#include "functcom.h"

typedef struct {
  int sockclient;
  int iden ;
  char *name;
  int ip;

}client;

/*char *read_name(char tab1[],char tab2[]){
  int j=0;
  int i=0;
  char *msg;
  msg=malloc(sizeof(char)*36);
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
}*/

char *concat_string(char *s1,char *s2)
{
     char *s3=NULL;
     s3=(char *)malloc((strlen(s1)+strlen(s2))*sizeof(char));
     strcpy(s3,s1);
     strcat(s3,s2);
     return s3;
     }

void send_list( char *msg, int conex, client *tabclient, int msg_size, int cactual){
       int j;
       char *who = "/who\n";
       char *who_name="";
       if (strcmp(msg, who) == 0){
         memset(msg, '\0', msg_size);

       for (j=1; j<conex; j++){
         char *name;
         name=malloc(sizeof(char)*36);
         char *list2 = " - ";
         name=concat_string(list2,tabclient[j].name);
         who_name=concat_string(who_name,name);
       }
     write(tabclient[cactual].sockclient, who_name, strlen(who_name));
     }
   }


void send_info(char *msg, client *tabclient, int msg_size, int nbclients, int cactual, char *port){
  char *whois = "/whois ";
  char *user;
  user=malloc(sizeof(char)*36);
  char *command;
  command=malloc(sizeof(char)*36);
  //command=concat_string(command,"\n");
  char *info="";
  char *info1="";

  int i=0;
  sscanf(msg, "%s %s", command, user);

  if (strncmp(command, whois, strlen(whois)-1) == 0){
    /*while (strncmp(msg, concat_string("/whois ", tabclient[i].name), strlen(concat_string("/whois ", tabclient[i].name))) !=0 ){
      i++;
      if (i>nbclients){
        char  *mistake = "This client doesn't exist";
        write(tabclient[cactual].sockclient, mistake, strlen(mistake));
      }
    }*/

    //sscanf(msg, "%s %s", command, user);
    memset(msg, '\0', msg_size);

    info = concat_string(user, " connected since ");

    //extraire la date
    info1 = concat_string( info, "date");

    info = concat_string( info1, " with IP adress ");
    //extraire l'adresse ip

    info1= concat_string( info, "addip");

    info = concat_string( info1, " and port number ");
    //extraire le port

    info1 = concat_string( info, port);


    write(tabclient[cactual].sockclient, info1, strlen(info1));

}
}


void ident(client *tabclient, int cactual, char *msg){
  char *nick = "/nick ";
  if (tabclient[cactual].iden == 0){

    if (strncmp(msg, nick, strlen(nick)) == 0 && strcmp(tabclient[cactual].name,"")==0){
      tabclient[cactual].name=read_name(msg,"/nick ");
      tabclient[cactual].iden++;
      printf("Identification of %s\n", tabclient[cactual].name);
    }

    else {
      printf("Identification failed\n");
    }
  }
}

/*ssize_t readline(int fd, char str[], size_t maxlen){
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
}*/

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
    char *port=argv[1];

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
            tabclient[i].iden = 0;
            tabclient[i].name="";
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




                /*char *nick = "/nick ";
                if (tabclient[i].iden == 0){

                  if (strncmp(msg, nick, strlen(nick)) == 0 && strcmp(tabclient[i].name,"")==0){
                    tabclient[i].name=read_name(msg,"/nick ");
                    tabclient[i].iden++;
                    printf("Identification of %s\n", tabclient[i].name);
                  }

                  else{
                    printf("Identification failed\n");
                  }
                }*/

                ident(tabclient, i, msg);



                //else {



                  /*
                  char *who = "/who";
                  if (strncmp(msg, who, strlen(who)) == 0){
                    memset(msg, '\0', msg_size);
                    int j;
                    char *who_name="";

                    for (j=1; j<conex; j++){
                      char *name;
                      name=malloc(sizeof(char)*36);
                      char *list2 = " - ";
                      name=concat_string(list2,tabclient[j].name);
                      who_name=concat_string(who_name,name);
                    }
<<<<<<< HEAD
                  }

                  printf("Message received by %s\n",tabclient[i].name);






                  write(tabclient[i].sockclient, who_name, strlen(who_name));
                }*/

                  send_list(msg, conex, tabclient, msg_size, i);

                  send_info(msg, tabclient, msg_size, n, i, argv[1]);

                  printf("Message received by client %s\n",tabclient[i].name);

                  //we write back to the client
                  if (strcmp(msg, "quit\n") == 0){
                    //write(client[i], ms, size);
                    //memset(msg, '\0', msg_size); si memset pas de server closed
                    int clos=tabclient[i].sockclient-3;
                    close(tabclient[i].sockclient);
                    tabclient[i].sockclient=0;
                    conex=conex-1;
                    printf("Client %d is deconnected\n",clos);
                    if (conex<=1){
                      break;
                    }
                  }




                write(tabclient[i].sockclient, msg, size);
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
