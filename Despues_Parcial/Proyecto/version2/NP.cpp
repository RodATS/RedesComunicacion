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

#define PORT "9034" // port we're listening on
#define PORT_CLIENT "9036" //puerto para los NA

map<int,  queue<char *>> socket_info; //un buffer para cada socket
map<int,string> files;
map<int, int> transacciones;// cada socket relacionado a una transacción para la devolucion ID del cliente 

void read_pass(int id){

    while(true){

        if(files[id] == "")
        {
        	continue;
        }

        while( files[id] != ""){
            if (send(id, files[id].c_str(),  strlen(files[id].c_str()), 0) == -1) {
                perror("send");
            }

            files[id] = "";
        }
    }
                
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void){
	fd_set master; // master file descriptor list
	fd_set read_fds; // temp file descriptor list for select()
	int fdmax; // maximum file descriptor number
	int listener; // listening socket descriptor
	int identificador;
	
	int newfd; // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;
	char buf[256]; // buffer for client data
	int nbytes;
	char remoteIP[INET6_ADDRSTRLEN];
	int yes=1; // for setsockopt() SO_REUSEADDR, below
	int i, j, rv,transaccionID=0;
	string response;
	struct addrinfo hints, *ai, *p;
	
	FD_ZERO(&master); // clear the master and temp sets
	FD_ZERO(&read_fds);


    //Configuración socket de escucha
	
	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
	fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
	exit(1);
	}
	for(p = ai; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) {
		    continue;
		}
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
		    close(listener);
		    continue;
		}
		break;
	}
	// if we got here, it means we didn't get bound
	if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
	}
	freeaddrinfo(ai); // all done with this
	// listen
	if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
	}
	// add the listener to the master set
	FD_SET(listener, &master);
	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one
    vector<thread> threads_leer;


    /*Configuración socket cliente*/

    int client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == client_socket)
    {
        perror("cannot create client socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(atoi(PORT_CLIENT));
    client_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("bind client socket failed");
        exit(EXIT_FAILURE);
    }
    if (listen(client_socket, 10) == -1)
    {
        perror("listen client socket failed");
        exit(EXIT_FAILURE);
    }
    FD_SET(client_socket, &master); // Añade el socket del cliente al conjunto maestro
    if (client_socket > fdmax)
    {
        fdmax = client_socket;
    }


	// main loop
    for(;;) 
    {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) 
        {
            cout<<"de vuelta\n";
            if (FD_ISSET(i, &read_fds)) { // we got one!!
            if (i == listener) {
                // handle new connections
                addrlen = sizeof remoteaddr;
                newfd = accept(listener,
                (struct sockaddr *)&remoteaddr,
                &addrlen);
                if (newfd == -1) {
                perror("accept");
                } else {
                FD_SET(newfd, &master); // add to master set
                if (newfd > fdmax) { // keep track of the max
                fdmax = newfd;
                }
                printf("selectserver: new connection from %s on "
                "socket %d\n",
                inet_ntop(remoteaddr.ss_family,
                get_in_addr((struct sockaddr*)&remoteaddr),
                remoteIP, INET6_ADDRSTRLEN),
                newfd);
                files[newfd] = ("");
                //threads_leer.push_back(thread(read_pass, newfd));

                }
            }
            else if(i == client_socket){
                // Nuevo cliente conectado al servidor principal
                    addrlen = sizeof remoteaddr;
                    newfd = accept(client_socket, (struct sockaddr*)&remoteaddr, &addrlen);
                    if (newfd == -1)
                    {
                        perror("accept client");
                    }
                    else
                    {
                        FD_SET(newfd, &master); // Añade al conjunto maestro
                        if (newfd > fdmax)
                        {
                            fdmax = newfd;
                        }
                        printf("Servidor principal: nueva conexión de cliente desde %s en el socket %d\n",
                            inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN), newfd);

                        cout<<"No se cierra\n";
                        files[newfd] = ("");
                        cout<<"No se cierra\n";
                    }
            }else {

            // handle data from a client
                // el protocolo cc rc rd uu dd
                if ((nbytes = recv(i, buf, 2 , 0)) <= 0) {
                    // got error or connection closed by client
                    if (nbytes == 0) {
                    // connection closed
                    printf("selectserver: socket %d hung up\n", i);
                    } 
                    else {
                    perror("recv");
                    }
                    close(i); // bye!
                    FD_CLR(i, &master); // remove from master set
                } 
                else {
                // we got some data from a client
                //esto lo metemos en el thread y reenviamos al NA
                    //nbytes = recv(i, buf, 1 , 0);

                    switch(buf[0]){
                        case 'c' : //TODO: case cliente -> r al server
                        {
                            buf[nbytes] = '\0';
                    
                            response = "";
                            /*
                            response += "tt000" + to_string(transaccionID);
                            transacciones[i] = transaccionID; // identificar la transaccion con el cliente correspondiente
                            transaccionID++;
                            */
                            response += buf;
                            nbytes = recv(i, buf, 3 , 0);
                            buf[nbytes] = '\0';
                            response += buf;
                            int tam = atoi(buf); 
                            nbytes = recv(i, buf, tam , 0);
                            buf[nbytes] = '\0';
                            response+=buf;
                            nbytes = recv(i, buf, 3 , 0);
                            buf[nbytes] = '\0';
                            response += buf;
                            tam = atoi(buf); 
                            nbytes = recv(i, buf, tam , 0);
                            buf[nbytes] = '\0';
                            response+=buf;
                            
                            //aca se activa el thread
                            files[i] += response;

                            //cout<<response<<endl;
                            response = "";
                            break;
                        }

                        case 'r' : //TODO: case cliente -> r al server
                        {
                            buf[nbytes] = '\0';
                    
                            response = "";
                            response += "tt000" + to_string(transaccionID);
                            transacciones[i] = transaccionID; // identificar la transaccion con el cliente correspondiente
                            transaccionID++;

                            response += buf;
                            nbytes = recv(i, buf, 3 , 0);
                            buf[nbytes] = '\0';
                            response += buf;
                            int tam = atoi(buf); 
                            nbytes = recv(i, buf, tam , 0);
                            buf[nbytes] = '\0';
                            response+=buf;
                            nbytes = recv(i, buf, 3 , 0);
                            buf[nbytes] = '\0';
                            response += buf;
                            tam = atoi(buf); 
                            nbytes = recv(i, buf, tam , 0);
                            buf[nbytes] = '\0';
                            response+=buf;
                            
                            //aca se activa el thread
                            files[i] += response;

                            //cout<<response<<endl;
                            response = "";
                            break;
                        }


                        case 'u':
                        {
                            buf[nbytes] = '\0';
                    
                            response = "";
                            response += "tt000" + to_string(transaccionID);
                            transacciones[i] = transaccionID; // identificar la transaccion con el cliente correspondiente
                            transaccionID++;

                            response += buf;
                            nbytes = recv(i, buf, 3 , 0);
                            buf[nbytes] = '\0';
                            response += buf;
                            int tam = atoi(buf); 
                            nbytes = recv(i, buf, tam , 0);
                            buf[nbytes] = '\0';
                            response+=buf;
                            //nuevo dato o glosario a actualizar
                            nbytes = recv(i, buf, 3 , 0);
                            buf[nbytes] = '\0';
                            response += buf;
                            tam = atoi(buf); 
                            nbytes = recv(i, buf, tam , 0);
                            buf[nbytes] = '\0';
                            response+=buf;

                            
                            //aca se activa el thread
                            files[i] += response;

                            //cout<<response<<endl;
                            response = "";
                            break;
                        }

                        

                        case 't':
                        {
                            nbytes = recv(i, buf, 4 , 0);
                            buf[nbytes] = '\0';
                            int idTransaccion = atoi(buf); 
                            int socketCliente;
                        
                            
                            if(!(transacciones.find(idTransaccion) == transacciones.end())){
                                auto sock = transacciones[idTransaccion];
                                socketCliente= sock;
                            }else{
                                perror("bad Tid");
                            }

                            nbytes = recv(i, buf, 5 , 0); // tamaño del vector resultante de campos o datos
                            buf[nbytes] = '\0';
                            int tam = atoi(buf); 
                            nbytes = recv(i, buf, tam , 0); //los datos
                            buf[nbytes] = '\0';
                            response+=buf;
                            
                            //aca se activa el thread
                            files[i] += response;

                            send(socketCliente, files[i].c_str(),  strlen(files[i].c_str()), 0);
                            response = "";
                            //cout<<response<<endl;
                        }
                    }
                    
                    


                /* Para después
                string hashMessage = files[i];
                hash<string> myhash;
                auto obj = myhash(hashMessage) % 4 + 4;
                if(send(obj, hashMessage.c_str(),strlen(hashMessage.c_str()),0) == -1){
                    perror("send to NA");
                }
                */
                

                    //segun el hash definir cual es sonnclientes y cuales NA's
                    //int NA, puede disminuir al hacer la reutilizacion= 5 al inicio
                    for(j = 0; j <= fdmax; j++) {
                    // send to everyone
                        if (FD_ISSET(j, &master)) {
                            //except the listener and ourselves
                            if (j != listener && j != i) {
                            if (send(j, files[i].c_str(),  strlen(files[i].c_str()), 0) == -1) {
                            perror("send");
                            }
                            }
                        }
                    }
                
                }
            } // END handle data from client
            cout<<"No se cierra\n";
            } // END got new incoming connection
            cout<<"No se cierra\n";
            
        } // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!
	return 0;
}


/*


int main() {
    std::map<int, std::string> mapa;
    mapa[1] = "Hola";
    mapa[2] = "Mundo";

    // Verificar si una clave existe en el mapa
    if (mapa.count(1) > 0) {
        std::cout << "La clave 1 está presente en el mapa." << std::endl;
    } else {
        std::cout << "La clave 1 no está presente en el mapa." << std::endl;
    }

    if (mapa.count(3) > 0) {
        std::cout << "La clave 3 está presente en el mapa." << std::endl;
    } else {
        std::cout << "La clave 3 no está presente en el mapa." << std::endl;
    }

    return 0;
}



*/
