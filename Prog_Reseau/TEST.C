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
  read(fd, &str, maxlen);
  int i;
  while (str[i]!='\n'){
    i=i++;
  }
  return i;
}
