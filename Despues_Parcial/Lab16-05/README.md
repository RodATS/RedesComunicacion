Esta en elaula virtual: book socket guide (lIbro <br>
Cap 7: Técnicas avanzadas - Select<br>

<li>Leer todo por siaca</li>
<li>SET: Vector del cuaderno select</li>
<li>Hay Funcinodes FD_: ejemplo FD_ZERO limpia el vector. </li>
 <li> Hay un codigo en la pag 37 (enumeración del documento) al final.</li>
 
 <br>
 El select es para todo file desctriptor: terminal, mouse, socket, todo ls dispositivos y archivos.


<h4> Código </h4>
<li>master lista de vectores. </li>
<li> read_fds: donde leere los vecotres </li>
<li> fdmax: max de vectores, cero al inicio </li>
<li> listener </li>
<li> newfd </li>
<li> remoteaddre: estructura que indica puerto del cliente y algo más </li>
<li> addrlen: tamaño de la estructura. </li>
<li> nbytes: para lectura creo? </li>
<li> read_fds: temporal del master, copia </li>

LAS VARIABLES SON DIFERNETES PERO EL PROCEDIMINETO SI ES CONOCIDO <br>
<li> binding: conectar la estructura con el socket, el server lo tiene pero diferente.</li>
<li> FD_SET listener: va en el vector master.</li>

<h6>fdmax = listener;</h6>
saber el max tamaño de los descriptores en el master. Emparejamos los descriptores con el maximo, sabremos el descriptor mayor (los descriptores son secuenciales).

<h3> Tarea: copiar le codigo, compilarlo para el 23/05</h3>
