#include <gazebo-11/gazebo/gazebo.hh>
#include <functional>
#include <gazebo-11/gazebo/physics/physics.hh>
#include <gazebo-11/gazebo/common/common.hh>
#include <ignition/math6/ignition/math/Vector3.hh>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>

// Includes for the socket
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
            // Create the server socket
            this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            // Datatype used for storing the address of the socket
            sockaddr_in serverAdress;
            serverAdress.sin_family = AF_INET;
            // This function converts the unsigned int from machine byte order to network byte order
            serverAdress.sin_port = htons(8080);
            // We do not want to bind the socket to a particular IP address, just listen to all the available IPs
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

            // Create the parameters as variables:
            double proportional1 = 0;
            double integral1 = 0;
            double derivative1 = 0;
            double target1 = 0;

            double proportional2 = 0;
            double integral2 = 0;
            double derivative2 = 0;
            double target2 = 0;

            // Get the plugin parameters:

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

            // Store the pointer to the model
            this->model = _model;
            this->baseLink = model->GetLinks()[0];

            this->rev1 = model->GetJoints()[2];
            this->rev2 = model->GetJoints()[4];

            std::cerr << "\nSe ha iniciado el plugin con el modelo [" << this->model->GetName() << "]\n";

            std::cerr << "Los joints de entrada se llaman: " << this->rev1->GetName() << " y " << this->rev2->GetName() << std::endl;

            // Listen to the update event. This event is broadcast every
            // simulation iteration.
            this->updateConnection = event::Events::ConnectWorldUpdateBegin(
                std::bind(&ModelPush::OnUpdate, this));

            this->pid1 = common::PID(proportional1, integral1, derivative1);
            this->pid2 = common::PID(proportional2, integral2, derivative2);

            this->model->GetJointController()->SetPositionPID(
                this->rev1->GetScopedName(), this->pid1);

            this->model->GetJointController()->SetPositionTarget(
                this->rev1->GetScopedName(), target1);

            this->model->GetJointController()->SetPositionPID(
                this->rev1->GetScopedName(), this->pid2);

            this->model->GetJointController()->SetPositionTarget(
                this->rev2->GetScopedName(), target2);
        }

        // Called by the world update start event
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
                recv(this->clientSocket, this->buffer, sizeof(this->buffer), 0);

                switch (buffer[0]){
                    case 't':
                    {

                        // El cambio aquí es que se separa entre el PID 1 y el 2
                        // Primero se divide el mensaje con el método ya creado
                        std::vector<double> mensaje_div = dividirMensaje(buffer);

                        // Y luego se filtra dependiendo del segundo número del mensaje y se 
                        if (buffer[1] == '1'){
                            int pid1_ok = this->model->GetJointController()->SetPositionTarget(
                                this->rev1->GetScopedName(), mensaje_div[0]);
                            // TODO: Realizar comprobación de correcto establecimiento del PID1
                        }else if (buffer[1] == '2'){
                            int pid2_ok = this->model->GetJointController()->SetPositionTarget(
                                this->rev2->GetScopedName(), mensaje_div[0]);
                            // TODO: Realizar comprobación de correcto establecimiento del PID2
                        }
                        

                        // if ((pid1_ok == 1) && (pid2_ok == 1))
                        // {
                        //     std::cout << "Se han establecido los target de los PID correctamente." << std::endl;
                        //     std::cout << "Los valores son: " << std::endl;
                        //     std::cout << "  PID 1: " << mensaje_div[0] << std::endl;
                        //     std::cout << "  PID 2: " << mensaje_div[1] << std::endl;
                        // }
                        // else
                        // {
                        //     std::cerr << "Error al establecer los valores de los PID." << std::endl;
                        //     std::cerr << "  El PID 1 ha devuelto: " << pid1_ok << std::endl;
                        //     std::cerr << "  El PID 2 ha devuelto: " << pid2_ok << std::endl;
                        // }
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
            std::stringstream ss(mensaje); // Creamos un stringstream con el mensaje

            std::string parte;
            while (ss >> parte)
            { // Leemos cada parte del stringstream
                double valor;
                if (std::istringstream(parte) >> valor)
                { // Intentamos convertir la parte en un double
                    // Si la conversión es exitosa, agregamos el valor al vector resultado
                    resultado.push_back(valor);
                }
            }

            return resultado;
        }

    private:
        // Pointer to the model
        physics::ModelPtr model;

        // Pointer to the base link
        physics::LinkPtr baseLink;

        // Pointers to the joints
        physics::JointPtr rev1;
        physics::JointPtr rev2;

        /// A PID controller for the joint.
        common::PID pid1;
        common::PID pid2;

        // Pointer to the update event connection
        event::ConnectionPtr updateConnection;

        // Socket
        int serverSocket;
        int clientSocket;
        char buffer[1024] = {0};

        // Commands
        bool show_positions = false;
    };

    // Register this plugin with the simulator
    GZ_REGISTER_MODEL_PLUGIN(ModelPush)
}