#include <gazebo-11/gazebo/gazebo.hh>
#include <functional>
#include <gazebo-11/gazebo/physics/physics.hh>
#include <gazebo-11/gazebo/common/common.hh>
#include <ignition/math6/ignition/math/Vector3.hh>
#include <thread>


namespace gazebo
{
  class ModelPush : public ModelPlugin
  {
    public: void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
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
    public: void OnUpdate()
    {
      // Apply a small linear velocity to the model.
      // this->model->SetLinearVel(ignition::math::Vector3d(.3, 0, 0));

      // Apply a small force to the model. This way the gravity is still working
      // this->baseLink->AddForce(ignition::math::Vector3d(100, 0, 0));

      std::system("clear");
      std::cerr << this->rev1->GetName() << " position is: " << this->rev1->Position(0) << std::endl;
      std::cerr << this->rev2->GetName() << " position is: " << this->rev2->Position(0) << std::endl;
      
    }

    // Pointer to the model
    private: physics::ModelPtr model;

    // Pointer to the base link
    private: physics::LinkPtr baseLink;

    //Pointers to the joints
    private: physics::JointPtr rev1;
    private: physics::JointPtr rev2;

    /// A PID controller for the joint.
    private: common::PID pid1;
    private: common::PID pid2;

    // Pointer to the update event connection
    private: event::ConnectionPtr updateConnection;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(ModelPush)
}