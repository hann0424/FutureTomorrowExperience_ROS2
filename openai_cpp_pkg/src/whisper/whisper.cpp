#include "openai_cpp_pkg/whisper.hpp"

Whisper::Whisper()
    : Node("whisper_node")
{
     auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10));
    _question_pub = this->create_publisher<std_msgs::msg::String>("pub_question", qos_profile);
    _timer = this->create_wall_timer(1s, std::bind(&Whisper::publish_question, this));
}

void Whisper::publish_question()
{
    openai_whisper();
    _question_pub->publish(_question_msg);
}

void Whisper::openai_whisper()
{

    openai::start();
    {
        auto transcription = openai::audio().transcribe(R"(
            {
            "file": "/home/jty6109/robot_ws/src/FutureTomorrowExperience_ROS2/openai_cpp_pkg/audio/question.mp3",
            "model": "whisper-1"
            }
        )"_json);
        _question_msg.data = transcription["text"];
    }
}