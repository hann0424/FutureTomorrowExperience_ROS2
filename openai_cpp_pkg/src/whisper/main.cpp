#include "openai_cpp_pkg/whisper.hpp"

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Whisper>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
