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
    rclcpp::TimerBase::SharedPtr _timer;
    rclcpp::TimerBase::SharedPtr _stop_timer; // 5초 후 움직임을 멈추기 위한 타이머

    std::string _answer;
    std::string _content;
    float _x_val;
    bool _movement_active; // 현재 움직임이 활성화되었는지 확인하는 플래그

    void callback(const openai_interface::srv::QaInterface::Request::SharedPtr request,
                  openai_interface::srv::QaInterface::Response::SharedPtr response);

    void gpt(const std::string str);
    void publish_turtlesim_msg();
    void start_movement(); // 5초 동안 움직임을 시작하는 함수
    void stop_movement();  // 움직임을 멈추는 함수

public:
    Gpt();
};

#endif // GPT_HPP
