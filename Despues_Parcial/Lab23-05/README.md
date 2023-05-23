Continuación
<br>
Protocolo: estructura<br>
9 Bytes: ID  - 7 Bytes: size - Variable: Data  - 10 Bytes

<br>
<h3> Servidor (con select) </h3>
Cualquier cosa q el cliente escriba, el server lo reenviará a los demás clientes. Pero adicionando el número del mensaje<br>
Número del mensaje  -   9 Bytes: ID  - 7 Bytes: size - Variable: Data  - 10 Bytes <br>
Mandar los 10K archivos.<br>


<h3>Como mandar el mensaje</h3>
Cat: abre el archivo standard output -> standar input del server <br>
cat data.txt nc (netCat) al servidor con IPAdrr: 127.0.0.1  -  puerto: 9034
