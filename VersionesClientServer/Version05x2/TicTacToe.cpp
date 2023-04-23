#include <iostream>
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


int main() {
  int movimiento;
  int jugador = 1;
  while(TableroCompleto() == false && VerficarGanador() ==0){
    PrintTablero();
    if(jugador == 1){
      cout<<"Mover jugador 1 (O): ";
      cin>>movimiento;
      
      ColocarFicha(movimiento, jugador);
      jugador = 2;
    }

    else{
      cout<<"Mover jugador 2 (X): ";
      cin>>movimiento;
      
      ColocarFicha(movimiento, jugador);
      jugador=1;
    }

  }
  PrintTablero();
  int ganador = VerficarGanador();
  switch (ganador){
    case 0:{
      cout<<"Empate"<<endl;
      break;
    }

    case 1:{
      cout<<"Ganó Jugador 1"<<endl;
      break;
    }

    case 2:{
      cout<<"Gano jugador 2"<<endl;
      break;
    }
  }
}
