#include <sys/types.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <map>
#include <queue>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <functional>

using namespace std;


std::string agregarCeros(int longitud) {
    char userBuffSize[4];
    sprintf(userBuffSize, "%03d", longitud);
    return std::string(userBuffSize);
}

//ejecucion cliente.exe c (w/g) nombre juan
int main(int argc, char* argv[])
{
    string parametro1 (argv[1]); //comando: c r u d
    
    string parametro2 (argv[2]); //word or glosario
    string parametro3 (argv[3]); //campo
    string parametro4 (argv[4]); //word o glosario


    string envioFinal = parametro1 + parametro2 + agregarCeros(parametro3.size()) + parametro3 + agregarCeros(parametro4.size())+ parametro4;

    cout<<"Enviaste: "<<envioFinal<<endl;

    int saberLlego = -1;
    int port;
    port = 9034; // Puerto del cliente
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // Es TCP
    if (-1 == SocketFD)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    stSockAddr.sin_family = AF_INET; // Tipo de comunicación
    stSockAddr.sin_port = htons(port); // Puerto del cliente
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr); // IP

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

    if (-1 == connect(SocketFD, (const struct sockaddr*)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    
    //--------aca--------
    /*
    for(;;)
    {
        
    }
    */
    //-------------------------
    
    send(SocketFD, envioFinal.c_str(), envioFinal.size(),0);

     do{
      

        //n = write(SocketFD,"Hi, this is Julio.",18);
        bzero(buffer,256);
        n = read(SocketFD,buffer,255); //bytes leidos
        printf("Here is the message: [%s]\n",buffer);
        
        saberLlego = 1;
    }
        while(saberLlego != 1);

    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
}

