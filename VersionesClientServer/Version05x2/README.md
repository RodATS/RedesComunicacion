TicTacToe: codigo separado, todo funca siu <br>
Server: ya tiene el tic tac, falta terminar el protocolo de los movimientos y enviar el tablero para que lo vea el cliente. <br>
Ya funcinoa el server, YA LO ARREGLÉ
Hay que explicar cada funcion? <br>
Yo puse esto (CLiente) <br>	Internamente se guarda así: 01N (tamaño mensaje) (mensaje) (tamaño nombre usuario destinatario) (nombre usuario destinatario). <br>
string temp = "N" +  string(size_m) + string(msg) + string(userBuffSize) + string(receiver);
