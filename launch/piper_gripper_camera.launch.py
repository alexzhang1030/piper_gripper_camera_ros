from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument("device_id",    default_value="0"),
        DeclareLaunchArgument("image_width",  default_value="640"),
        DeclareLaunchArgument("image_height", default_value="480"),
        DeclareLaunchArgument("fps",          default_value="30.0"),
        DeclareLaunchArgument("camera_info_url", default_value=""),

        Node(
            package="piper_gripper_camera",
            executable="piper_gripper_camera_node",
            name="piper_gripper_camera",
            output="screen",
            parameters=[{
                "device_id":       LaunchConfiguration("device_id"),
                "image_width":     LaunchConfiguration("image_width"),
                "image_height":    LaunchConfiguration("image_height"),
                "fps":             LaunchConfiguration("fps"),
                "camera_info_url": LaunchConfiguration("camera_info_url"),
            }],
        ),
    ])
