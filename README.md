# IMU Pipeline Package

A ROS 2 Jazzy package implementing a multi-sensor processing pipeline:

```
sensor_driver  →  /imu/raw  →  filter_node  →  /imu/filtered  →  logger_node
```

The package contains three nodes:

- `sensor_driver`: Publishes simulated IMU data.
- `filter_node`: Applies a moving average filter.
- `logger_node`: Prints the difference between raw and filtered acceleration values.

---

# Build Instructions

## 1. Go to your ROS 2 workspace

```bash
cd ~/ros2_ws
```

## 2. Build the package

```bash
colcon build
```

## 3. Source the workspace

After building, source the generated setup file:

```bash
source install/setup.bash
```

---

# Running the Pipeline

There are two ways to run the package.

---

# Method 1: Running with the Launch File (Recommended)

The launch file starts all three nodes automatically:

- `sensor_driver`
- `filter_node`
- `logger_node`

Run:

```bash
ros2 launch imu_pipeline imu_pipeline.launch.py
```

The launch file automatically:

- Loads parameters from the YAML configuration file.
- Starts all nodes.
- Applies the `/imu` namespace.

The resulting topics are:

```
/imu/raw

/imu/filtered
```

---

# Method 2: Running Nodes Manually (Without Launch File)

Each node can also be started individually.

Open separate terminals and source the workspace in each one:

```bash
source install/setup.bash
```

---

## Start the sensor driver

```bash
ros2 run imu_pipeline sensor_driver \
--ros-args \
--params-file src/imu_pipeline/config/imu_params.yaml
```

---

## Start the filter node

```bash
ros2 run imu_pipeline filter_node \
--ros-args \
--params-file src/imu_pipeline/config/imu_params.yaml
```

---

## Start the logger node

```bash
ros2 run imu_pipeline logger_node
```

---
Note that you must run the following command each time you open a new terminal
```bash
source /opt/ros/jazzy/setup.bash
```

