#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class TextSubscriber : public rclcpp::Node
{
public:
    TextSubscriber()
        : Node("bridge_answer_node")
    {
        // "pub_answer" 토픽 구독
        auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10));
        _subscription = this->create_subscription<std_msgs::msg::String>(
            "pub_answer", qos_profile,
            std::bind(&TextSubscriber::topic_callback, this, std::placeholders::_1));

        _publisher = this->create_publisher<std_msgs::msg::String>("pub_tts", qos_profile);
    }

private:
    // 콜백 함수: 퍼블리셔로부터 받은 메시지를 처리
    void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
    {
        RCLCPP_INFO(this->get_logger(), "받은 질문: %s", msg->data.c_str());

        std_msgs::msg::String new_msg;
        new_msg.data = msg->data;
        _publisher->publish(new_msg);

    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr _subscription;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr _publisher;
};

int main(int argc, char **argv)
{
    // ROS2 초기화
    rclcpp::init(argc, argv);

    // 노드 실행
    rclcpp::spin(std::make_shared<TextSubscriber>());

    // ROS2 종료
    rclcpp::shutdown();
    return 0;
}
