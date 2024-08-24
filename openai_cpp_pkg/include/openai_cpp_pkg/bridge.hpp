#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "openai/openai.hpp"
#include "openai_interface/srv/qa_interface.hpp"
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono_literals;

class Bridge : public rclcpp::Node {
private:
    rclcpp::Service<openai_interface::srv::QaInterface>::SharedPtr _service_whisper; //whisper에 request에 response
    rclcpp::Client<openai_interface::srv::QaInterface>::SharedPtr _client_gpt;
    rclcpp::Client<openai_interface::srv::QaInterface>::SharedPtr _client_tts;
    rclcpp::Client<openai_interface::srv::QaInterface>::SharedPtr _client_speaker;

    std::string _gpt_question;
    std::string _gpt_answer;
    std::string _tts_answer;
    //bool _status;

    void send_request_gpt();
    void response_callback_gpt_thread(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future);
    void response_callback_gpt(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future);
    void send_request_tts();
    void response_callback_tts(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future);
    void send_request_speaker();
    void response_callback_speaker(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future);
    void callback_whisper(const openai_interface::srv::QaInterface::Request::SharedPtr request,
        openai_interface::srv::QaInterface::Response::SharedPtr response);

public:
    Bridge();
};

#endif