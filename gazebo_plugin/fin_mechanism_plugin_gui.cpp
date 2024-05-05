#include <gazebo-11/gazebo/gazebo.hh>
#include <functional>
#include <gazebo-11/gazebo/physics/physics.hh>
#include <gazebo-11/gazebo/common/common.hh>
#include <ignition/math6/ignition/math/Vector3.hh>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace gazebo
{
    class ModelPush : public ModelPlugin
    {
    public:
        void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
        {
            std::cout << "Es necesario hacer conexión con el cliente antes de continuar." << std::endl;
            // Creación del socket
            this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

            // Creación de las variables para almacenar la información de la dirección de conexión
            sockaddr_in serverAdress;
            serverAdress.sin_family = AF_INET;

            serverAdress.sin_port = htons(8080);
            // No hacemos el bind a una IP concreta, sino que se escuchan a todas las IP disponibiles
            serverAdress.sin_addr.s_addr = INADDR_ANY;
            bind(this->serverSocket, (struct sockaddr *)&serverAdress, sizeof(serverAdress));

            listen(this->serverSocket, 5);

            if ((this->clientSocket = accept(this->serverSocket, nullptr, nullptr)) > 0)
            {
                std::cout << "La conexión con el cliente ha sido establecida con éxito." << std::endl;
            }
            else
            {
                std::cerr << "Atención. Error en la conexión con el cliente." << std::endl;
                std::cerr << "Vuelva a ejecutar el programa." << std::endl;
                exit(0);
            }

            // Creación de las variables para los parámetros:
            double proportional1 = 0;
            double integral1 = 0;
            double derivative1 = 0;
            double target1 = 0;

            double proportional2 = 0;
            double integral2 = 0;
            double derivative2 = 0;
            double target2 = 0;

            // Obtención de los parámetros del plugin:

            if (_sdf->HasElement("proportional1"))
                proportional1 = _sdf->Get<double>("proportional1");

            if (_sdf->HasElement("integral1"))
                integral1 = _sdf->Get<double>("integral1");

            if (_sdf->HasElement("derivative1"))
                derivative1 = _sdf->Get<double>("derivative1");

            if (_sdf->HasElement("target1"))
                target1 = _sdf->Get<double>("target1");

            if (_sdf->HasElement("proportional2"))
                proportional2 = _sdf->Get<double>("proportional2");

            if (_sdf->HasElement("integral2"))
                integral2 = _sdf->Get<double>("integral2");

            if (_sdf->HasElement("derivative2"))
                derivative2 = _sdf->Get<double>("derivative2");

            if (_sdf->HasElement("target2"))
                target2 = _sdf->Get<double>("target2");

            // Variable para guardar el puntero al modelo
            this->model = _model;
            this->baseLink = model->GetLinks()[0];

            this->rev1 = model->GetJoints()[2];
            this->rev2 = model->GetJoints()[4];

            std::cerr << "\nSe ha iniciado el plugin con el modelo [" << this->model->GetName() << "]\n";

            std::cerr << "Los joints de entrada se llaman: " << this->rev1->GetName() << " y " << this->rev2->GetName() << std::endl;

            this->updateConnection = event::Events::ConnectWorldUpdateBegin(
                std::bind(&ModelPush::OnUpdate, this));

            this->pid1 = common::PID(proportional1, integral1, derivative1);
            this->pid2 = common::PID(proportional2, integral2, derivative2);

            this->model->GetJointController()->SetPositionPID(
                this->rev1->GetScopedName(), this->pid1);

            this->model->GetJointController()->SetPositionTarget(
                this->rev1->GetScopedName(), target1);

            this->model->GetJointController()->SetPositionPID(
                this->rev2->GetScopedName(), this->pid2);

            this->model->GetJointController()->SetPositionTarget(
                this->rev2->GetScopedName(), target2);
        }

    public:
        void OnUpdate()
        {
            // Activa o desactiva la impresión de las posiciones en pantalla de forma constante
            if (show_positions)
            {
                std::system("clear");
                std::cout << this->rev1->GetName() << " position is: " << this->rev1->Position(0) << std::endl;
                std::cout << this->rev2->GetName() << " position is: " << this->rev2->Position(0) << std::endl;
            }

            // Configuración del socket
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(this->clientSocket, &readfds);

            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;

            /*
             Comprobar si hay algún mensaje listo para ser recivido. 
             Básicamente se comprueba si ha habido cambios en el fichero del socket.
             Esto se utiliza para evitar una llamada bloqueante (recv) sin que haya mensajes que recibir, lo que pararía
             la simulación.
            */ 
            int ready = select(this->clientSocket + 1, &readfds, NULL, NULL, &timeout);

            if (ready > 0 && FD_ISSET(this->clientSocket, &readfds))
            {
                // Se iinicializa a 0 el buffer antes de realizar la lectura para evitar restos de valores anteriores
                memset(buffer, 0, sizeof(buffer));

                recv(this->clientSocket, this->buffer, sizeof(this->buffer), 0);

                std::cout << buffer << std::endl;

                switch (buffer[0]){
                    case 't':
                    {

                        // El cambio aquí es que se separa entre el PID 1 y el 2
                        // Primero se limpia el vector de los datos anteriores
                        // Y a continuación se divide el mensaje con el método ya creado
                        std::vector<double> mensaje_div;
                        mensaje_div.clear();
                        mensaje_div = dividirMensaje(buffer);

                        // Luego se filtra dependiendo del segundo número del mensaje y se actúa en consecuencia
                        if (buffer[1] == '1'){
                            int pid1_ok = this->model->GetJointController()->SetPositionTarget(
                                this->rev1->GetScopedName(), mensaje_div[0]);
                            
                            if (pid1_ok == 1){
                                std::string return_str;
                                return_str = "t Se ha establecido el target del PID 1 correctamente.";
                                return_str += "  PID 1: "; 
                                return_str += std::to_string(mensaje_div[0]);
                                // std::cout << return_str << std::endl;
                                strncpy(return_message, return_str.c_str(), sizeof(return_message)); // Copiar cadena a arreglo de caracteres
                                return_message[sizeof(return_message) - 1] = '\0';

                                send(this->clientSocket, this->return_message, sizeof(this->return_message), 0);
                            }
                            else{  
                                std::string return_str;
                                return_str = "t Ha habido un error al establecer el target del PID 1.";
                                // std::cout << return_str << std::endl;
                                strncpy(return_message, return_str.c_str(), sizeof(return_message)); // Copiar cadena a arreglo de caracteres
                                return_message[sizeof(return_message) - 1] = '\0';

                                send(this->clientSocket, this->return_message, sizeof(this->return_message), 0);
                            }

                        }else if (buffer[1] == '2'){
                            int pid2_ok = this->model->GetJointController()->SetPositionTarget(
                                this->rev2->GetScopedName(), mensaje_div[0]);

                            if (pid2_ok == 1){
                                std::string return_str;
                                return_str = "t Se ha establecido el target del PID 2 correctamente.";
                                return_str += "  PID 2: ";
                                return_str += std::to_string(mensaje_div[0]);
                                // std::cout << return_str << std::endl;
                                strncpy(return_message, return_str.c_str(), sizeof(return_message)); // Copiar cadena a arreglo de caracteres
                                return_message[sizeof(return_message) - 1] = '\0';

                                send(this->clientSocket, this->return_message, sizeof(this->return_message), 0);
                            }
                            else{  
                                std::string return_str;
                                return_str = "t Ha habido un error al establecer el target del PID 2.";
                                // std::cout << return_str << std::endl;
                                strncpy(return_message, return_str.c_str(), sizeof(return_message)); // Copiar cadena a arreglo de caracteres
                                return_message[sizeof(return_message) - 1] = '\0';

                                send(this->clientSocket, this->return_message, sizeof(this->return_message), 0);
                            }
                        }
                    }
                        break;
                    case 's':
                        show_positions = !show_positions;                       
                        break;
                    case '0':
                        std::cout << "El socket se ha cerrado." << std::endl;
                        exit(0);
                        break;
                    default:
                        std::cout << "Por favor, escriba un comando válido" << std::endl;
                        break;
                }
            }
        }

        std::vector<double> dividirMensaje(const char *mensaje)
        {
            std::vector<double> resultado;
            std::stringstream ss(mensaje); // Se crea un stringstream con el mensaje

            std::string parte;
            while (ss >> parte)
            { // Se lee cada parte del stringstream
                double valor;
                if (std::istringstream(parte) >> valor)
                { // Se intenta convertir la parte en un double
                    // Si la conversión es exitosa, se agrega el valor al vector resultado
                    resultado.push_back(valor);
                }
            }

            return resultado;
        }

    private:
        // Puntero al modelo
        physics::ModelPtr model;

        // Puntero al link base
        physics::LinkPtr baseLink;

        // Punteros a las juntas o joints
        physics::JointPtr rev1;
        physics::JointPtr rev2;

        /// Punteros para los PID de las joints
        common::PID pid1;
        common::PID pid2;

        // Puntero para el evento de actualización
        event::ConnectionPtr updateConnection;

        // Socket
        int serverSocket;
        int clientSocket;
        char buffer[1024] = {0};
        char return_message[1024] = {0};

        // Commandos
        bool show_positions = false;
    };

    // Registro del plugin para la simulación
    GZ_REGISTER_MODEL_PLUGIN(ModelPush)
}