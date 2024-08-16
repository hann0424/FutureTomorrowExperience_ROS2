#ifndef WHISPER_HPP
#define WHISPER_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "openai/openai.hpp"
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

class Whisper : public rclcpp::Node
{
private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr _question_pub;
    rclcpp::TimerBase::SharedPtr _timer;
    std_msgs::msg::String _question_msg;
    void publish_question();
    void openai_whisper();
public:
    Whisper();
};

#endif