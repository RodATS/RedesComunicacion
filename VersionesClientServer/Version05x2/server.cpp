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

int Tablero[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
string fichas[3] = {"-","O","X"};

void ReiniciarTablero(){
	for(int i =0; i < 3; i++){
		for(int j =0;j<3; j++){
			Tablero[i][j]=0;
		}
	}
}


//Jugador1 = 1,  Jugador2 = 2
int VerficarGanador(){
	int ganador = 0; //0: indica que aún no hay ganador
	//gana jugador 1
  //modo horizontal
	if ( (Tablero[0][0] == 1 && Tablero[0][1] == 1 && Tablero[0][2] == 1 ) || 
     ( Tablero[1][0] == 1 && Tablero[1][1] == 1 && Tablero[1][2] == 1 ) ||
     ( Tablero[2][0] == 1 && Tablero[2][1] == 1 && Tablero[2][2] == 1 ) ||
  //vertical
    ( Tablero[0][0] == 1 && Tablero[1][0] == 1 && Tablero[1][0] == 1 ) ||
    ( Tablero[0][1] == 1 && Tablero[1][1] == 1 && Tablero[2][1] == 1 ) ||
    ( Tablero[0][2] == 1 && Tablero[1][2] == 1 && Tablero[2][2] == 1 ) ||

    //diagonal
    ( Tablero[0][0] == 1 && Tablero[1][1] == 1 && Tablero[2][2] == 1 ) ||
    ( Tablero[0][2] == 1 && Tablero[1][1] == 1 && Tablero[2][0] == 1 )
    )
  {
		ganador = 1;
	}

  else{
      if ( (Tablero[0][0] == 2 && Tablero[0][1] == 2 && Tablero[0][2] == 2 ) || 
       ( Tablero[1][0] == 2 && Tablero[1][1] == 2 && Tablero[1][2] == 2 ) ||
       ( Tablero[2][0] == 2 && Tablero[2][1] == 2 && Tablero[2][2] == 2 ) ||
    //vertical
      ( Tablero[0][0] == 2 && Tablero[1][0] == 2 && Tablero[1][0] == 2 ) ||
      ( Tablero[0][1] == 2 && Tablero[1][1] == 2 && Tablero[2][1] == 2 ) ||
      ( Tablero[0][2] == 2 && Tablero[1][2] == 2 && Tablero[2][2] == 2 ) ||
  
      //diagonal
      ( Tablero[0][0] == 2 && Tablero[1][1] == 2 && Tablero[2][2] == 2 ) ||
      ( Tablero[0][2] == 2 && Tablero[1][1] == 2 && Tablero[2][0] == 2 )
      )
    {
  		ganador = 2;
  	}
  }

  return ganador;
}

bool TableroCompleto(){
  for(int i =0; i < 3; i++){
		for(int j =0;j<3; j++){
			if(Tablero[i][j] == 0){
        return false;
      }
		}
	}

  //Si el tablero ya esta completo
  return true;
}


void ColocarFicha(int pos, int jugador){
  /*
  3 en raya
  _1_|_2_|_3_
  _4_|_5_|_6_
  _7_|_8_|_9_
*/
  switch (pos){
    case 1:{
        Tablero[0][0] = jugador;
        break;
    }

    case 2:{
        Tablero[0][1] = jugador;
        break;
    }

    case 3:{
        Tablero[0][2] = jugador;
        break;
    }

    case 4:{
        Tablero[1][0] = jugador;
        break;
    }

    case 5:{
        Tablero[1][1] = jugador;
        break;
    }

    case 6:{
        Tablero[1][2] = jugador;
        break;
    }

    case 7:{
        Tablero[2][0] = jugador;
        break;
    }

    case 8:{
        Tablero[2][1] = jugador;
        break;
    }

    case 9:{
        Tablero[2][2] = jugador;
        break;
    }
  }
}


void PrintTablero(){
  for(int i =0; i < 3; i++){
		for(int j =0;j<3; j++){
			cout<<fichas[Tablero[i][j]]<<" | ";
		}
    cout<<"\n---------"<<endl;
	}
}

//------------------------------------------------------------



map<string, int> users;
map<int, string> sockets;

map<string,int> jugadores;

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
                
	       //Vamo a jugar? INvitacion
 		case 'P':{ 
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

			write(rec, "P", strlen("P"));
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
                
			    //Respuesta al juego, Y yes
                case 'Y':{
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

			cout << "User " << sockets[socketC] << " accept game with " << user2 << endl;

			write(rec, "Y", strlen("Y"));
			//tamaño del nombre del usuario que mando el mensaje
			write(rec, cszuser2, strlen(cszuser2));
			//nombre usuario
			write(rec, user2, strlen(user2));

			cout << "Game Accepted" << endl;
			
			//Se agregan los jugadores al map:  map<string,int> jugadores;
                	jugadores.insert({sockets[socketC], 1});
			jugadores.insert({user2,2});
			
                	break;
                }
                
                //Rechaza invitacion para jugar
                case 'X':{
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

			cout << "User " << sockets[socketC] << " reject game with " << user2 << endl;

			write(rec, "X", strlen("X"));
			//tamaño del nombre del usuario que mando el mensaje
			write(rec, cszuser2, strlen(cszuser2));
			//nombre usuario
			write(rec, user2, strlen(user2));

			cout << "Game Rejected" << endl;
			//Se elimina a quien mandó la invitación del juego
			users.erase({user2});
                	break;
                }
			    
		case 'W': { //Movimientos 3 en raya
		
			//Tamaño del mensaje que se recibio
			n = read(socketC, buffer, 4);
			buffer[4] = '\0';

			//se lee el movimiento
			int size_m = atoi(buffer);
			char csize_m[10000];
			sprintf(csize_m, "%04d", size_m);
			n = read(socketC, buffer, size_m);
			buffer[size_m] = '\0';

			char *msg = (char*)malloc(sizeof(char) * size_m);
			sprintf(msg, "%s", buffer);
			
			//ficha pos movimiento
			int move = atoi(buffer);


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

			cout << "Jugador: " << sockets[socketC] << " coloco en la posicion " << msg << endl;

			ColocarFicha(move,jugadores[sockets[socketC]]);
			
			write(rec, "W", strlen("W"));
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
