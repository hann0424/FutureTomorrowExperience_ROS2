#ifndef SPEAKER_HPP
#define SPEAKER_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "openai/openai.hpp"
#include "openai_interface/srv/qa_interface.hpp"
#include <cstdlib>
#include <sys/stat.h>
#include <cerrno>
#include <chrono>
#include <SFML/Audio.hpp>


using namespace std;
using namespace std::chrono_literals;

class Speaker : public rclcpp::Node
{
public:
    Speaker();
private:
    rclcpp::Service<openai_interface::srv::QaInterface>::SharedPtr _service_speaker;
    void callback_speaker(const openai_interface::srv::QaInterface::Request::SharedPtr request,
        openai_interface::srv::QaInterface::Response::SharedPtr response);
};

#endif