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


void thread_read(int socketC);

//ejecucion cliente.exe c (w/g) nombre juan
int main(int argc, char* argv[])
{
    string parametro1 (argv[1]); //comando: c r u d
    
    string parametro2 (argv[2]); //word or glosario
    string parametro3 (argv[3]); //campo
    string parametro4 (argv[4]); //word o glosario

    string envioFinal = parametro1 + parametro2 + to_string(parametro3.size()) + parametro3 + to_string(parametro4.size()) + parametro4;

    cout<<parametro1<<" - " << parametro2 << " - "<< parametro3 << " - " << parametro4<<endl;
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

    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
}


/*
void thread_read(int socketC) 
{
    int n;
    char buffer[10000];
    do{
        bzero(buffer, 255);
        n = read(socketC, buffer, 1);
        buffer[1] = '\0';

        if (buffer[0] == 'N') {
            n = read(socketC, buffer, 4);
            buffer[4] = '\0';

            int size_m = atoi(buffer);
            n = read(socketC, buffer, size_m);
            buffer[size_m] = '\0';

            char *msg = (char*)malloc(sizeof(char) * size_m);
            sprintf(msg, "%s", buffer);
            

            n = read(socketC, buffer, 4);
            buffer[4] = '\0';

            int userBuffSize = atoi(buffer);
            n = read(socketC, buffer, userBuffSize);
            buffer[userBuffSize] = '\0';

            cout << endl << buffer << ": " << msg << endl;
        }
        
        if (buffer[0] == 'I'){
            n = read(socketC, buffer, 4);
            buffer[4] = '\0';
            
	    int size_m = atoi(buffer);
            n = read(socketC, buffer, size_m);
            buffer[size_m] = '\0';

            char *msg = (char*)malloc(sizeof(char) * size_m);
            sprintf(msg, "%s", buffer);
            cout<<"Usuario: " << msg << endl;
        }
	    
	
        if (buffer[0] == 'P') {
            n = read(socketC, buffer, 4);
            buffer[4] = '\0';

            int size_m = atoi(buffer);
            n = read(socketC, buffer, size_m);
            buffer[size_m] = '\0';

            char *msg = (char*)malloc(sizeof(char) * size_m);
            sprintf(msg, "%s", buffer);
            

            n = read(socketC, buffer, 4);
            buffer[4] = '\0';

            int userBuffSize = atoi(buffer);
            n = read(socketC, buffer, userBuffSize);
            buffer[userBuffSize] = '\0';

            cout << endl << buffer << ": " << msg << endl;
        }
	
	//Tic Tac Toe--------------------------
    	if (buffer[0] == 'Y'){
		int n_Game;
    		char buffer_Game[10000];
		do{
			if (buffer[0] == 'W') {
			    n = read(socketC, buffer, 4);
			    buffer[4] = '\0';

			    int size_m = atoi(buffer);
			    n = read(socketC, buffer, size_m);
			    buffer[size_m] = '\0';

			    char *msg = (char*)malloc(sizeof(char) * size_m);
			    sprintf(msg, "%s", buffer);


			    n = read(socketC, buffer, 4);
			    buffer[4] = '\0';

			    int userBuffSize = atoi(buffer);
			    n = read(socketC, buffer, userBuffSize);
			    buffer[userBuffSize] = '\0';

			    cout << endl << buffer << ": Coloco su ficha en la pos: " << msg << endl;
			}
		} while(strcmp(buffer,"Win") != 0 || strcmp(buffer,"Lose") != 0)
	}
	
	// Rechazar el juego
        if (buffer[0] == 'N'){
	    n = read(socketC, buffer, 4);
            buffer[4] = '\0';

            int userBuffSize = atoi(buffer);
            n = read(socketC, buffer, userBuffSize);
            buffer[userBuffSize] = '\0';
		
	    cout<<buffer<< " No quiere jugar"<<endl;
	}
        
    } while( true );

}
*/

