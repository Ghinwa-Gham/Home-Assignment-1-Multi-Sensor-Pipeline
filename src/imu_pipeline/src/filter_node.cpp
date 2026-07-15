#include <memory>
#include <deque>

#include "rclcpp/rclcpp.hpp"
#include "imu_pipeline/msg/imu_reading.hpp"

class FilterNode : public rclcpp::Node
{
public:
  FilterNode()
: Node("filter_node")
{

    this->declare_parameter<int>("window_size",10);
    window_size_ =this->get_parameter("window_size").as_int();


    auto qos =rclcpp::QoS(rclcpp::KeepLast(20)).reliable();


    publisher_ =this->create_publisher<imu_pipeline::msg::ImuReading>("filtered",qos);


    auto topic_callback =
        [this](const imu_pipeline::msg::ImuReading::SharedPtr msg) -> void
        {
        RCLCPP_INFO(this->get_logger(), "I heard accel_x: '%.3f'", msg->accel_x);
        slide_window(accel_x_buffer_, msg->accel_x);
        slide_window(accel_y_buffer_, msg->accel_y);
        slide_window(accel_z_buffer_, msg->accel_z);

        slide_window(gyro_x_buffer_, msg->gyro_x);
        slide_window(gyro_y_buffer_, msg->gyro_y);
        slide_window(gyro_z_buffer_, msg->gyro_z);

        // Create filtered message Obj to send over the topic filtered
        auto filtered =imu_pipeline::msg::ImuReading();
        
        filtered.accel_x = calculate_average(accel_x_buffer_);
        filtered.accel_y = calculate_average(accel_y_buffer_);
        filtered.accel_z = calculate_average(accel_z_buffer_);

        filtered.gyro_x = calculate_average(gyro_x_buffer_);
        filtered.gyro_y = calculate_average(gyro_y_buffer_);
        filtered.gyro_z = calculate_average(gyro_z_buffer_);

        filtered.stamp = msg->stamp;

        // Publish
        publisher_->publish(filtered);
        RCLCPP_INFO(this->get_logger(), "Filter Node published accel_x: '%.3f'", filtered.accel_x);    
      };




    subscription_ = this->create_subscription<imu_pipeline::msg::ImuReading>("raw",qos,topic_callback);
}
  
private:
  rclcpp::Subscription<imu_pipeline::msg::ImuReading>::SharedPtr subscription_;
  rclcpp::Publisher<imu_pipeline::msg::ImuReading>::SharedPtr publisher_;
  std::deque<double> accel_x_buffer_;
  std::deque<double> accel_y_buffer_;
  std::deque<double> accel_z_buffer_;
  std::deque<double> gyro_x_buffer_;
  std::deque<double> gyro_y_buffer_;
  std::deque<double> gyro_z_buffer_;
  int window_size_;

double calculate_average(const std::deque<double>& buffer)
{
    double sum = 0.0;
    if(buffer.empty())
        return sum;

    for(double value : buffer)
    {
        sum += value;
    }

    return sum / buffer.size();
}

void slide_window(std::deque<double>& buffer,double val)
{
    
    if(buffer.size()>=static_cast<size_t>(this->window_size_))
      buffer.pop_front();

    buffer.push_back(val);
}


};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FilterNode>());
  rclcpp::shutdown();
  return 0;
}



