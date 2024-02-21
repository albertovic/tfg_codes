from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument, SetEnvironmentVariable, IncludeLaunchDescription
import os
from ament_index_python.packages import get_package_share_directory, get_package_prefix
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command, LaunchConfiguration

def generate_launch_description():

    model_arg = DeclareLaunchArgument(
        name="model",
        default_value=os.path.join(get_package_share_directory("closed_chain_pkg"), "description", "simple_closed_chain.sdf")
    )

    world_arg = DeclareLaunchArgument(
        name="world",
        default_value=os.path.join(get_package_share_directory("rona_robot"), "worlds", "obstacles.world"),
        description="Absolute path to Gazebo world file"
    )

    env_var = SetEnvironmentVariable("GAZEBO_MODEL_PATH", os.path.join(get_package_prefix("closed_chain_pkg"), "share"))

    start_gazebo_server = IncludeLaunchDescription(PythonLaunchDescriptionSource(
        os.path.join(get_package_share_directory("gazebo_ros"), "launch", "gzserver.launch.py")
        ), launch_arguments={'paused': 'true', 'world': LaunchConfiguration("world")}.items())
    
    start_gazebo_client = IncludeLaunchDescription(PythonLaunchDescriptionSource(
        os.path.join(get_package_share_directory("gazebo_ros"), "launch", "gzclient.launch.py")
    ))

    spawn_robot = Node(
        package="gazebo_ros",
        executable="spawn_entity.py",
        arguments=["-entity", "closed_chain_pkg", "-topic", "robot_description"],
        output="screen"
    )

    return LaunchDescription([
        model_arg,
        world_arg,
        env_var,
        start_gazebo_client,
        start_gazebo_server,
        spawn_robot
    ])
