Continuación
<br>
Protocolo: estructura<br>
9 Bytes: ID  - 7 Bytes: size - Variable: Data  - 10 Bytes

<br>
<h3> Servidor (con select) </h3>
Cualquier cosa q el cliente escriba, el server lo reenviará a los demás clientes. Pero adicionando el número del mensaje<br>
Seperar cada campo por ":" y un "\n"<br>
Número del mensaje: será de 5 BYtes  -   9 Bytes: ID  - 7 Bytes: size - Variable: Data  - 10 Bytes <br>
Mandar los 10K archivos.<br>

<h4>Importante</h4>
Número del mensaje: será de 5 BYtes  -   9 Bytes: ID, ver si estos son iguales, para saber si hay pérdida de "paquetes". <br>

<h3>Como mandar el mensaje</h3>
Cat: abre el archivo standard output -> standar input del server <br>
cat data.txt | nc  al servidor con IPAdrr: 127.0.0.1  -  puerto: 9034 <br>
cat data.txt | nc 127.0.0.1 9034
<h3>Como leer el mensaje</h3>
En otra terminal, actua como cliente <br>
nc 127.0.0.1 9034




