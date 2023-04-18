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


/*
3 en raya
_1_|_2_|_3_
_4_|_5_|_6_
_7_|_8_|_9_
*/




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
                
                case 'L': { //Login
                n = read(socketC, buffer, 4);
                buffer[4] = '\0';
                //cout<<buffer<<endl;
                int userBuffSize = atoi(buffer);
                //cout<<userBuffSize<<endl;
                n = read(socketC, buffer, userBuffSize+1);
                buffer[userBuffSize - 1] = '\0';
                
                cout << "Login: " << buffer << endl;
                users.insert({buffer, socketC});
                sockets.insert({socketC, buffer});
                break;
            
            	} 
            
            	case 'N': { //Message to other user
		
		//Tamaño del mensaje que se recibio
                n = read(socketC, buffer, 4);
                buffer[4] = '\0';

		//se lee el mensaje
                int size_m = atoi(buffer);
                char csize_m[10000];
                sprintf(csize_m, "%04d", size_m);
                n = read(socketC, buffer, size_m);
                buffer[size_m] = '\0';

                char *msg = (char*)malloc(sizeof(char) * size_m);
                sprintf(msg, "%s", buffer);
                
                
		//tamaño del nombre del usuario
                n = read(socketC, buffer, 4);
                buffer[4] = '\0';
		
		//nombre del usuario
                int userBuffSize = atoi(buffer);
                n = read(socketC, buffer, userBuffSize);
                buffer[userBuffSize] = '\0';

                char *cli = (char*)malloc(sizeof(char) * userBuffSize);
                sprintf(cli, "%s", buffer);
		
		//el socket del cliente a quien se le mandara el mensaje
                int rec = users[buffer];

                char user2[10000];
                sprintf(user2, "%s", sockets[socketC].c_str());
                char cszuser2[5];
                int szuser2 = strlen(user2);
                sprintf(cszuser2, "%04d", szuser2);

                cout << "Message from " << sockets[socketC] << " to " << cli << ": " << msg << endl;

                write(rec, "N", strlen("N"));
                //tamaño del mensaje
                write(rec, csize_m, strlen(csize_m));
                //mensaje
                write(rec, msg, strlen(msg));
                //tamaño del nombre del usuario que mando el mensaje
                write(rec, cszuser2, strlen(cszuser2));
                //nombre usuario
                write(rec, user2, strlen(user2));

                cout << "Message sent." << endl;
                break;
                }
                
                
                //Log out
                case 'O':{
                	cout << "LogOut " << sockets[socketC]<<endl;
                	users.erase({buffer});
                	sockets.erase({socketC});
                	break;
                }
                
                //Listtt
 		case 'I':{
                	cout << "Users List"<<endl;
                	map<int, string>::iterator it;
                	for(it = sockets.begin(); it != sockets.end(); it++){
                		const void * nombre = ((*it).second).c_str();
                		char csize_nombre[5];
				int szuser = ((*it).second).length();
				sprintf(csize_nombre, "%04d", szuser);
				
				write(socketC, "I", strlen("I"));
                		write(socketC, csize_nombre, strlen(csize_nombre));
                		write(socketC, nombre, ((*it).second).length());
                	}
                	break;
                }
                
	       //Vamo a jugar?
 		case 'P':{ //Falta a modificar
                	//Tamaño del mensaje que se recibio
			n = read(socketC, buffer, 4);
			buffer[4] = '\0';

			//se lee el mensaje
			int size_m = atoi(buffer);
			char csize_m[10000];
			sprintf(csize_m, "%04d", size_m);
			n = read(socketC, buffer, size_m);
			buffer[size_m] = '\0';

			char *msg = (char*)malloc(sizeof(char) * size_m);
			sprintf(msg, "%s", buffer);


			//tamaño del nombre del usuario
			n = read(socketC, buffer, 4);
			buffer[4] = '\0';

			//nombre del usuario
			int userBuffSize = atoi(buffer);
			n = read(socketC, buffer, userBuffSize);
			buffer[userBuffSize] = '\0';

			char *cli = (char*)malloc(sizeof(char) * userBuffSize);
			sprintf(cli, "%s", buffer);

			//el socket del cliente a quien se le mandara el mensaje
			int rec = users[buffer];

			char user2[10000];
			sprintf(user2, "%s", sockets[socketC].c_str());
			char cszuser2[5];
			int szuser2 = strlen(user2);
			sprintf(cszuser2, "%04d", szuser2);

			cout << "Invitacion from " << sockets[socketC] << " to " << cli << ": " << msg << endl;

			write(rec, "N", strlen("N"));
			//tamaño del mensaje
			write(rec, csize_m, strlen(csize_m));
			//mensaje
			write(rec, msg, strlen(msg));
			//tamaño del nombre del usuario que mando el mensaje
			write(rec, cszuser2, strlen(cszuser2));
			//nombre usuario
			write(rec, user2, strlen(user2));

			cout << "Invitacion al juego sent." << endl;
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
