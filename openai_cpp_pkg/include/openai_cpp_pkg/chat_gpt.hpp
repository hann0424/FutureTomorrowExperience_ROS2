#ifndef GPT_HPP
#define GPT_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "openai/openai.hpp"
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

class Gpt : public rclcpp::Node {
private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr _sub_question;
    std::string _question_gpt; //gpt에 질문
    std::string _anwser_gpt; //gpt 답변
    void sub_question_msg(const std_msgs::msg::String::SharedPtr msg);
    void gpt(std::string question);
public:
    Gpt();
};


#endif