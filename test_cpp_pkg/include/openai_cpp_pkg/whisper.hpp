#ifndef WHISPER_HPP
#define WHISPER_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "openai/openai.hpp"
#include "openai_interface/srv/qa_interface.hpp"
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

class Whisper : public rclcpp::Node
{
private:
    rclcpp::Client<openai_interface::srv::QaInterface>::SharedPtr client_;
    rclcpp::TimerBase::SharedPtr _timer;
    std_msgs::msg::String _question_msg;
    void send_request();
    void openai_whisper();
    void response_callback(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future);
public:
    Whisper();
    ~Whisper();
};

#endif