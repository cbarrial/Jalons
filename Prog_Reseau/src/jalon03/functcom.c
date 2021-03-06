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
      char *list2 = " - ";
      name=concat_string(list2,tabclient[j].name);
      who_name=concat_string(who_name,name);
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
<<<<<<< HEAD
      strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&tabclient[cactual].date));

      char *command;
      command=malloc(sizeof(char)*36);

=======


      char *command;
      command=malloc(sizeof(char)*36);
>>>>>>> e20876e8e8bbe626c9fc2800aef382b8bfb91a42
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
      tabclient[cactual].iden++;
      printf("Identification of %s\n", tabclient[cactual].name);
    }

    else {
      printf("Identification failed\n");
    }
  }
}
