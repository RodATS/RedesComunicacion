  /* Server code in C */
 
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
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//spcket client
    char buffer[256];
    int n;
    //chat
    string mensajeStringServer, mensajeStringClient;
 
    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("error bind failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if(-1 == listen(SocketFD, 10))
    {
      perror("error listen failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    for(;;)
    {
      int SocketClient = accept(SocketFD, NULL, NULL);
 
      if(0 > SocketClient)
      {
        perror("error accept failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
      
 
 do
     {
     bzero(buffer,256);
     n = read(SocketClient,buffer,255); //bytes leidos
     if (n < 0) perror("ERROR reading from socket");
     printf("Here is the message: [%s]\n",buffer);
     int indice=0;
     mensajeStringClient = "";
     while(buffer[indice] != '\0'){
     	mensajeStringClient += buffer[indice];
     	indice++;
     }
      //scanf or cin
      
     getline(cin,mensajeStringServer);
     int tamMensaje = mensajeStringServer.length();
     //char mensajeChar* = const_cast<char *>(mensajeString.str());
     //n = write(SocketClient,"I got your message",18);
     const void * msj = mensajeStringServer.c_str();
     n = write(SocketClient,msj,tamMensaje);
     if (n < 0) perror("ERROR writing to socket");
}
      while(mensajeStringClient != "bye");
     /* perform read write operations ... */
 
      shutdown(SocketClient, SHUT_RDWR); //se cierra el cliente
 
      close(SocketClient);
    }
 
    close(SocketFD);
    return 0;
  }
