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
    rclcpp::Client<openai_interface::srv::QaInterface>::SharedPtr _client_bridge;
    rclcpp::Service<openai_interface::srv::QaInterface>::SharedPtr _service_mic;
    //rclcpp::TimerBase::SharedPtr _timer;
    std::string _question_msg;
    std::string _response_bridge;
    bool _status;
    void callback_mic(const openai_interface::srv::QaInterface::Request::SharedPtr request,
        openai_interface::srv::QaInterface::Response::SharedPtr response);
    void response_callback_whisper_thread(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future);
    void send_request();
    void openai_whisper();
    void response_callback(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future);
public:
    Whisper();
};

#endif