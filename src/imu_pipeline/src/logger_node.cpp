#include <memory>
#include <functional>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "imu_pipeline/msg/imu_reading.hpp"

using namespace std::chrono_literals;

class LoggerNode : public rclcpp::Node
{
public:
    LoggerNode()
    : Node("logger_node")
    {
        // QoS: Reliable, depth 20
        auto qos = rclcpp::QoS(rclcpp::KeepLast(20)).reliable();

        // Subscribe to raw IMU data
        raw_subscription_ =
            this->create_subscription<imu_pipeline::msg::ImuReading>(
                "raw",
                qos,
                [this](const imu_pipeline::msg::ImuReading::SharedPtr msg)
                {
                    latest_raw_ = msg;
                });

        // Subscribe to filtered IMU data
        filtered_subscription_ =
            this->create_subscription<imu_pipeline::msg::ImuReading>(
                "filtered",
                qos,
                [this](const imu_pipeline::msg::ImuReading::SharedPtr msg)
                {
                    latest_filtered_ = msg;
                });

        // Timer that prints once every second
        timer_ = this->create_wall_timer(1s,[this]() { this->log_values(); });

        RCLCPP_INFO(this->get_logger(), "Logger node started.");
    }

private:
    void log_values()
    {
        // Wait until we've received at least one message from each topic
        if (!latest_raw_ || !latest_filtered_)
        {
            RCLCPP_INFO(this->get_logger(), "Waiting for messages...");
            return;
        }

        double difference =
            latest_raw_->accel_x - latest_filtered_->accel_x;

        RCLCPP_INFO(
            this->get_logger(),
            "Raw accel_x: %.3f | Filtered accel_x: %.3f | Difference: %.3f",
            latest_raw_->accel_x,
            latest_filtered_->accel_x,
            difference);
    }

    // Subscribers
    rclcpp::Subscription<imu_pipeline::msg::ImuReading>::SharedPtr raw_subscription_;
    rclcpp::Subscription<imu_pipeline::msg::ImuReading>::SharedPtr filtered_subscription_;

    // Latest received messages
    imu_pipeline::msg::ImuReading::SharedPtr latest_raw_;
    imu_pipeline::msg::ImuReading::SharedPtr latest_filtered_;

    // 1 Hz timer
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<LoggerNode>());

    rclcpp::shutdown();

    return 0;
}