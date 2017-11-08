#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <fcntl.h>
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

// read the message untill \n
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

//get a substring of tab1 when tab1 and tab2 have the same starting string
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

//error message
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//concat two strings
char *concat_string(char *s1,char *s2)
{
     char *s3=NULL;
     s3=(char *)malloc((strlen(s1)+strlen(s2))*sizeof(char));
     strcpy(s3,s1);
     strcat(s3,s2);
     return s3;
   }

//function /who, write on current client socket the connected clients
int send_list( char *msg, int conex, client *tabclient, int msg_size, int cactual){

  int j;
  char *who = "/who\n";
  char *who_name="";

  if (strcmp(msg, who) == 0){  // message starts with /who
    memset(msg, '\0', msg_size);

    for (j=1; j<conex; j++){ // all connected clients
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
    return -1; // not /who
  }
}

//function bonus, returns the list of existing channels
int who_channel( char **tabchannel, int chanel_index, char *msg, int conex, client *tabclient, int msg_size, int cactual){

  int j;
  char *who = "/whochannel\n";
  char *who_name="";

  if (strcmp(msg, who) == 0){
    memset(msg, '\0', msg_size);

    for (j=0; j<chanel_index; j++){
      char *name;
      name=malloc(sizeof(char)*36);
      char *list2 = " \n- ";
      name=concat_string(list2,tabchannel[j]);
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

//function /whois, write on current client socket the information about the right client
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
      if (strcmp(command,"/whois") == 0){ // message starts with /whois


        while (i<nbclients){
          if (strncmp(user, tabclient[i].name, strlen(tabclient[i].name)) == 0){ // right client
            strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&tabclient[i].date)); // get the date and time
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
        if (i == nbclients){ // special case
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

//function /nick, add the name of client in socket if good Identification
void ident(client *tabclient, int cactual, char *msg){
  char *nick = "/nick ";
  if (tabclient[cactual].iden == 0){ // first Identification of the client

    if (strncmp(msg, nick, strlen(nick)) == 0 && strcmp(tabclient[cactual].name,"")==0){
      tabclient[cactual].name=read_name(msg,"/nick ");
      tabclient[cactual].name[strlen(tabclient[cactual].name)-1]='\0';
      tabclient[cactual].iden++;
      printf("Identification of %s\n", tabclient[cactual].name);
    }

    else { // false Identification
      printf("Identification failed\n");
    }
  }
  else {
    if (strncmp(msg, nick, strlen(nick)) == 0 ){ // rename the client name
      tabclient[cactual].name=read_name(msg,"/nick ");
      tabclient[cactual].name[strlen(tabclient[cactual].name)-1]='\0';

    }

  }
}

//function /msgall, write the message to all connected clients (for() in server.c)
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

  if (strncmp(msg, msgall, strlen(msgall)) == 0 ){ // message starts with /msgall
    if (j!=tabclient[i].sockclient){ //doesn't write on the current client


      write(tabclient[i].sockclient, info1, strlen(info1));


    }
    return 0;
        }

  else {
          return -1;
        }


}

//function broadcast of channel
int broadcast2(client *tabclient, int cactual,int i, char *j, char *msg){
  char *msgall = "/msg2all "; //invisible/implicit /msg2all, in order to use same function of broadast
  char *say;
  say=malloc(sizeof(char)*100);
  say=read_name(msg,"/msg2all ");
  char *info="";
  char *info1="";

  info=concat_string("[User ",tabclient[cactual].name);
  info1=concat_string(info,"]");
  info= concat_string(info1," : ");
  info1= concat_string(info,say);

  if (strncmp(msg, msgall, strlen(msgall)) == 0 ){
    //printf("%s\n", tabclient[i].channel);
    if (strcmp(j,tabclient[i].channel)==0){


      write(tabclient[i].sockclient, info1, strlen(info1)); // write to all clients of the channel


    }
    return 0;
        }

  else {
          return -1;
        }


}


//function /msg, write a message to a specific client
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

  sscanf(msg, "%s %s %s", command, client, say); //get several informations



  int k;
  int sock;
  for (k=1;k<conex;k++){ // all connected clients
    if (strcmp(tabclient[k].name,client) == 0 ){ // right client who was asked
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

  if (strcmp(command,"/msg") == 0 ){ // message starts with /msg

      write(sock, info1, strlen(info1));



    return 0;
        }

  else {
          return -1;
        }


}

//function /create, create a channel
int create_chanel(client *tabclient, int cactual,int i, int j, char *msg, char** tabchannel,int chanel_index){
  char *command = "/create ";
  char *name;
  name=malloc(sizeof(char)*36);
  name=read_name(msg,command);
  name[strlen(name)-1]='\0';
    char *info="";
    info=concat_string("You have created channel ",name);

    if (strncmp(msg, command, strlen(command)) == 0 ){ // message starts with /create

      int mis=0;
      int i;
      for (i=0;i<chanel_index;i++){ // all channel names

        if (strcmp(tabchannel[i],name)==0){ //already exists
          mis=1;
        }
      }
        if (mis==1){
          char  *mistake = "This channel already exists";
          write(tabclient[cactual].sockclient, mistake, strlen(mistake));
          return 0;
        }
      else {

      write(tabclient[cactual].sockclient,info,strlen(info)); // write to the client who created the channel
      tabchannel[chanel_index]=name; // add name to tabchannel
      tabclient[cactual].chanel_creator =name; // add information to the structure, needed for quit function

      return 0;
    }}
    else {
      return -1;
    }
  }


// function /quit, go out of the channel
int quit(char **tabchannel,int channel_index, char *msg, char *j2, int actual, int conex, client *tabclient){
    int i;
    char *quit;
    char *name;
    name=malloc(sizeof(char)*36);
    name=read_name(msg,"/msg2all ");//implicit /msg2all
    quit = malloc(sizeof(char)*36);
    quit = concat_string("/quit ", j2);


    if (strncmp(name, quit, strlen(quit)) == 0){ // message starts with /quit

      tabclient[actual].channel = "";
      int l=conex;
      for (i=1; i<conex; i++){ // all connected clients

        if (strcmp(tabclient[i].channel, j2) != 0 ){
          l=l-1;
        }
      }

      if (l==1){ // no more client in the channel


        for(i=0; i<channel_index; i++){ // all channel names
          if (strcmp(tabchannel[i], j2)==0){
            tabchannel[i]=""; //empty the channel name
          }
        }
        int j;
        for (j=1; j<conex; j++){

          if (strcmp(tabclient[j].chanel_creator, j2) == 0){ //looking for the client who created the channel



            tabclient[j].chanel_creator ="";

            char *mistake = "Channel name has been destroyed\n";
            char *mistake2 = "You left the channel\n";
            write(tabclient[actual].sockclient, mistake2, strlen(mistake2));
            write(tabclient[j].sockclient, mistake, strlen(mistake));
      }
    }
  }
  else {
    char *mistake = "You left the channel\n";
    write(tabclient[actual].sockclient, mistake, strlen(mistake));
  }



      return 0;
    }

    else {
      return -1;
    }
  }

//function /join, go in the channel
  int join(client *tabclient, char **tabchannel, int channel_index, char *msg,int actual){
    char *command = "/join ";
    char *nameofchannel;
    nameofchannel=malloc(sizeof(char)*36);
    nameofchannel=read_name(msg,command);
    nameofchannel[strlen(nameofchannel)-1]='\0';
    char *info="";
    info=concat_string("You have joined channel ", nameofchannel);

    int k=1;
    int i;


    if (strncmp(msg, command, strlen(command))==0){ // message starts with /join
      for(i=0; i<channel_index; i++){ //all channel names
        if (strcmp(tabchannel[i], nameofchannel)==0){
          k=0;
        }
      }
      if (k==1){
        char  *mistake = "This channel doesn't exist";
        write(tabclient[actual].sockclient, mistake, strlen(mistake));
        return 0;
      }
      else { //channel exists


        write(tabclient[actual].sockclient,info,strlen(info)); //write to the current client
        tabclient[actual].channel=nameofchannel; //add to the stucture

        return 0;
      }
    }
    else {
      return -1;
    }

  }

//Recieve the file and save it under the same name into the server repertory
void server_send(char *filename, client *tabclient, int i){

  int n=0;
  int file;
  int rcv;
  char *buffer;
  int sizemsg =0;

  file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

  //Read the size of the message
  read(tabclient[i].sockclient, &sizemsg, sizeof(int));

  while (rcv < sizemsg){
    n = read(tabclient[i].sockclient, buffer, sizeof(char)*36);
    rcv = rcv + n;
    write(file, buffer, n);
  }
  close(file);
}

//get the file
void server_accept(char *filename, client *tabclient, int i){

  int n=0;
  int file;
  struct stat infos;
  int sizefile = 0;
  int sizesent = 0;
  char *buffer;

  //Open the file
  file = open(filename, O_RDONLY);

  //size of the file
  fstat(file, &infos);
  sizefile = infos.st_size;

  //Send the file size
  write(tabclient[i].sockclient, &sizefile, sizeof(int));

  while (sizesent < sizefile){
    n = read(file, buffer, sizeof(char)*36);
    sizesent = sizesent + n;
    write(tabclient[i].sockclient, buffer, n);
  }
  close(file);

}

//the client put the file
void client_send(char *filename, int sock){

  int n =0;
  int file;
  struct stat infos;
  int sizefile = 0;
  int sizesent = 0;
  char *buffer;

  //Open the file
  file = open(filename, O_RDONLY);

  //size of the file
  fstat(file, &infos);
  sizefile = infos.st_size;

  //Send the file size
  write (sock, &sizefile, sizeof(int));

  while (sizesent < sizefile){
    n = read(file, buffer, sizeof(char)*36);
    sizesent = sizesent + n;
    write(sock, buffer, n);
  }
  close(file);

}

//the client get the file
void client_accept(char *filename, int sock){

  int n=0;
  int file;
  int rcv;
  char *buffer;
  int sizemsg = 0;

  file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

  //Read the size of the message
  read(sock, &sizemsg, sizeof(int));

  while (rcv < sizemsg){
    n = read(sock, buffer, sizeof(char)*36);
    rcv = rcv + n;
    write(file, buffer, n);
  }
  close(file);

}

int isend(char *msg, client *tabclient, int conex, int i){

  char *send;
  send = malloc(sizeof(char)*36);
  char *user;
  user=malloc(sizeof(char)*36);
  char *filename;
  filename=malloc(sizeof(char)*36);

  sscanf(msg, "%s %s %s", send, user, filename);


  int k;
  int sock;
  for (k=1;k<conex;k++){
    if (strcmp(tabclient[k].name, user) == 0 ){
      sock=tabclient[k].sockclient;
    }
  }

  if (strncmp(send, "/send", strlen("/send")) == 0 ){

      char *ask;
      ask=malloc(sizeof(char)*36);
      ask =  concat_string("[FILE TRANSFERT] ", tabclient[i].name);
      ask = concat_string(ask, " wants you to accept the transfer of the file named ");
      ask = concat_string(ask, filename);
      ask = concat_string(ask, ". Do you accept ? [Y/N]");
      write(sock, ask, strlen(ask));

    return 0;
        }

  else {
          return -1;
        }


}

int answer(char *msg, int sockstock, client *tabclient, int cactual, int index){
  char *info;
  info = malloc(sizeof(char)*36);

  if (strcmp(msg, "Y\n") == 0 || strcmp(msg, "y\n") == 0){
    info = concat_string(tabclient[cactual].name, " accepted the file transfert.\n");
    write(sockstock, info, strlen(info));
    index--;
    return 1;
  }
  else if (strcmp(msg, "N\n") == 0 || strcmp(msg, "n\n") == 0){
    info = concat_string(tabclient[cactual].name, " cancelled the file transfert.\n");
    write(sockstock, info, strlen(info));
    index--;
    return 2;
  }
  else{
    return 0;
    //write(tabclient[cactual].sockclient, "[FILE TRANSFERT] Wrong syntaxe !", strlen("[FILE TRANSFERT] Wrong syntaxe !"));
  }

}
