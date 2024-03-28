#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

int main() 
{ 
    // creating socket 
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0); 
  
    // specifying address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8080); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
  
    // sending connection request 
    if ((connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) == 0){
        cout << "La conexión con el servidor ha sido establecida con éxito." << endl;
    } else {
        cerr << "Atención. Error en la conexión con el servidor." << endl;
        cerr << "Vuelva a ejecutar el programa." << endl;
        exit(0);
    }
    
    char buffer[1024] = {0};
    char lower_buffer[1024] = {0};

    // sending data 
    cout << "Bienvenido a la interfaz con Gazebo. Para ver los comandos utilice h" << endl;
    while (true){
        cout << "Escriba a continuación el comando: ";
        cin.getline(buffer, sizeof(buffer));
        transform(buffer, buffer + strlen(buffer), lower_buffer, ::tolower);
        if (buffer[0] == 'h'){
            cout << "--- Manual de comandos ---" << endl;
            cout << " c                   Para limpiar la pantalla." << endl;
            cout << " p 1 <p1> <i1> <d1>  Para introducir los parámetros del nuevo PID 1" << endl;
            cout << " p 2 <p2> <i2> <d2>  Para introducir los parámetros del nuevo PID 2" << endl;
            cout << " s                   Para mostrar la posición de cada iteración." << endl;
            cout << "                     Al mandarlo de nuevo se desactiva." << endl;
            cout << " t <pos1> <pos2>     Para introducir nuevas posiciones." << endl;
        }
        else if (buffer[0] == 'c'){
            std::system("clear");
        }
        else if(buffer[0] != 'c' && buffer[0] != 'p' && buffer[0] != 's' && buffer[0] != 't' && buffer[0] != 'h')
            cout << "Por favor, escriba un comando válido." << endl;
        else
            send(clientSocket, lower_buffer, strlen(lower_buffer), 0);
    }
  
    // closing socket 
    close(clientSocket); 
  
    return 0; 
}