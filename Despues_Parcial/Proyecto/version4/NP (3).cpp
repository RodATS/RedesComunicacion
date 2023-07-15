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

#define PORT "9034" // Puerto para los NA
#define PORT_CLIENT "9036" //puerto para el cliente

map<int,  queue<char *>> socket_info; //un buffer para cada socket


map<int,string> files;
map<int, int> transacciones;// cada socket relacionado a una transacción para la devolucion ID del cliente
map<int, vector<int>> transaccionNA; //cada NA se relaciona con una transaccion y para la devolucion saber quien tenia la info

hash<string> myhash;
//vector<bool> isUp(10);

map<int, bool> isUp;
int count_word = 0;
int count_glos = 0;

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
	//string response;
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
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) { //Nueva conección
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
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

                        isUp[newfd] = true;

                        files[newfd] = ("");
                        //threads_leer.push_back(thread(read_pass, newfd));

                    }
                }
                else if(i == client_socket){ //Datos de cliente
                        addrlen = sizeof remoteaddr;
                        newfd = accept(client_socket, (struct sockaddr*)&remoteaddr, &addrlen);
                        if (newfd == -1)                   
                            perror("accept client");                 
                        else
                        {
                            FD_SET(newfd, &master); // Añade al conjunto maestro
                            if (newfd > fdmax)
                                fdmax = newfd;
                            printf("Nueva conexión de cliente desde %s en el socket %d\n",
                                inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr),
                                    remoteIP, INET6_ADDRSTRLEN), newfd);

                            //isUp[newfd] = true;      //para que no altere los NA

                        }
                }else {
                    // handle data
                    // el protocolo c r u d (w/g)
                    if ((nbytes = recv(i, buf, 2 , 0)) <= 0) {
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                            isUp[i] = false;
                            cout<<"el isUp de: "<<i<<"se apago "<<endl;
                           // isUp[i] = true;
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

                        //-----------------------CREATE--------------------------------    
                            case 'c' : //TODO: case cliente -> r al server
                            {
                                if(buf[1] == 'w'){
                                    count_word++;
                                }
                                else {
                                    count_glos++;
                                }

                                string hashMessage;
                                buf[nbytes] = '\0';
                        
                                string response;
                                response = "";

                                response += "tt";
                                char userBuffSize[9];
                                sprintf(userBuffSize, "%08d", transaccionID);
                                response += string(userBuffSize);

                                transacciones[transaccionID] = i; // identificar la transaccion con el cliente correspondiente
                                transaccionID++;
                                response += buf;

                                //tamaño campo
                                nbytes = recv(i, buf, 3 , 0);
                                buf[nbytes] = '\0';
                                response += buf;
                                int tam = atoi(buf);

                                //campo 
                                nbytes = recv(i, buf, tam , 0);
                                buf[nbytes] = '\0';
                                hashMessage+=buf;
                                response+=buf;

                                //tamaño word o glosario
                                nbytes = recv(i, buf, 3 , 0);
                                buf[nbytes] = '\0';
                                response += buf;
                                tam = atoi(buf); 

                                //word o glosario
                                nbytes = recv(i, buf, tam , 0);
                                buf[nbytes] = '\0';
                                response+=buf;        

                                //cout<<"Mensaje: " <<response<<endl;

                                files[i] = response;

                                cout<<response<<endl;
                                response = "";
                               
                                /*Definir NA con hash*/                             
                                auto obj = myhash(hashMessage) % 4 + 5;

                                auto redundantObj = obj+1;
                                redundantObj = (redundantObj%4==1)? 5 : redundantObj;

                                //obj = 5;
                                //cout<<"Envia a socket: "<<obj<<endl;
                                if (FD_ISSET(obj, &master)) {                                 
                                    if(send(obj, files[i].c_str(),strlen(files[i].c_str()),0) == -1){
                                        perror("send to NA");
                                    }                                   
                                }
                                //cout<<"Envia a socket redundante: "<<redundantObj<<endl;
                                if (FD_ISSET(redundantObj, &master)) {                                 
                                    if(send(redundantObj, files[i].c_str(),strlen(files[i].c_str()),0) == -1){
                                        perror("send to NA");
                                    }                                   
                                }

                                break;
                            }
                        
                        //---------------------------END----CREATE---------------------
                        
                        //---------------------------------READ---------------------------
                            case 'r' : //TODO: case cliente -> r al server
                            {
                                string hashMessage;
                                cout<<"Me mandaron un read"<<endl;

                                buf[nbytes] = '\0';
                                
                                string response;
                                response = "";
 
                                response += "tr";
                                char userBuffSize[9];
                                sprintf(userBuffSize, "%08d", transaccionID);
                                response += string(userBuffSize);

                                
                                response += buf; //r (w/g) 

                                transacciones[transaccionID] = i;  //cada transaccion se le asigna el valor del ID cliente
                                cout<<"la transaccion: "<<transaccionID<< " guardo el socketCliente: "<<i<<endl;
                               // transacciones[i] = transaccionID; // identificar la transaccion con el cliente correspondiente
                                //transaccionID++; esto esta al final 

                                nbytes = recv(i, buf, 3 , 0); //Csize
                                buf[nbytes] = '\0';
                                response += buf;
                                int tam = atoi(buf); 
                                nbytes = recv(i, buf, tam , 0); //Campo
                                buf[nbytes] = '\0';
                                response+=buf;
                                hashMessage+=buf;

                                
                                //aca se activa el thread
                                

                                cout<<"Esto estoy enviando "<<response<<endl;
                                
                                /*for(j = 5; j < fdmax; j++) {
                                // send to everyone
                                    if (FD_ISSET(j, &master)) {
                                        //except the listener and ourselves
                                        if (j != listener && j != i) {
                                            //cout<<"files["<<i<<"] : "<<files[i]<<endl;
                                            if (send(j, files[i].c_str(),  strlen(files[i].c_str()), 0) == -1) {
                                                perror("send");
                                            }

                                            transaccionNA[transaccionID].push_back(j); //se agrega a la transaccion ID que NA trabajan con el
                                            cout<<"Envia a socket: "<<j<<endl;
                                        }
                                    }
                                }*/

                                auto obj = myhash(hashMessage) % 4 + 5;
                                auto redundantObj = obj+1;
                                redundantObj = (redundantObj%4==1)? 5 : redundantObj;



                                cout<<"Buscar en socket: "<<obj<<endl;

                                if(isUp[obj] == true) cout<<"sigue encendido: "<<obj<<endl;
                                if(isUp[obj] == true){

                                    char buffNAID[2];
                                    sprintf(buffNAID, "%02d", obj);
                                    response += string(buffNAID);
                                    files[i] = response;

                                    if (FD_ISSET(obj, &master)) {
                                    
                                        if(send(obj, files[i].c_str(),strlen(files[i].c_str()),0) == -1){
                                            perror("send to NA");
                                        }
                                    }
                                    
                                }else{
                                    char buffNAID[2];
                                    sprintf(buffNAID, "%02d", redundantObj);
                                    response += string(buffNAID);
                                    files[i] = response;

                                    cout<<"Socket original caído, buscando en respaldo: " <<redundantObj<<'\n';
                                    if (FD_ISSET(redundantObj, &master)) {
                                
                                        if(send(redundantObj, files[i].c_str(),strlen(files[i].c_str()),0) == -1){
                                            perror("send to NA");
                                        }
                                
                                    }
                                }
                                response = "";

                                transaccionID++;

                                break;
                            }

                        //-------------------END--READ---------------------------------------------

                        //--------------------UPDATE-------------------------------------------------
                            case 'u':
                            {
                                cout<<"enviaron un update"<<endl;
                                buf[nbytes] = '\0';

                                string hashMessage = "";
                                
                                string response;
                                response = "";
                                
                                //response += "tt000" + to_string(transaccionID);
                                response += "tt";
                                char userBuffSize[9];
                                sprintf(userBuffSize, "%08d", transaccionID);
                                response += string(userBuffSize);


                                transacciones[transaccionID] = i; // identificar la transaccion con el cliente correspondiente
                                transaccionID++;

                                response += buf;

                                //campo
                                nbytes = recv(i, buf, 3 , 0);
                                buf[nbytes] = '\0';
                                response += buf;
                                int tam = atoi(buf); 
                                nbytes = recv(i, buf, tam , 0);
                                buf[nbytes] = '\0';
                                response+=buf;
                                hashMessage += buf;

                                //dato o glosario antiguo
                                nbytes = recv(i, buf, 3 , 0);
                                buf[nbytes] = '\0';
                                response += buf;
                                tam = atoi(buf); 
                                nbytes = recv(i, buf, tam , 0);
                                buf[nbytes] = '\0';
                                response+=buf;
                                hashMessage += buf;

                                //nuevo dato o glosario a actualizar
                                nbytes = recv(i, buf, 3 , 0);
                                buf[nbytes] = '\0';
                                response += buf;
                                tam = atoi(buf); 
                                nbytes = recv(i, buf, tam , 0);
                                buf[nbytes] = '\0';
                                response+=buf;


                                //aca se activa el thread
                                files[i] = response;

                                
                                cout<<"el NP envia esto: "<< response <<endl;
                                cout<<"el hash recibe: "<<hashMessage <<endl;
                                //cout<<response<<endl;
                                response = "";

                                auto obj = myhash(hashMessage) % 4 + 5;
                                //obj = 5;
                                cout<<"Envia a socket: "<<obj<<endl;
                                if (FD_ISSET(obj, &master)) {
                                    
                                    if(send(obj, files[i].c_str(),strlen(files[i].c_str()),0) == -1){
                                        perror("send to NA");
                                    }
                                    
                                }
                                
                                cout<<"update enviado al NA con exito"<<endl;
                                hashMessage = "";


                                //cout<<response<<endl;
                                response = "";
                                break;
                            }

                        //----------------------------END-UPDATE---------------------------------------

                        //----------------------------DELETE-------------------------------------------               
                            case 'd'://delete
                            {

                                cout<<"Enviaron un delete"<<endl;
                                string hashMessage = "";
                                buf[nbytes] = '\0';
                                
                                string response;
                                response = "";
                                
                                //response += "tt000" + to_string(transaccionID);
                                response += "tt";
                                char userBuffSize[9];
                                sprintf(userBuffSize, "%08d", transaccionID);
                                response += string(userBuffSize);


                                transacciones[transaccionID] = i; // identificar la transaccion con el cliente correspondiente
                                transaccionID++;
                                response += buf;

                                //SizeCampo
                                nbytes = recv(i, buf, 3 , 0);
                                buf[nbytes] = '\0';
                                response += buf;
                                int tam = atoi(buf); 

                                //Campo
                                nbytes = recv(i, buf, tam , 0);
                                buf[nbytes] = '\0';
                                hashMessage += buf;
                                response+=buf;

                                //SiezeDato
                                nbytes = recv(i, buf, 3 , 0);
                                buf[nbytes] = '\0';
                                response += buf;
                                tam = atoi(buf); 

                                //Dato
                                nbytes = recv(i, buf, tam , 0);
                                buf[nbytes] = '\0';                                                             
                                response+=buf;
                             
                                //aca se activa el thread
                                files[i] = response;

                                cout<<"el NP envia esto: "<< response <<endl;
                                cout<<"el hash recibe: "<<hashMessage <<endl;
                                //cout<<response<<endl;
                                response = "";

                                auto obj = myhash(hashMessage) % 4 + 5;
                                //obj = 5;
                                cout<<"Envia a socket: "<<obj<<endl;
                                if (FD_ISSET(obj, &master)) {
                                    
                                    if(send(obj, files[i].c_str(),strlen(files[i].c_str()),0) == -1){
                                        perror("send to NA");
                                    }
                                    
                                }
                                
                                cout<<"delete enviado al NA con exito"<<endl;
                                hashMessage = "";
                                break;
                            }

                        //---------------------------END--DELETE-----------------------------------


                        //------------------------DEVOLVER CONTADOR----------------------------------
                            case 'n':{
                            
                            switch (buf[1]){
                                case 'w':
                                {
                                    string response = "cantidad words: ";

                                    response += to_string(count_word);

                                    files[i] = response;

                                    send(i, files[i].c_str(),  strlen(files[i].c_str()), 0);
                                    break;
                                }
                                
                                case 'g':
                                {
                                    string response = "cantidad glos: ";

                                    response += to_string(count_glos);

                                    files[i] = response;

                                    send(i, files[i].c_str(),  strlen(files[i].c_str()), 0);
                                    break;
                                }

                            }
                            
                        }


                        //-------------------------------END DEVOLVER CONTADOR-----------------------
                        
                        //--------------------------DEVOLUCION DEL NA--------------------------------
                        //-----------------------CONFIRMACION PARA ENVIAR AL CLIENT---------------------
                            case 't':
                            {
                                
                                switch(buf[1]){
                                    //saber si es un c u d
                                    case 't':{
                                        
                                        string response;
                                        //cout<<"hay una repsuesta"<<endl;
                                        nbytes = recv(i, buf, 8 , 0);
                                        buf[nbytes] = '\0';
                                        //cout<<"["<<buf<<"]"<<endl;
                                        int idTransaccion = atoi(buf); 
                                        int socketCliente;
                                    
                                        if(transacciones.find(idTransaccion) != transacciones.end()){
                                            auto sock = transacciones[idTransaccion];
                                            socketCliente= sock;
                                        }else{
                                            cout<<"Tid no encontrado\n";
                                        }

                                        nbytes = recv(i, buf, 4, 0); // tamaño del vector resultante de campos o datos
                                        buf[nbytes] = '\0';
                                        //cout<<"["<<buf<<"]"<<endl;
                                        //response += buf; //no es necesario son mensajes predeterminados
                                        
                                        int tam = atoi(buf); 

                                        //en que momento agrega el tamaño??
                                        nbytes = recv(i, buf, tam , 0); //los datos
                                        buf[nbytes] = '\0';
                                        //cout<<"["<<buf<<"]"<<endl;
                                        response+=buf;
                                        
                                        //aca se activa el thread
                                        files[i] = response;

                                        //cout<<"envia al cliente"<<endl;
                                        send(socketCliente, files[i].c_str(),  strlen(files[i].c_str()), 0);
                                        response = "";
                                        //cout<<response<<endl;
                                        break;
                                    }

                                    //saber si es una respuesta del r esto esta en el switch r
                                    case 'r':
                                    {
                                        string response;
                                        response +="r";
                                        cout<<"hay una repsuesta del read"<<endl;
                                        nbytes = recv(i, buf, 8 , 0);
                                        buf[nbytes] = '\0';
                                        cout<<"["<<buf<<"]"<<endl;
                                        int idTransaccion = atoi(buf); 
                                        cout<<"el id Transaccion: "<<idTransaccion<<endl;
                                        int socketCliente;
                                        //vector<int> socketNA;
                                    
                                        
                                        if(transacciones.find(idTransaccion) != transacciones.end()){
                                            int sock = transacciones[idTransaccion];
                                            //auto sockNA = transaccionNA[idTransaccion]; //saber en que NA estaba la info
                                            socketCliente= sock;
                                            //socketNA = sockNA;
                                        }else{
                                            cout<<"Tid no encontrado\n";
                                        }




                                        nbytes = recv(i, buf, 4, 0); // tamaño del vector resultante de campos o datos
                                        buf[nbytes] = '\0';
                                        cout<<"["<<buf<<"]"<<endl;
                                        response += buf;
                                        int tam = atoi(buf); 

                                        nbytes = recv(i, buf, tam , 0); //los datos
                                        buf[nbytes] = '\0';
                                        cout<<"["<<buf<<"]"<<endl;
                                        response+=buf;

                                        nbytes = recv(i, buf, 2, 0); //id del socket
                                        buf[nbytes] = '\0';
                                        response+=buf;
                                        
                                        //agrega el NA donde estuvo la info
                                        //response += to_string(socketNA);
                                        //aca se activa el thread
                                        files[i] = response;
                                        cout<<files[i]<<endl;

                                        //cout<<"envia al cliente"<<endl;
                                        send(socketCliente, files[i].c_str(),  strlen(files[i].c_str()), 0);
                                        response = "";
                                        cout<<response<<endl;
                                        break;
                                    }
                                }
                                
                            }

                            //----------------END----TRANSACCION EXITOSA-----------------------

                        }

                        //---------END DEL SWITCH DE PROTOCOLOS-----------------------------
                            
                        //segun el hash definir cuales son clientes y cuales NA's
                        //int NA, puede disminuir al hacer la reutilizacion= 5 al inicio
                        //el ultimo siempre será el cliente

                        /*for(j = 5; j < fdmax; j++) {
                            
                        // send to everyone
                            if (FD_ISSET(j, &master)) {
                                //except the listener and ourselves
                                if (j != listener && j != i) {
                                    //cout<<"files["<<i<<"] : "<<files[i]<<endl;
                                    if (send(j, files[i].c_str(),  strlen(files[i].c_str()), 0) == -1) {
                                        perror("send");
                                    }
                                    cout<<"Envia a socket: "<<j<<endl;
                                }
                            }
                            //close(j);
                            //FD_CLR(j,&master);
                        }*/

                    }
                } // END handle data from client
            
            } // END got new incoming connection

            
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