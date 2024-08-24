#ifndef GPT_HPP
#define GPT_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "openai/openai.hpp"
#include "openai_interface/srv/qa_interface.hpp"
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono_literals;

class Gpt : public rclcpp::Node {
private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr _pub;
    rclcpp::Service<openai_interface::srv::QaInterface>::SharedPtr _service;
    std::string _answer;
    std::string _content;
    rclcpp::TimerBase::SharedPtr _timer;
    float _x_val;
    void callback(const openai_interface::srv::QaInterface::Request::SharedPtr request,
        openai_interface::srv::QaInterface::Response::SharedPtr response);

    void gpt(const std::string str);
    void publish_turtlesim_msg();
    
public:
    Gpt();
};


#endif