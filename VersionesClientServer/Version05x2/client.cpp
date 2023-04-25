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

    cout << "Log in: ";
    char userBuff[10000];
    fgets(userBuff, 10000, stdin);

    char userBuffSize[5];
    sprintf(userBuffSize, "%04d", ((int)strlen(userBuff)));

    //cout<<"L"<<userBuffSize<<userBuff;

    std::string test = "L" + std::string(userBuffSize) + std::string(userBuff);

    write(SocketFD, test.c_str(), test.size()+1);

	//
	//string menuInicial = "Log Out (O) - Message (N) - List (I) - Invitacion Tic Tac Toe (P): ";
	
	string menuInicial = "Log Out (O) - Message (N) - List (I) - Invitacion Tic Tac Toe (P) - Aceptar o Rechazar el juego (Y/X) - Movimiento juego (W): ";
	
	string menuInicialModoJuego = "Log Out (O) - Message (N) - List (I) - Movimiento juego (W): ";
	
	string menu = menuInicial;

    for(;;) {
        char receiver[10000];
        char size_m[5];
        char msg[10000];
        
	cout<<menu;
	char type[1];
	fgets(type, 10000, stdin);
	//cin>>type;
	switch (type[0]){
		case 'N':{
			cout << "Send to:";
			fgets(receiver, 10000, stdin);
			receiver[strlen(receiver) - 1] = '\0';
			
			
			printf("Message: ");

			fgets(msg, 10000, stdin);
			msg[strlen(msg) - 1] = '\0';
			sprintf(size_m, "%04d", ((int)strlen(msg)));
			sprintf(userBuffSize, "%04d", ((int)strlen(receiver)));
			

			string temp = "N" +  string(size_m) + string(msg) + string(userBuffSize) + string(receiver);
			//cout<<temp<<endl;
			write(SocketFD, temp.c_str(), temp.size()+1);
			break;
        	}
        	
        	case 'O':{
			string temp = "O";
			//cout<<temp<<endl;
			write(SocketFD, temp.c_str(), temp.size()+1);
			shutdown(SocketFD, SHUT_RDWR);

	    		close(SocketFD);
	    		return 0;
        	}
        	
        	case 'I':{
        		string temp = "I";
			//cout<<temp<<endl;
			write(SocketFD, temp.c_str(), temp.size()+1);
			break;
        	}
			
		case 'P':{
			cout << "Send to:";
			fgets(receiver, 10000, stdin);
			receiver[strlen(receiver) - 1] = '\0';
			
			string pregJugar = "Jugamos TicTacToe: (Y/N)";
			for( int i =0; i < pregJugar.length(); i++){
				msg[i] = pregJugar[i];
			}
			
			msg[strlen(msg) - 1] = '\0';
			sprintf(size_m, "%04d", ((int)strlen(msg)));
			sprintf(userBuffSize, "%04d", ((int)strlen(receiver)));

			string temp = "P" +  string(size_m) + string(msg) + string(userBuffSize) + string(receiver);
			//cout<<temp<<endl;
			write(SocketFD, temp.c_str(), temp.size()+1);
			break;
        	}
			
		case 'Y':{
			cout << "Send to (accept game):";
			fgets(receiver, 10000, stdin);
			receiver[strlen(receiver) - 1] = '\0';
			

			string temp = "Y" + string(userBuffSize) + string(receiver);
			//cout<<temp<<endl;
			write(SocketFD, temp.c_str(), temp.size()+1);
			break;
		}
		
		case 'X':{
			cout << "Send to (reject game):";
			fgets(receiver, 10000, stdin);
			receiver[strlen(receiver) - 1] = '\0';
			

			string temp = "X" + string(userBuffSize) + string(receiver);
			//cout<<temp<<endl;
			write(SocketFD, temp.c_str(), temp.size()+1);
			break;
		}
        	
        }
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
	    
	//Pregunta Invitacion del juego---
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
            
            //menu = menuInicialModoRespuesta;
        }
	
	//Tic Tac Toe--------------------------
    	if (buffer[0] == 'Y'){
		n = read(socketC, buffer, 4);
		buffer[4] = '\0';

		int userBuffSize = atoi(buffer);
		n = read(socketC, buffer, userBuffSize);
		buffer[userBuffSize] = '\0';

		cout<<buffer<< " Si quiere jugar"<<endl;
		
		//menu = menuInicialModoJuego;
	}
	
	    //----Movimiento en le tablero
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
	
	// Rechazar el juego
        if (buffer[0] == 'N'){
	    n = read(socketC, buffer, 4);
            buffer[4] = '\0';

            int userBuffSize = atoi(buffer);
            n = read(socketC, buffer, userBuffSize);
            buffer[userBuffSize] = '\0';
		
	    cout<<buffer<< " No quiere jugar"<<endl;
	    
	    //menu = menuInicial;
	}
        
    } while(strcmp(buffer,"bye") != 0 );

}
