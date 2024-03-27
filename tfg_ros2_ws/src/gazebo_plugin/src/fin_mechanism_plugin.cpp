#include <gazebo-11/gazebo/gazebo.hh>
#include <functional>
#include <gazebo-11/gazebo/physics/physics.hh>
#include <gazebo-11/gazebo/common/common.hh>
#include <ignition/math6/ignition/math/Vector3.hh>
#include <thread>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "gazebo_ros/node.hpp"

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

            if ((this->clientSocket = accept(this->serverSocket, nullptr, nullptr)) > 0){
                std::cout << "La conexión con el cliente ha sido establecida con éxito." << std::endl;
            }else{
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

            std::cerr << "\nThe plugin is attached to model[" << this->model->GetName() << "]\n";

            std::cerr << "The links are called: " << this->rev1->GetName() << " and " << this->rev2->GetName() << std::endl;

            // Listen to the update event. This event is broadcast every
            // simulation iteration.
            this->updateConnection = event::Events::ConnectWorldUpdateBegin(
                std::bind(&ModelPush::OnUpdate, this));

            this->pid1 = common::PID(proportional1, integral1, derivative1);
            this->pid2 = common::PID(proportional2, integral2, derivative2);

            this->model->GetJointController()->SetPositionPID(
                this->rev1->GetScopedName(), this->pid1);

            std::cerr << this->model->GetJointController()->SetPositionTarget(
                this->rev1->GetScopedName(), target1);

            this->model->GetJointController()->SetPositionPID(
                this->rev1->GetScopedName(), this->pid2);

            std::cerr << this->model->GetJointController()->SetPositionTarget(
                this->rev2->GetScopedName(), target2);
        }

        // Called by the world update start event
    public:
        void OnUpdate()
        {
            // Apply a small linear velocity to the model.
            // this->model->SetLinearVel(ignition::math::Vector3d(.3, 0, 0));

            // Apply a small force to the model. This way the gravity is still working
            // this->baseLink->AddForce(ignition::math::Vector3d(100, 0, 0));

            // std::system("clear");
            // std::cout << this->rev1->GetName() << " position is: " << this->rev1->Position(0) << std::endl;
            // std::cout << this->rev2->GetName() << " position is: " << this->rev2->Position(0) << std::endl;

            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(this->clientSocket, &readfds);
    
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;

            int ready = select(this->clientSocket + 1, &readfds, NULL, NULL, &timeout);

            if (ready > 0 && FD_ISSET(this->clientSocket, &readfds)) {
                recv(this->clientSocket, this->buffer, sizeof(this->buffer), 0);
                // int received = recv(this->clientSocket, this->buffer, sizeof(this->buffer), 0);
                std::cout << "El buffer contiene: " << buffer << std::endl;

                if (buffer[0] == 't'){
                    int i = 2;
                    int j = 0;

                    char t1[1024] = {0};
                    char t2[1024] = {0};

                    std::cout << "Buffer antes del while t1: " << buffer[i] << std::endl;
                    while(buffer[i] != ' '){
                        std::cout << "Inside t1 while loop." << std::endl;
                        t1[j] = buffer[i];
                        i++;
                        j++;
                    }
                    i++;
                    j = 0;
                    std::cout << "El buffer i tiene el valor: " << buffer[i] << std::endl;

                    while(buffer[i] != '\0'){
                        std::cout << "Inside t2 while loop." << std::endl;
                        t2[j] = buffer[i];
                        i++;
                        j++;
                    }
                    
                    std::cout << t1 << std::endl;
                    std::cout << t2 << std::endl;
                    
                    double d_t1 = atof(t1);

                    std::cout << "El valor de t1 es: " << d_t1 << std::endl;

                    std::cerr << this->model->GetJointController()->SetPositionTarget(
                        this->rev1->GetScopedName(), atof(t1));
                    
                    std::cerr << this->model->GetJointController()->SetPositionTarget(
                        this->rev2->GetScopedName(), atof(t2));
                }
                
            }
            // std::cout << "El comando es: " << this->buffer;
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
    };

    // Register this plugin with the simulator
    GZ_REGISTER_MODEL_PLUGIN(ModelPush)
}