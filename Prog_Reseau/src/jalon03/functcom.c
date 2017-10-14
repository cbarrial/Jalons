#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
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

void send_info(char *msg, client *tabclient, int msg_size, int nbclients, int cactual, char *portnb){
      char *whois = "/whois";
      char *user;
      user=malloc(sizeof(char)*36);
      user=read_name(msg,"/whois ");
      user[strlen(user)-1]='\0';
      struct tm date;
      time_t temps;
      time(&temps);
      date=*localtime(&temps);
      strftime(tabclient[cactual].date,100,"%c\n",&date);

      char *command;
      command=malloc(sizeof(char)*36);
        //command=concat_string(command,"\n");
      char *info="";
      char *info1="";

      int i=1;
      sscanf(msg, "%s", command);
      if (strcmp(command,"/whois") == 0){


        while (strncmp(user, tabclient[i].name, strlen(tabclient[i].name)) !=0 ){

          i++;
          if (i>nbclients){
            char  *mistake = "This client doesn't exist";
            write(tabclient[cactual].sockclient, mistake, strlen(mistake));
          }
        }


        memset(msg, '\0', msg_size);
        info = concat_string(user, " connected since ");

          //extraire la date
        info1 = concat_string( info, tabclient[cactual].date);
        info = concat_string( info1, " with IP adress ");
          //extraire l'adresse ip
        info1= concat_string( info, "addip");
        info = concat_string( info1, " and port number ");
          //extraire le port
        info1 = concat_string( info, portnb);

          //printf("et la socket : %d\n", tabclient[cactual].sockclient);

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
