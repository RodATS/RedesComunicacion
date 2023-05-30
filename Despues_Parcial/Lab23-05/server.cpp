#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
//--------------------------------
using namespace std;

//Puerto que escuchamos
#define PORT "9034"


// get sockaddr, IPv4 o IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//-------------------------------------------------

int main(void)
{
	fd_set master; 		//master file descriptor list, lista de vectores
	fd_set read_fds; 	//temp file descriptor list for select(), leerá los vectores
	int fdmax; 			//max file descriptor number, max vectores, cero al inicio

	int listener; 		//listening socket descriptor
	int newfd;			//newly accepted() socket descriptor
	struct sockaddr_storage remoteaddr; //client address, estructura que indica el puerto del cliente
	socklen_t addrlen; 	//tamaño de la estructura

	char buf[256];	//buffer for client data
	int nbytes; 	//para lectura

	char remoteIP[INET6_ADDRSTRLEN];

	int yes=1;		//for setsockopt() SO_REUSEADDR below
	int i, j, rv;

	struct addrinfo hints, *ai, *p;

	//llamada a funciones------------------------
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	//get us a socket and bind it-------------------
	//conectar la estructura con el socket, el server lo tiene pero diferente.
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0)
	{
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}


	for(p = ai; p != NULL; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
		{
			continue;
		}

		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listener);
			continue;
		}
		break;
	}

	// if we got here, it means we didn´t get bound
	if(p == NULL)
	{
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

	// listen
	if (listen(listener, 10) == -1) 
	{
		perror("listen");
		exit(3);
	}

	// add the listener to the master set
	FD_SET(listener, &master);
	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	
	
	//-----------------------------------------------------------------------------------------------------------------
	// main loop-------
	for(;;)
	{
		
		read_fds = master; // copy it
		//acá espera el programa
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
		{
			perror("select");
			exit(4);
		}
		
		
	//buscar datos existentes
	// run through the existing connections looking for data to read-------------------------------
		//fdmax: socket mayor o más grande  - Los desciprtores se crean de amnera ordenada
		for(i = 0; i <= fdmax; i++) {
			//ver si hay info para leer
			//si la i está dentro del vector
			if (FD_ISSET(i, &read_fds)) 
			{ // we got one!!
				//nueva conexón
				if (i == listener)
				{
					// handle new connections
					addrlen = sizeof remoteaddr;
					//accept: nuevo cliente, lo agregamos al vector maestro
					newfd = accept(listener,(struct sockaddr *)&remoteaddr,	&addrlen);
				
					if (newfd == -1) 
					{
						perror("accept");
					}
					else 
					{
						//accept: nuevo cliente, lo agregamos al vector maestro
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) 
						{ // keep track of the max
							fdmax = newfd;
						}
						printf("selectserver: new connection from %s on ""socket %d\n",	inet_ntop(remoteaddr.ss_family,	get_in_addr((struct sockaddr*)&remoteaddr),	remoteIP, INET6_ADDRSTRLEN), newfd);
					}
				}
//---------------------------------no es nuevo cliente-------------------- ESTO MODIFICAR--------------------------------
				else
				{
					// handle data from a client, manenjar datos
					//nbytes: tamaño del mensaje
					//i= socket, leer buf, leer tamaño determinado del buff 256
					//aca debe leer los 16 Bytes iniciales -> se sabe le tamaño del mensaje -> 10 Bytes finales
					//Opciones
					//0. Crear un string, para almacenar todo el mensaje
					//1. crear char buf_id[9], añade al string, añadir :
					//2. crear char buf_size[7], añade al string añadir : , y se vuelve entero este char
					//3. nbytes = recv(i, buf, el char buff_size en enteero, 0)
					
					//0.
					string mensaje;
					
					//agregar el numero del paquete
					char buff_id[9];
					if ((nbytes = recv(i, buff_id, 9, 0))<= 0) 
					{
						// got error or connection closed by client
						if (nbytes == 0)
						{
							// connection closed
							printf("selectserver: socket %d hung up\n", i);
						} 
						else 
						{
							perror("recv");
						}
						close(i); // bye!
						//sacarlo de la lista, porquw se cierra la conexión
						FD_CLR(i, &master); // remove from master set
					}
					
					
					
					else
					{
						
						//Meterlo al string
						for(int indice_buff = 0; indice_buff < 9; indice_buff++)
						{
							mensaje += buff_id[indice_buff];
						}
						mensaje+= ":";
						
						
						char buff_size[7];
						(nbytes = recv(i, buff_size, 7, 0));
						//Meterlo al string
						string tamaño;
						for(int indice_buff_2 = 0; indice_buff_2 < 9; indice_buff_2++)
						{
							mensaje += buff_size[indice_buff_2];
							tamaño += buff_size[indice_buff_2];
						}
						
						//volverlo int
						int tamaño_mensaje = stoi(tamaño);
						mensaje+= ":";
						
						(nbytes = recv(i, buf, tamaño_mensaje, 0));
						for(int indice_buff_3 = 0; indice_buff_3 < tamaño_mensaje; indice_buff_3++)
						{
							mensaje += buf[indice_buff_3];
						}
						
						mensaje+= ":";
						char final[10];
						//Meterlo al string
						(nbytes = recv(i, final, 10, 0));
						for(int indice_buff_4 = 0; indice_buff_4 < 10; indice_buff_4++)
						{
							mensaje += final[indice_buff_4];
						}
						
						mensaje+= "\n";
						
						//--------------
						//meterlo en un nuevo buffer
						
						int tamaño_mensaje_final= mensaje.length()+1;
						//char *mensaje_final = (char *)malloc( tamaño_mensaje_final * sizeof(char));
    						char *mensaje_final = new char[tamaño_mensaje_final];
						
						for (int indice_j = 0; indice_j < tamaño_mensaje_final - 1; j++) {
							mensaje_final[indice_j] = mensaje[indice_j];
						 }
						 mensaje_final[tamaño_mensaje_final - 1] = '\0';
						
						// we got some data from a client
						for(j = 0; j <= fdmax; j++) 
						{
							// send to everyone!
							if (FD_ISSET(j, &master)) 
							{
								// except the listener and ourselves
								if (j != listener && j != i) 
								{
									//reenvia el mensaje
									//ENVIAR EL NUEVO BUFF
									//(send(j, mensaje_final, sizeof mensaje_final, 0)
									if (send(j, mensaje_final, tamaño_mensaje_final - 1, 0) == -1) 
									{
										perror("send");
									}
								}
							}
						}
					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

	return 0;
}
