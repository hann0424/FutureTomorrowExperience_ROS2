#ifndef GPT_HPP
#define GPT_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "openai/openai.hpp"
#include "openai_interface/srv/qa_interface.hpp"
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

class Gpt : public rclcpp::Node {
private:
    rclcpp::Service<openai_interface::srv::QaInterface>::SharedPtr _service;
    std::string _answer;
    void callback(const openai_interface::srv::QaInterface::Request::SharedPtr request,
    openai_interface::srv::QaInterface::Response::SharedPtr response);

    void gpt(const std::string str);
    
public:
    Gpt();
};


#endif