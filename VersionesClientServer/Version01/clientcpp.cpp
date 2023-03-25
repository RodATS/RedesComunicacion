 /* Client code in C */
 
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <iostream>
  #include <string>
  using namespace std;
 
  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
    char buffer[256];
    
    //chat
    string mensajeStringServer, mensajeStringClient;
    int tamMensaje;

 
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in)); //la estructura de ese socket
 
    stSockAddr.sin_family = AF_INET; //conectarse a dif maquinas
    stSockAddr.sin_port = htons(1100);
    //Res = inet_pton(AF_INET, "192.168.1.33", &stSockAddr.sin_addr);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
 
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
   do{
      
     getline(cin,mensajeStringClient);
     tamMensaje = mensajeStringClient.length();
     //char mensajeCharClient* = const_cast<char *>(mensajeString.str());
     //n = write(SocketFD,"I got your message",18);
     const void * msj = mensajeStringClient.c_str();
     n = write(SocketFD,msj,tamMensaje);

    //n = write(SocketFD,"Hi, this is Julio.",18);
    bzero(buffer,256);
     n = read(SocketFD,buffer,255); //bytes leidos
     printf("Here is the message: [%s]\n",buffer);
     int indice=0;
	     mensajeStringServer = "";
	     while(buffer[indice] != '\0'){
	     	mensajeStringServer += buffer[indice];
	     	indice++;
	}
   }
      while(mensajeStringClient != "bye");

    /* perform read write operations ... */
 
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);
    return 0;
  }
