#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int strcmpsi(char* buffer, const char* str) {
  int i = 0;
  while(str[i]) {
    if(buffer[i] != str[i]) {
      return 1;
    }
    i++;
  }
  return 0;
}

// send file to client
void sendfile(int sock, char* filename) {
  FILE *fp;
  fp = fopen(filename,"r");
  fseek(fp,0,SEEK_END);
  int buffersize = ftell(fp);
  fseek(fp,0,SEEK_SET);
  char* buffer = (char*)malloc(buffersize+19);
  char filebuffer[buffersize];
  strcpy(buffer,"HTTP/1.1 200 OK\r\n\r\n");
  fread(filebuffer,buffersize,1,fp);
  strcat(buffer,filebuffer);
  send(sock,buffer,buffersize+19,0);
}


int main() {
  int server_socket, client_socket;
  int port;
  printf("enter port no : ");
  scanf("%d",&port);
  server_socket = socket(AF_INET,SOCK_STREAM,0);
  
  struct sockaddr_in server, client;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  memset(server.sin_zero,0,8);
  unsigned len = sizeof(server);
  
  bind(server_socket,(struct sockaddr*)&server,len);
  
  listen(server_socket,10);
  
  char buffer[4000];
  while(1) {
    memset(buffer,0,4000);
    client_socket = accept(server_socket,(struct sockaddr*)&client, &len);
    int recvb = recv(client_socket,buffer,4000,0);
    if(recvb > 0) {
      if(strcmpsi(buffer,"GET / ") == 0) {
        sendfile(client_socket,"../index.html");
      }
      else if(strcmpsi(buffer,"GET /style.css") == 0) {
        sendfile(client_socket,"../style.css");
      }
      else {
        sendfile(client_socket,"../error.html");
      }
    }
    close(client_socket);
  }
  close(server_socket);
}
  
