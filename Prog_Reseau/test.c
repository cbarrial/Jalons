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
        return i++;
      }
    }
    else {
      break;
    }
  }
  return i;
}

int main(int argc, char const *argv[]) {
  int i=0;
  char *str;
  size_t maxlen=1000;
  int n = readline(i, str, maxlen);
  printf("nombre de caractères : %d\n", n);
  return 0;
}
