Esta en elaula virtual: book socket guide (lIbro <br>
Cap 7: Técnicas avanzadas - Select<br>

<li>Leer todo por siaca/<li>
<li>SET: Vector del cuaderno select</li>
<li>Hay Funcinodes FD_: ejemplo FD_ZERO limpia el vector. </li>
 <li> Hay un codigo en la pag 37 al final.</li>
 
 <br>
 El select es para todo file desctriptor: terminal, mouse, socket, todo ls dispositivos y archivos.


<h4> Código </h4>
master lista de vectores. <br>
read_fds: donde leere los vecotres
fdmax: max de vectores, cero al inicio
listener
newfd
remoteaddre: estructura que indica puerto del cliente y algo más
addrlen: tamaño de la estructura.
nbytes: para lectura creo?

read_fds: temporal del master, copia <br>
LAS VARIABLES SON DIFERNETES PERO EL PROCEDIMINETO SI ES CONOCIDO <br>
binging: conectar la estructura con el socket, el server lo tiene pero diferente.<br>
FD_SET listener: va en el vector master.<br>

<h6>fdmax = listener;</h6>
saber el max tamaño de los descriptores en el master. Emparejamos los descriptores con el maximo, sabremos el descriptor mayor (los descriptores son secuenciales).

<h3> Tarea: copiar le codigo, compilarlo para el 23/05</h3>
