#include "openai_cpp_pkg/speaker.hpp"

Speaker::Speaker()
    : Node("speaker_node")
{
    _service_speaker = this->create_service<openai_interface::srv::QaInterface>("speaker",
        std::bind(&Speaker::callback_speaker, this, std::placeholders::_1, std::placeholders::_2));
}

void Speaker::callback_speaker(const openai_interface::srv::QaInterface::Request::SharedPtr request,
        openai_interface::srv::QaInterface::Response::SharedPtr response)
{
    const char* filePath = "/home/ubuntu/robot_ws/src/FutureTomorrowExperience_ROS2/result_audio/answer.wav";
    const char* command = "ffmpeg -y -i /home/ubuntu/robot_ws/src/FutureTomorrowExperience_ROS2/result_audio/answer.wav -acodec pcm_s16le -ar 44100 -ac 2 answer2.wav";
    
    if (chmod(filePath, S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
        std::cerr << "Error changing file permissions: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Permissions changed successfully for " << filePath << std::endl;
    }

    int result = system(command);
    RCLCPP_INFO(get_logger(), "%d", result);

    if (request->question == "answer recorded complete") {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile("/home/ubuntu/robot_ws/src/FutureTomorrowExperience_ROS2/result_audio/answer2.wav")) {
            std::cerr << "Error loading audio file!" << std::endl;
        }
        //buffer.loadFromFile("/home/jty6109/robot_ws/src/FutureTomorrowExperience_ROS2/result_audio/answer.wav");

        // 음성 재생 객체 생성
        sf::Sound sound;
        sound.setBuffer(buffer);

        // 음성 재생
        sound.play();

        // 음성이 끝날 때까지 대기
        while (sound.getStatus() == sf::Sound::Playing) {
            sf::sleep(sf::milliseconds(100));  // 짧게 대기
        }
    }
    response->answer = "audio play done";
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Speaker>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
