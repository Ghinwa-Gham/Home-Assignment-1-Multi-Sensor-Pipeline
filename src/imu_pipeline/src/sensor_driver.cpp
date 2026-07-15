#include <chrono>
#include <memory>
#include <string>
#include <cmath>
#include <stdlib.h>


#include "rclcpp/rclcpp.hpp"
#include "imu_pipeline/msg/imu_reading.hpp"

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses a fancy C++11 lambda
* function to shorten the callback syntax, at the expense of making the
* code somewhat more difficult to understand at first glance. */

class SensorDriver : public rclcpp::Node
{
public:
//Constructor use list initializer to call the parent class constructor passing a name for the node
//
  SensorDriver()
  : Node("sensnor_driver"), t_(0.0)
  {
    //calling the create_publisher() passing the topic and buffer size as parameters
    //the publisher sends messages of type ImuReading
    auto qos =rclcpp::QoS(rclcpp::KeepLast(20)).reliable();
    publisher_ = this->create_publisher<imu_pipeline::msg::ImuReading>("raw",qos);

    //writing a lambda function and palcing it inside the variable timer_callback (so it can be referenced 
    //to be called later on based on events)
    auto timer_callback =
      [this]() -> void {
        //creates the message object
        auto message = imu_pipeline::msg::ImuReading();

        //getClock() a node helper function that retrieves a pointer to the node's system or simulation clock.
        //now() a clock method that captures the exact current time
        message.stamp = this->get_clock()->now();

        //sinusoid on the accel fields 
        message.accel_x = std::sin(this->t_);
        message.accel_y = std::sin(this->t_ + 2.0); //added offset to have different sin wave for each dimension
        message.accel_z = std::sin(this->t_ + 3.5);
        this->t_+= 1.0/publish_rate_hz_; // to esnure that the sin changes progressively
        std::cout << "t = " << t_ << std::endl;

        //random gyro noise
        //std::rand() generate an int whose max guaranteed value (since it depends on the compiler is )
        message.gyro_x = (std::rand() % 1000) / 1000.0;
        message.gyro_y = (std::rand() % 1000) / 1000.0;
        message.gyro_z = (std::rand() % 1000) / 1000.0;

        //Prinitng on a console a value for debugging purposes
        RCLCPP_INFO(this->get_logger(), "Publishing accel_x: %.3f", message.accel_x);

        //publish the message
        this->publisher_->publish(message);
      };



    //Publish Rate (not hardcoded can be passed as a parameter if not default value is 50Hz)
    this->declare_parameter<int>("publish_rate_hz", 50);
    //Get the publish rate value
    publish_rate_hz_ =get_parameter("publish_rate_hz").as_int();
    //Convert from Hz to ms
    auto period = std::chrono::milliseconds(static_cast<int>(1000.0 / publish_rate_hz_));
    //Create the timer
    timer_ = this->create_wall_timer(period, timer_callback);
    RCLCPP_INFO(this->get_logger(), "Sesnor driver started at %d Hz", publish_rate_hz_);
  }

private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<imu_pipeline::msg::ImuReading>::SharedPtr publisher_;
  double t_;
  int publish_rate_hz_;

};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SensorDriver>());
  rclcpp::shutdown();
  return 0;
}