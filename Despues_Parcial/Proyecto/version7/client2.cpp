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
#include <sstream>
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

    //para el caso del update
    string parametro5 (argv[5]); // nuevo word o glosario

    int n;
    char buffer[10000];
    int saberLlego = -1;
    int saberLlegoRead = 0;
    int port;
    port = 9036; // Puerto del cliente
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

    


    //-------------------------
    if(string(argv[1]) == "c"){
        string file(argv[5]);
        ifstream inputfile(file);
        if (!inputfile.is_open()) {
        cout << "No se pudo abrir el archivo \n";}
        string line;
        while(getline(inputfile,line)){
            stringstream ss(line);
            string value1, value2;
            getline(ss, value1, ',');
            getline(ss, value2);
            string envioFinal = parametro1 + parametro2 + agregarCeros(value1.size()) +
                                value1 + agregarCeros(value2.size()) + value2;
            send(SocketFD, envioFinal.c_str(), envioFinal.size(),0);
            do{
                bzero(buffer,10000);
                n = recv(SocketFD,buffer,17,0); //bytes leidos
                //printf("%s\n",buffer);
                //n = recv(SocketFD,buffer,17,0); //bytes leidos
                //printf("%s\n",buffer);
                saberLlego = 1;
            }
            while(saberLlego != 1);
            //cout<<"Enviaste: "<<envioFinal<<endl;
        }
        inputfile.close();
    }
    else if(string(argv[1])=="r"){
        
        string envioFinal = parametro1 + parametro2 + agregarCeros(parametro3.size()) + parametro3;
        
        send(SocketFD, envioFinal.c_str(), envioFinal.size(),0);
        //cout<<"Enviaste: "<<envioFinal<<endl;
        do{
            
            n = recv(SocketFD,buffer,1,0);
            buffer[n] = '\0';
            //cout<<"["<<buffer<<"]\n";
            if(buffer[0] == 'r') cout<<"Recibiendo respuestas...\n";
            n = recv(SocketFD, buffer, 4, 0);
            buffer[n] = '\0';
            //cout<<"["<<buffer<<"]\n";
            int size = atoi(buffer);
            n = recv(SocketFD, buffer, size, 0);
            buffer[n] = '\0';
            //cout<<"["<<buffer<<"]\n";
            printf("Respuesta: [%s]",buffer);

            n = recv(SocketFD, buffer, 2, 0);
            buffer[2] = '\0';
            //cout<<"["<<buffer<<"]\n";
            printf(" del NA : %s\n",buffer);


            saberLlego = 1;
        }
        while(saberLlego != 1);
        
    }
    else if(string(argv[1])=="d"){
        string envioFinal = parametro1 + parametro2 + agregarCeros(parametro3.size()) + parametro3 + agregarCeros(parametro4.size())+ parametro4;
        cout<<"Enviaste: "<<envioFinal<<endl;
        
        send(SocketFD, envioFinal.c_str(), envioFinal.size(),0);
        do{
            //n = write(SocketFD,"Hi, this is Julio.",18);
            bzero(buffer,10000);
            n = recv(SocketFD,buffer,17,0); //bytes leidos
            printf("Here is the message: [%s]\n",buffer);
            
            saberLlego = 1;
        }
        while(saberLlego != 1);
    }
    //update
   else if(string(argv[1])=="u"){

        //string parametro5="Nuevo valor";  
        string envioFinal = parametro1 + parametro2+ agregarCeros(parametro3.size())+ parametro3 + agregarCeros(parametro4.size()) + parametro4 + agregarCeros(parametro5.size()) + parametro5  ;
        cout<<"Enviaste: "<<envioFinal<<endl;
        send(SocketFD, envioFinal.c_str(),envioFinal.size(),0);
        do{
            bzero(buffer,10000);
            n=recv(SocketFD, buffer,17,0);
            printf("Here is the message: [%s]\n", buffer);
            saberLlego=1;
          
    }  while(saberLlego!=1);

   }

   else if(string(argv[1])=="n"){

        //string parametro5="Nuevo valor";  
        string envioFinal = parametro1 + parametro2 ;
        cout<<"Enviaste: "<<envioFinal<<endl;
        send(SocketFD, envioFinal.c_str(),envioFinal.size(),0);
        do{
            bzero(buffer,10000);
            n=recv(SocketFD, buffer,20,0);
            printf("Here is the message: [%s]\n", buffer);
            saberLlego=1;
          
    }  while(saberLlego!=1);

   }


    
    

        //--------aca--------
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
}

