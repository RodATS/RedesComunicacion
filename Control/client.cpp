 #include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;

void thread_read(int socketC); 
 

int main()
{
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(AF_INET, SOCK_STREAM, 0); 
    int n;
    char buffer[256];

    if (-1 == SocketFD) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(45000);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

    if (0 > Res) {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    } else if (0 == Res) {
        perror("char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    int socketClient = connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
    // Here get connect ID, then pass it to the thread
    if (-1 == socketClient) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
	
    std::thread (thread_read,SocketFD).detach();


//Se manda al server el protocolo para recibir el archivo txt

    std::string test = "F";

    write(SocketFD, test.c_str(), test.size()+1);
	
	
	
	char userBuff[10000];
	char userBuffSize[5];

    for(;;) {
        char receiver[10000];
        char size_m[5];
        char msg[10000];
        
	char type[1];
	fgets(type, 10000, stdin);
	
    }

    
    shutdown(SocketFD, SHUT_RDWR);

    close(SocketFD);
    return 0;
}

void thread_read(int socketC) 
{
    int n;
    char buffer[10000];
    do{
        bzero(buffer, 255);
        n = read(socketC, buffer, 1);
        buffer[1] = '\0';

       
	   
	//Leer el txt
	if (buffer[0] == 'C'){
            n = read(socketC, buffer, 4);
            buffer[4] = '\0';
            
	    int size_m = atoi(buffer);
            n = read(socketC, buffer, size_m);
            buffer[size_m] = '\0';

            char *msg = (char*)malloc(sizeof(char) * size_m);
            sprintf(msg, "%s", buffer);
            cout<< msg << endl;
		
		string nombreArchivo = "archivoRecibido.txt";
    		ofstream archivo;
		archivo << msg;
		archivo.close();
		
		
	string confirmacion = "C";

    	write(socketC, confirmacion.c_str(), confirmacion.size()+1);
	shutdown(socketC, SHUT_RDWR);

   	 close(socketC);	
		
        }
        
    } while(strcmp(buffer,"bye") != 0 );

}
