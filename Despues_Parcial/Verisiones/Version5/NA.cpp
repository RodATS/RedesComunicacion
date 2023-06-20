#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <cstring>
#include <thread>
#include <fstream>
#include <sstream>

using namespace std;

void thread_read(int socketC);
void writetoFile(string campo, string data, string file);

int main(int argc, char* argv[])
{
    int port;
    port = (argc > 1) ? std::stoi(argv[1]) : 45000;
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Es TCP
    int n;
    if (-1 == SocketFD)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    stSockAddr.sin_family = AF_INET; //Tipo de comunicación 
    stSockAddr.sin_port = htons(port);//Puerto del servidor
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);//IP

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

    char* buff;
    bool end = false;
    buff = new char[1000];
    int Tid;
    
    for(;;)
    {
        n = recv(SocketFD,buff,4,0);
        buff[4] = '\0';
        Tid = atoi(buff);

        n = recv(SocketFD,buff,2,0);
        buff[2] = '\0';
        switch(buff[1])
        {
            case 'c': //Create
            {
                n = recv(SocketFD,buff,3,0);
                buff[3] = '\0';
                int cSize = atoi(buff);
                char* campo = new char[cSize+1];
                n = recv(SocketFD,campo,cSize,0);
                campo[cSize] = '\0';
                n = recv(SocketFD,buff,3,0);
                buff[3] = '\0';
                int dSize = atoi(buff);
                char* data = new char[dSize+1];
                n = recv(SocketFD,data,dSize,0);
                data[dSize] = '\0';
                writetoFile(string(campo),string(data),"file.txt");
                n = recv(SocketFD,buff,1,0); //leer el '\n'
            }
        }

        printf("Here is the message: [%s]\n",buff);

        
    }
        
    delete buff;
    /* perform read write operations ... */
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
}

void writetoFile(string campo, string data, string file)
{
    ofstream outputfile(file);
    if(!outputfile.is_open())
    {
        cout<<"Error al abrir el file\n";
        return;
    }
    ostringstream oss;
    for(auto x : data)
        oss << x;
    oss<<" ";
    for(auto x : campo)
        oss << x;
    oss<<"\n";
    outputfile<<oss.str();
    outputfile.close();
}

