from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

package_dir = get_package_share_directory("imu_pipeline")
config_file = os.path.join( package_dir,"config","imu_params.yaml")

#Create Nodes
sensor_driver = Node(
    package="imu_pipeline",
    executable="sensor_driver",
    name="sensor_driver",
    namespace="imu",
    parameters=[config_file]
)
filter_node = Node(

    package="imu_pipeline",
    executable="filter_node",
    name="filter_node",
    namespace="imu",
    parameters=[config_file]
)

logger_node = Node(

    package="imu_pipeline",
    executable="logger_node",
    name="logger_node",
    namespace="imu"
)

def generate_launch_description():
    return LaunchDescription([
        sensor_driver,
        filter_node,
        logger_node
    ])
