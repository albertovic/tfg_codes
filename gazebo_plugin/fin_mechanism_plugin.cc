#include <gazebo-11/gazebo/gazebo.hh>
#include <functional>
#include <gazebo-11/gazebo/physics/physics.hh>
#include <gazebo-11/gazebo/common/common.hh>
#include <ignition/math6/ignition/math/Vector3.hh>

namespace gazebo
{
  class ModelPush : public ModelPlugin
  {
    public: void Load(physics::ModelPtr _model, sdf::ElementPtr /*_sdf*/)
    {
      // Store the pointer to the model
      this->model = _model;
      this->baseLink = model->GetLinks()[0];

      this->rev1 = model->GetJoints()[0];
      this->rev2 = model->GetJoints()[2];
      this->rev2_2 = model->GetJoints()[4];

      std::cerr << "\nThe plugin is attached to model[" << this->model->GetName() << "]\n";

      std::cerr << "The links are called: " << this->rev1->GetName() << ", " << this->rev2->GetName() << " and " << this->rev2_2->GetName() << std::endl;

      // Listen to the update event. This event is broadcast every
      // simulation iteration.
      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          std::bind(&ModelPush::OnUpdate, this));
    }

    // Called by the world update start event
    public: void OnUpdate()
    {
      // Apply a small linear velocity to the model.
      // this->model->SetLinearVel(ignition::math::Vector3d(.3, 0, 0));

      // Apply a small force to the model. This way the gravity is still working
      // this->baseLink->AddForce(ignition::math::Vector3d(100, 0, 0));

      std::cerr << this->rev1->GetName() << " position is: " << this->rev1->Position(0) << std::endl;
      std::cerr << this->rev2->GetName() << " position is: " << this->rev2->Position(0) << std::endl;
      std::cerr << this->rev2_2->GetName() << " position is: " << this->rev2_2->Position(0) << std::endl;
    }

    // Pointer to the model
    private: physics::ModelPtr model;

    // Pointer to the base link
    private: physics::LinkPtr baseLink;

    //Pointers to the joints
    private: physics::JointPtr rev1;
    private: physics::JointPtr rev2;
    private: physics::JointPtr rev2_2;

    // Pointer to the update event connection
    private: event::ConnectionPtr updateConnection;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(ModelPush)
}