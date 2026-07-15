cmake_minimum_required(VERSION 3.8)

project(imu_pipeline)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rclcpp REQUIRED)

# Generate custom messages
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/ImuReading.msg"
  DEPENDENCIES builtin_interfaces
)
add_executable(sensor_driver src/sensor_driver.cpp)
add_executable(filter_node src/filter_node.cpp)
add_executable(logger_node src/logger_node.cpp)

ament_target_dependencies(sensor_driver rclcpp)
ament_target_dependencies(filter_node rclcpp)
ament_target_dependencies(logger_node rclcpp)

rosidl_target_interfaces(sensor_driver ${PROJECT_NAME} "rosidl_typesupport_cpp")
rosidl_target_interfaces(filter_node ${PROJECT_NAME} "rosidl_typesupport_cpp")
rosidl_target_interfaces(logger_node ${PROJECT_NAME} "rosidl_typesupport_cpp")

install(TARGETS sensor_driver filter_node logger_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)

  find_package(ament_lint_auto REQUIRED)

  set(ament_cmake_copyright_FOUND TRUE)

  set(ament_cmake_cpplint_FOUND TRUE)

  ament_lint_auto_find_test_dependencies()

endif()


ament_package()