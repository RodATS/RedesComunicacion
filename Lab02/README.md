Review com funciona el write and read: Codigo:
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

Continua en el cuaderno lab02 :(
