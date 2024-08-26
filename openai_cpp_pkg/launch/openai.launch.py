import os

from ament_index_python import get_package_share_directory
from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    param_dir = LaunchConfiguration('param_dir', default=os.path.join(
        get_package_share_directory('openai_cpp_pkg'),
        'param',
        'openai.yaml'
        ))
    return LaunchDescription([
        Node(package='openai_cpp_pkg',
             executable='whisper',
             output='screen',
             parameters=[param_dir]
             ),
        Node(package='openai_cpp_pkg',
             executable='bridge',
             output='screen'
             ),
        Node(package='openai_cpp_pkg',
             executable='speaker',
             output='screen',
             parameters=[param_dir]
             ),
        Node(package='openai_py_pkg',
             executable='tts',
             output='screen',
             parameters=[param_dir]
             )
        ])