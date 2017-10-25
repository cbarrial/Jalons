#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include "functcom.h"

int do_socket(int domain, int type, int protocol) {
    int sockfd;
    int yes = 1;
//create the socket
    sockfd = socket(domain,type,protocol);

//check for socket validity

// set socket option, to prevent "already in use" issue when rebooting the server right on

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        error("ERROR setting socket options");

    return sockfd;
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

char *read_name(char tab1[],char tab2[]){
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
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


char *concat_string(char *s1,char *s2)
{
     char *s3=NULL;
     s3=(char *)malloc((strlen(s1)+strlen(s2))*sizeof(char));
     strcpy(s3,s1);
     strcat(s3,s2);
     return s3;
   }

int send_list( char *msg, int conex, client *tabclient, int msg_size, int cactual){

  int j;
  char *who = "/who\n";
  char *who_name="";

  if (strcmp(msg, who) == 0){
    memset(msg, '\0', msg_size);

    for (j=1; j<conex; j++){
      char *name;
      name=malloc(sizeof(char)*36);
      char *list2 = " \n- ";
      name=concat_string(list2,tabclient[j].name);
      who_name=concat_string(who_name,name);
      who_name=concat_string(who_name,"\n");

    }

    write(tabclient[cactual].sockclient, who_name, strlen(who_name));
    return 0;
  }
  else {
    return -1;
  }
}

int send_info(char *msg, client *tabclient, int msg_size, int nbclients, int cactual, char *portnb){
      char *whois = "/whois";
      char *user;
      user=malloc(sizeof(char)*36);
      user=read_name(msg,"/whois ");

      char date[20];


      char *command;
      command=malloc(sizeof(char)*36);
      char *info="";
      char *info1="";

      int i=1;
      sscanf(msg, "%s", command);
      if (strcmp(command,"/whois") == 0){


        while (i<nbclients){
          if (strncmp(user, tabclient[i].name, strlen(tabclient[i].name)) == 0){
            strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&tabclient[i].date));
            user[strlen(user)-1]='\0';
            memset(msg, '\0', msg_size);
            info1 = concat_string("[Server] : ", user);
            info = concat_string(info1, " connected since ");

            //extraire la date
            info1 = concat_string( info, date);
            info = concat_string( info1, " with IP adress ");
            memset(date, '\0', 20);
              //extraire l'adresse ip
            info1= concat_string( info, tabclient[i].ip);
            info = concat_string( info1, " and port number ");
              //extraire le port
            info1 = concat_string( info, portnb);

            write(tabclient[cactual].sockclient, info1, strlen(info1));
            return 0;
          }
          else {
            i++;
          }

        }
        if (i == nbclients){
          char  *mistake = "This client doesn't exist";
          write(tabclient[cactual].sockclient, mistake, strlen(mistake));
          return 0;
        }
        return 0;
    }
    else {
      return -1;
    }
  }

void ident(client *tabclient, int cactual, char *msg){
  char *nick = "/nick ";
  if (tabclient[cactual].iden == 0){

    if (strncmp(msg, nick, strlen(nick)) == 0 && strcmp(tabclient[cactual].name,"")==0){
      tabclient[cactual].name=read_name(msg,"/nick ");
      tabclient[cactual].name[strlen(tabclient[cactual].name)-1]='\0';
      tabclient[cactual].iden++;
      printf("Identification of %s\n", tabclient[cactual].name);
    }

    else {
      printf("Identification failed\n");
    }
  }
  else {
    if (strncmp(msg, nick, strlen(nick)) == 0 ){
      tabclient[cactual].name=read_name(msg,"/nick ");
      tabclient[cactual].name[strlen(tabclient[cactual].name)-1]='\0';

    }

  }
}


int broadcast(client *tabclient, int cactual,int i, int j, char *msg){
  char *msgall = "/msgall ";
  char *say;
  say=malloc(sizeof(char)*100);
  say=read_name(msg,"/msgall ");
  char *info="";
  char *info1="";

  info=concat_string("\n[User ",tabclient[cactual].name);
  info1=concat_string(info,"]");
  info= concat_string(info1," : ");
  info1= concat_string(info,say);

  if (strncmp(msg, msgall, strlen(msgall)) == 0 ){
    if (j!=tabclient[i].sockclient){


      write(tabclient[i].sockclient, info1, strlen(info1));


    }
    return 0;
        }

  else {
          return -1;
        }


}


int unicast(client *tabclient, int cactual,int i, int j, char *msg, int conex){
  char *say;
  say=malloc(sizeof(char)*100);
  char *say2;
  say2=malloc(sizeof(char)*100);
  char *command;
  command=malloc(sizeof(char)*36);
  char *client;
  client=malloc(sizeof(char)*36);
  char *info="";
  char *info1="";

  sscanf(msg, "%s %s %s", command, client, say);



  int k;
  int sock;
  for (k=1;k<conex;k++){
    if (strcmp(tabclient[k].name,client) == 0 ){
      sock=tabclient[k].sockclient;
    }
  }

  client=concat_string(client," ");
  say2=read_name(msg,"/msg ");
  say=read_name(say2,client);

  info=concat_string("\n[User ",tabclient[cactual].name);
  info1=concat_string(info,"]");
  info= concat_string(info1," : ");
  info1= concat_string(info,say);

  if (strcmp(command,"/msg") == 0 ){

      write(sock, info1, strlen(info1));



    return 0;
        }

  else {
          return -1;
        }


}


int create_chanel(client *tabclient, int cactual,int i, int j, char *msg, char** tabchannel,int chanel_index){
  char *command = "/create ";
  char *name;
  name=malloc(sizeof(char)*36);
  name=read_name(msg,command);
  name[strlen(name)-1]='\0';
    char *info="";
    info=concat_string("You have created channel ",name);

    if (strncmp(msg, command, strlen(command)) == 0 ){

      int mis=0;
      int i;
      for (i=0;i<chanel_index;i++){

        if (strcmp(tabchannel[i],name)==0){
          mis=1;
        }
      }
        if (mis==1){
          char  *mistake = "This channel already exists";
          write(tabclient[cactual].sockclient, mistake, strlen(mistake));
          return 0;
        }
      else {

      write(tabclient[cactual].sockclient,info,strlen(info));
      tabchannel[chanel_index]=name;

      return 0;
    }}
    else {
      return -1;
    }
  }

  int join(char **tabchannel, int channel_index, char *msg, int i){

    int k;
    char *command = "/join";
    char *nameofchannel = read_name(msg, command);
    nameofchannel[strlen(nameofchannel)-1]='\0';

    //for(i=0; i<channel_index; i++){
      k = strcmp(tabchannel[i], nameofchannel);
      if (k == 0){
        return 0;
      }

    else {
      return -1;
    }

  }
