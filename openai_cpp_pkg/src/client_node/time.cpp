#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <chrono>
#include <memory>

using namespace std::chrono_literals;

class WhisperPublisher : public rclcpp::Node
{
public:
    WhisperPublisher()
        : Node("whisper_publisher_node")
    {
        // 퍼블리셔 설정
        publisher_ = this->create_publisher<std_msgs::msg::String>("whisper_topic", 10);

        // 타이머 설정 (1초마다 publish 수행)
        timer_ = this->create_wall_timer(
            30s, std::bind(&WhisperPublisher::publish_message, this));
    }

private:
    // 메시지를 발행하는 함수
    void publish_message()
    {
        // 메시지 생성 및 내용 설정
        auto message = std_msgs::msg::String();
        message.data = "답변 받는 시간이 초과되었습니다.";

        // 메시지 발행
        publisher_->publish(message);
    }

    // 퍼블리셔와 타이머 선언
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<WhisperPublisher>());
    rclcpp::shutdown();
    return 0;
}
