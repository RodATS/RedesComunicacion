<h1>Tarea</h1>
Protocolos de LogIn (L), LogOut (O), Msg (M) , Msg_all (A), Keep_alive (K) <br>
Revisar en el cuaderno.<br>

<h3>Review com funciona el write and read: Codigo:</h3>
Cliente: <br>
char buffer[1000]
 <br> Write(socketDescriptor,buffer, longitud del mensaje)//
 <br> read(socketD,buffer, necesitamos informacion para saber el tmaaño del mensaje)//
 <br> int s = atoi(buffer);
<br> read(socketD,buffer,s);

Otro ejemplo:
<br> strpcy(buffer,"hello... LA:");
<br> strpcy(buffer,"bye");
<br> sprintf(buffer2,"%s02", sizeof(buffer));
<br> write(socketD, buffer, sizeof(buffer) = 2); //2 es protocol

Continua en el cuaderno lab02 Tarea :(
