/*
** selectserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <vector>
#include <fstream>
#include <map>
#include <queue>
#include <mutex>
#include <iostream>
#include <sstream>

#define PORT "9034"   // port we're listening on

fd_set master;    // master file descriptor list
fd_set read_fds;  // temp file descriptor list for select()

std::map<int, std::queue<char *>> mymap;
std::map<int, std::string> myresponse;
int IDS[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool lastLine[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //si el thread esta en una linea partida a la mitaf

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void reader(int i)
{
    int nbytes;
    
    char id[10];
    char sz[8];
    char data[100];
    char chsum[11];

    char buf2[100000];    // buffer for client data
    //ID+sz+chsum+nro de ':'
    int idCounter=0,sizeNoData=9+7+10+3,totalSize;
    int lastSize=0;
    std::string savedLine;
    while(true){
        if(mymap[i].empty())
        continue;
        while (!mymap[i].empty()) {
            //copia
            char *buf3 = strdup(mymap[i].front());
            //vacía el string del socket i
            mymap[i].pop();

            //copiar la info a un objeto de istringstream
            //para poder pasarlo al getline
            std::istringstream Tss(buf3);
            std::string tokenLine;
            while (std::getline(Tss,tokenLine)) {
                //copia la info a savedLine
                savedLine=tokenLine;
                //copia tokenLine a liness que es un objeto del tipo istringstream
                std::istringstream liness(tokenLine);
                std::string tkn;

                //insertar 00001 en idCounterStr
                std::string idCounterStr(std::to_string(IDS[i]));
                idCounterStr.insert(0,5-idCounterStr.length(),'0');
                //std::cout<<idCounterStr;

                //si es que ese socket no esta siendo utlizado
                if(!lastLine[i]){
                    myresponse[i]=myresponse[i]+idCounterStr;
                    IDS[i]++;
                }
                //file<<idCounterStr;

                int counterSZ=0;

                //indicar que el final de cada linea será :
                //000001:00003:283 seria:
                //000001
                //00003
                //283

                //no se xd 
                //..
                while (std::getline(liness,tkn,':')) {
                    if(counterSZ==1){
                        //sizeNoData=9+7+10+3
                        totalSize= sizeNoData + std::stoi(tkn);
                    }
                    //file<<":"<<tkn;
                    if(counterSZ ++== 0 && lastLine[i]==1){
                        myresponse[i]=myresponse[i]+tkn;
                        //counterSZ++;
                        continue;
                    }
                    myresponse[i]=myresponse[i]+":"+tkn;
                    //std::cout<<":"<<tkn;
                } 
                if(totalSize==tokenLine.length()||lastLine[i]==1){
                    //file<<"\n";
                    myresponse[i]=myresponse[i]+"\n";
                    lastLine[i]=0;
                    //std::cout<<"\n";
                }
                else{
                    lastLine[i]=1;
                }
                // hasta aqui no se xd 
                //..
            }
        }
    }
    //std::cout<<myresponse[i]<<std::endl;
   std::cout<<"end of doc\n";
    
}

int main(void)
{
    char buf[66000];    // buffer for client data
                          //
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

	struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

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
    std::vector<std::thread> thrds;

    // main loop
    int ready;
    for(;;) {
        read_fds = master; // copy it
        if ((ready=select(fdmax+1, &read_fds, NULL, NULL, NULL)) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
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
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
                        mymap[newfd].push("");
                        myresponse[newfd]="";
                        thrds.push_back(std::thread(reader,newfd));
                        std::cout<<"\n Nueva connecion\n";
                    }
                } else {
                    int maxR = 10000;
                    int n = read(i, buf, maxR);
                    if(n!=0){
                        buf[n] = '\0';
                        std::cout<<"N:"<<n<<std::endl;
                        mymap[i].push(strdup(buf));
                        while((n=read(i,buf,maxR))==strlen(buf)){
                            buf[n] = '\0';
                            mymap[i].push(strdup(buf));
                            printf("%s\n",buf);
                        }
                        buf[n]='\0';
                        mymap[i].push(strdup(buf));
                    }
                    else{
                        if(n==0){
                        }
                        for(int j=4; j<=fdmax; j++){
                            std::cout<<"Writing files\n";
                            std::fstream file("Files/file"+std::to_string(j)+".txt",std::ios::out);
                            file<<myresponse[j];
                            close(j);
                            FD_CLR(j,&master);
                        }
                    }
                    std::cout<<"Iteration"<<std::endl;
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors

            /*std::cout<<"before join\n";
            for (auto &i: thrds) {
                i.join();
            }
            std::cout<<"after join\n";*/
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}
