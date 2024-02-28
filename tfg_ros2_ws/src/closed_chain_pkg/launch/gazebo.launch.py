#This launchfile is in charge of launching a bunch of stuff:
# 1. The path to the urdf model is defined with "model_arg"
# 2. The world path can be defined in the "world" argument or just by writing world:=/usr/share/gazebo-11/worlds/worldName
# 2. After that, the GAZEBO_MODEL_PART environmental variable is set to be the path of the share folder of the package.
# 3. Then the robot_state_publisher is launched
# 4. The gazebo server and client are launched including already made launch files.
# 5. Lastly, the robot is spawned thanks to the executable "spawn_entity.py" from the gazebo_ros package
#    The necessary arguments are passed to it, so it can read from the topic "robot_description"

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument, SetEnvironmentVariable, IncludeLaunchDescription
import os
from ament_index_python.packages import get_package_share_directory, get_package_prefix
from launch_ros.parameter_descriptions import ParameterValue
from launch.substitutions import Command, LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource


def generate_launch_description():

    model_arg = DeclareLaunchArgument(
        name="model",
        default_value=os.path.join(get_package_share_directory("closed_chain_pkg"), "description", "fin_description.sdf"),
        description="Absolute path to robot SDF file"
    )

    world_arg = DeclareLaunchArgument(
        name="world",
        default_value=os.path.join(get_package_share_directory("closed_chain_pkg"), "worlds", "obstacles.world"),
        description="Absolute path to Gazebo world file"
    )


    robot_description = ParameterValue(LaunchConfiguration("model"))

    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description": robot_description}]
    )

    env_var = SetEnvironmentVariable("GAZEBO_MODEL_PATH", os.path.join(get_package_prefix("closed_chain_pkg"), "description", "meshes"))

    start_gazebo_server = IncludeLaunchDescription(PythonLaunchDescriptionSource(
        os.path.join(get_package_share_directory("gazebo_ros"), "launch", "gzserver.launch.py")
        ), launch_arguments={'paused': 'true', 'world': LaunchConfiguration("world")}.items())

    start_gazebo_client = IncludeLaunchDescription(PythonLaunchDescriptionSource(
        os.path.join(get_package_share_directory("gazebo_ros"), "launch", "gzclient.launch.py")
    ))

    spawn_robot = Node(
        package="gazebo_ros",
        executable="spawn_entity.py",
        arguments=["-entity", "fin_mechanism", "-topic", "robot_description"],
        output="screen"
    )

    return LaunchDescription([
        model_arg,
        world_arg,
        env_var,
        robot_state_publisher,
        start_gazebo_client,
        start_gazebo_server,
        spawn_robot
    ])