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

                n = read(socketC, buffer, 4);
                buffer[4] = '\0';

                int size_m = atoi(buffer);
                char csize_m[10000];
                sprintf(csize_m, "%04d", size_m);
                n = read(socketC, buffer, size_m);
                buffer[size_m] = '\0';

                char *msg = (char*)malloc(sizeof(char) * size_m);
                sprintf(msg, "%s", buffer);

                n = read(socketC, buffer, 4);
                buffer[4] = '\0';

                int userBuffSize = atoi(buffer);
                n = read(socketC, buffer, userBuffSize);
                buffer[userBuffSize] = '\0';

                char *cli = (char*)malloc(sizeof(char) * userBuffSize);
                sprintf(cli, "%s", buffer);

                int rec = users[buffer];

                char user2[10000];
                sprintf(user2, "%s", sockets[socketC].c_str());
                char cszuser2[5];
                int szuser2 = strlen(user2);
                sprintf(cszuser2, "%04d", szuser2);

                cout << "Message from " << sockets[socketC] << " to " << cli << ": " << msg << endl;

                write(rec, "N", strlen("N"));
                write(rec, csize_m, strlen(csize_m));
                write(rec, msg, strlen(msg));
                write(rec, cszuser2, strlen(cszuser2));
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
                		const void * msj = ((*it).second).c_str();
                		write(socketC, msj, ((*it).second).length());
                	}
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
