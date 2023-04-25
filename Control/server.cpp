
 #include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <map>

using namespace std;

map<string, int> users;
map<int, string> sockets;


void thread_read(int socketC) 
{

    int n;
    char buffer[10000];
    
    do{     
           bzero(buffer, 255);
            n = read(socketC, buffer, 1);
            buffer[1] = '\0';

            char protocol = buffer[0];

            switch (protocol)
            {   
                
                
			    
		case 'F':{
			
			char fileName[10000];
			char sizeFileName[5];
			
			//leer un txt
			string nombreTxt = "notas.txt";
			ifstream archivo(nombreTxt.c_str());
			
			string linea;
			
			//Para probar
			// string archivoTxt = "notas.txt000024santisteban 20\npedro 05\n";
			
			string archivoTxt;
			// Obtener línea de archivo, y almacenar contenido en "linea"
			while (getline(archivo, linea)) {
				archivoTxt += linea;
			}
			
			
			for( int i =0; i < archivoTxt.length(); i++){
				fileName[i] = archivoTxt[i];
			}
			fileName[strlen(fileName)] = '\0';
			sprintf(sizeFileName, "%04d", ((int)strlen(fileName)));
			
			
			//enviar al cliente
			write(socketC, "C", strlen("C"));
			//tamaño del mensaje
			write(socketC, sizeFileName, strlen(sizeFileName));
			//mensaje
			write(socketC, fileName, strlen(fileName));
			break;
		}
			    
	  	case 'C':{
			
			cout<<"Confirmacion del cliente de que recibio el txt"<<endl;
			break;
		}
            
                
                
                default:
                    break;
            }
            /* perform read write operations ... */
        } while (strcmp(buffer, "Bye") != 0);
}

int main()
{
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buffer[256];
    char *user, *user2;
    int n;

    if (-1 == SocketFD)
    {
        perror("Can not create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(45000);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if (-1 == bind(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("Error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == listen(SocketFD, 10))
    {
        perror("Error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        int ConnectFD = accept(SocketFD, NULL, NULL);

        if (0 > ConnectFD)
        {
            perror("Error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }

         std::thread (thread_read, ConnectFD).detach();

       
    }

    close(SocketFD);
    return 0;
}
