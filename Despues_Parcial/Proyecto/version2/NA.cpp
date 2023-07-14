#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <cstring>
#include <thread>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#include <algorithm>

using namespace std;

//por siaca 
map<string, vector<string>> BD_word;
map<string, vector<string>> BD_glosario;


void thread_read(int socketC);
void writetoFile(string campo, string data, string file);


string leer_mapa_dato(string busqueda, map<string, vector<string>> BD){
    string encontrados = "";
    for(auto it = BD.begin(); it != BD.end(); it++){
        for(auto x : it->second)
        {
            if(busqueda == x){
                encontrados+= it->first+" ,";
            }
        }
    }
    return encontrados;
}




void actualizar_info(map<string, vector<string>>& BD, string campo, string antiguaInfo, string nuevaInfo) {
    if (BD.find(campo) != BD.end()) {
        vector<string>& infoCampo = BD[campo];
        auto it = find(infoCampo.begin(), infoCampo.end(), antiguaInfo);
        if (it != infoCampo.end()) {
            *it = nuevaInfo;
        }
    } else {
        std::cout << "No encontrado" << std::endl;
    }

    cout << "Informacion actualizada" << endl;
}


void eliminar_info(map<string, vector<string>> BD, string campo, string antiguaInfo){
   // vector<string> resultados;
    
    if(BD.find(string(campo)) != BD.end())
    {
        auto it =  find((BD[string(campo)]).begin(), (BD[string(campo)]).end(), antiguaInfo) ;
         if (it != (BD[string(campo)]).end()) {
            (BD[string(campo)]).erase(it);
        }
    }else{
        std::cout<<"No encontrado\n";
    }

    cout<<"Informacion actualizada\n";
}




int main(int argc, char* argv[])
{
    int port;
    //-------------------------PUERTO------------
    port = (argc > 1) ? std::stoi(argv[1]) : 9034;
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Es TCP
    int n;
    if (-1 == SocketFD)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    stSockAddr.sin_family = AF_INET; //Tipo de comunicación 
    stSockAddr.sin_port = htons(port);//Puerto del servidor
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);//IP

    if (0 > Res)
    {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
        perror("char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    char* buff;
    bool end = false;
    buff = new char[1000];
    string Tid = "";
    
    for(;;)
    {
        //-----------------ID--TRANSACCION-----------------
        /*
        n = recv(SocketFD,buff,6,0);
        buff[6] = '\0';
        Tid = string(buff);
        */
        //-------------------------------------------------
        n = recv(SocketFD,buff,1,0);
        buff[1] = '\0';
        switch(buff[0])
        {

            //--------------------------------------CREATE-----------------------------------------------
            case 'c': //Create
            {

                n = recv(SocketFD,buff,1,0);
                buff[1] = '\0';

                switch(buff[0])
                {
                    case 'w': //word
                    {
                        //leer el tamaño de campo
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int cSize = atoi(buff);
                        char* campo = new char[cSize+1];

                        //leer campo
                        n = recv(SocketFD,campo,cSize,0);
                        campo[cSize] = '\0';

                        //leer el tamaño de word
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int dSize = atoi(buff);
                        char* data = new char[dSize+1];

                        //leer word
                        n = recv(SocketFD,data,dSize,0);
                        data[dSize] = '\0';
                        cout<< string(campo) <<"-"<<string(data)<<endl;

                        //agrega
                        BD_word[string(campo)].push_back(string(data));
                        cout<<string(campo)<<endl; cout<<BD_word[string(campo)][0]<<endl;
                        cout<<"Se agrego un word\n";
                        //writetoFile(string(campo),string(data),"file.txt");
                        
                        //n = recv(SocketFD,buff,1,0); //leer el '\n'
                        
                        //BD.insert({string(campo), string(data)});
                        
                        break;
                    }

                    case 'g': //Glosario
                    {
                        //leer el tamaño de campo
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int cSize = atoi(buff);
                        char* campo = new char[cSize+1];

                        //leer campo
                        n = recv(SocketFD,campo,cSize,0);
                        campo[cSize] = '\0';

                        //leer el tamaño de glosario
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int dSize = atoi(buff);
                        char* data = new char[dSize+1];

                        //leer glosario
                        n = recv(SocketFD,data,dSize,0);
                        data[dSize] = '\0';
                        cout<< string(campo) <<"-"<<string(data)<<endl;

                        //agrega
                        BD_glosario[string(campo)].push_back(string(data));
                        cout<<string(campo)<<endl; cout<<BD_glosario[string(campo)][0]<<endl;
                        cout<<"Se agrego un glosario\n";

                        break;
                    }

                    default:
                        cout<<"Comando de Create no reconocido\n";
                        break;
                }
            }
            //------------------------------END-CREATE----------------------------------------

            //------------------------------READ-------------------------------------------------
            case 'r': //read
            {
                n = recv(SocketFD,buff,1,0);
                buff[1] = '\0';

                switch(buff[0])
                {
                    

                        case 'w': //leer un BD word
                        {
                            n = recv(SocketFD,buff,3,0);
                            buff[3] = '\0';
                            int cSize = atoi(buff);
                            char* campo = new char[cSize+1];
                            n = recv(SocketFD,campo,cSize,0);
                            campo[cSize] = '\0';
                            vector<string>resultados;
                            if(BD_word.find(string(campo)) != BD_word.end())
                            {
                                resultados = BD_word[string(campo)];
                            }else{
                                std::cout<<"No encontrado\n";
                            }
                            for(int i = 1; i < resultados.size(); i++)
                            {
                                resultados[0]+=resultados[i];
                            }

                            send(SocketFD, resultados[0].c_str(), resultados[0].length(),0);

                            break;
                        }

                        case 'g': //leer un BD g
                        {
                            n = recv(SocketFD,buff,3,0);
                            buff[3] = '\0';
                            int cSize = atoi(buff);
                            char* campo = new char[cSize+1];
                            n = recv(SocketFD,campo,cSize,0);
                            campo[cSize] = '\0';
                            vector<string>resultados;
                            if(BD_glosario.find(string(campo)) != BD_glosario.end())
                            {
                                resultados = BD_glosario[string(campo)];
                            }else{
                                std::cout<<"No encontrado\n";
                            }
                            for(int i = 1; i < resultados.size(); i++)
                            {
                                resultados[0]+=resultados[i];
                            }

                            send(SocketFD, resultados[0].c_str(), resultados[0].length(),0);

                            break;
                        }

                        default:
                            cout<<"Comando de Read no reconocido\n";
                            break;

                    }

                    //----------NO SE SI ESTE TAMBIEN HAREMOS YA QUE EL GLOSARIO NO NOS DEJARIA--------------------------
                    /*
                    case 'd'://leer por dato
                    {
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int cSize = atoi(buff);
                        char* campo = new char[cSize+1];
                        n = recv(SocketFD,campo,cSize,0);
                        campo[cSize] = '\0';
                        string resultados = leer_mapa_dato(string(campo), BD_word);
                        if(resultados == "") cout<<"No encontrado\n";
                        else{
                            Tid = Tid + "000" + to_string(resultados.length()) + resultados;

                            send(SocketFD, Tid.c_str(), Tid.length(),0);

                            Tid = "";
                        }
                        
                        break;
                    }

                    */

            }
             
            
            //---------------------------END--READ----------------------------------------------
        

            //---------------------------UPDATE---------------------------------------------
            case 'u'://update
            {
                n = recv(SocketFD,buff,1,0);
                buff[1] = '\0';

                switch (buff[0])
                {
                                   
                    case 'w':
                    {
                        //leer el tamaño de campo
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int cSize = atoi(buff);
                        char* campo = new char[cSize+1];

                        //leer campo
                        n = recv(SocketFD,campo,cSize,0);
                        campo[cSize] = '\0';

                        //leer el tamaño de word
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int dSize = atoi(buff);
                        char* data = new char[dSize+1];

                        //leer word
                        n = recv(SocketFD,data,dSize,0);
                        data[dSize] = '\0';

                        //leer el tamaño de nuevo dato
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int nSize = atoi(buff);
                        char* nData = new char[nSize+1];

                        //leer nuevo dato
                        n = recv(SocketFD,nData,nSize,0);
                        nData[nSize] = '\0';

                        actualizar_info(BD_word, campo, data, nData);

                        break;
                    }

                    case 'g':
                    {
                        //leer el tamaño de campo
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int cSize = atoi(buff);
                        char* campo = new char[cSize+1];

                        //leer campo
                        n = recv(SocketFD,campo,cSize,0);
                        campo[cSize] = '\0';

                        //leer el tamaño de word
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int dSize = atoi(buff);
                        char* data = new char[dSize+1];

                        //leer word
                        n = recv(SocketFD,data,dSize,0);
                        data[dSize] = '\0';

                        //leer el tamaño de nuevo dato
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int nSize = atoi(buff);
                        char* nData = new char[nSize+1];

                        //leer nuevo dato
                        n = recv(SocketFD,nData,nSize,0);
                        nData[nSize] = '\0';

                        actualizar_info(BD_glosario, campo, data, nData);
                        break;
                    }

                    default:
                        cout<<"Comando de Update no reconocido\n";
                        break;
                }
            }

            //----------------------------END-UPDATE--------------------------



            //----------------------------DELETE-----------------
            case 'd'://delete
            {
                n = recv(SocketFD,buff,1,0);
                buff[1] = '\0';

                switch (buff[0])
                {
                                   
                    case 'w':
                    {
                        //leer el tamaño de campo
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int cSize = atoi(buff);
                        char* campo = new char[cSize+1];

                        //leer campo
                        n = recv(SocketFD,campo,cSize,0);
                        campo[cSize] = '\0';

                        //leer el tamaño de word
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int dSize = atoi(buff);
                        char* data = new char[dSize+1];

                        //leer word
                        n = recv(SocketFD,data,dSize,0);
                        data[dSize] = '\0';


                        eliminar_info(BD_word, campo, data);

                        break;
                    }

                    case 'g':
                    {
                        //leer el tamaño de campo
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int cSize = atoi(buff);
                        char* campo = new char[cSize+1];

                        //leer campo
                        n = recv(SocketFD,campo,cSize,0);
                        campo[cSize] = '\0';

                        //leer el tamaño de word
                        n = recv(SocketFD,buff,3,0);
                        buff[3] = '\0';
                        int dSize = atoi(buff);
                        char* data = new char[dSize+1];

                        //leer word
                        n = recv(SocketFD,data,dSize,0);
                        data[dSize] = '\0';


                        eliminar_info(BD_glosario, campo, data);
                        break;
                    }

                    default:
                        cout<<"Comando de Update no reconocido\n";
                        break;
                }
            }

            //-----------------------------END-DELETE-------------------------


            default:
                cout<<"Comando no reconocido\n";
                break;

        }

        //printf("Here is the message: [%s]\n",buff);

        
    }
        
    delete buff;
    /* perform read write operations ... */
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
}

void writetoFile(string campo, string data, string file)
{
    ofstream outputfile(file, ios::app);
    if(!outputfile.is_open())
    {
        cout<<"Error al abrir el file\n";
        return;
    }
    ostringstream oss;
    for(auto x : data)
        oss << x;
    oss<<" ";
    for(auto x : campo)
        oss << x;
    oss<<"\n";
    outputfile<<oss.str();
    outputfile.close();
}


/* 
int main() {
    std::ofstream archivo("archivo.txt", std::ios::app);  // Abre el archivo en modo de escritura (appending)

    if (archivo.is_open()) {
        archivo << "Texto adicional que se va a concatenar al archivo existente.\n";
        archivo.close();
        std::cout << "Texto agregado exitosamente al archivo." << std::endl;
    } else {
        std::cout << "No se pudo abrir el archivo." << std::endl;
    }

    return 0;
}
*/




