#include "openai_cpp_pkg/whisper.hpp"

Whisper::Whisper()
    : Node("whisper_node")
{
    std::cout << "1" << std::endl;
    openai_whisper();
}

void Whisper::publish_question()
{

}

void Whisper::openai_whisper()
{

    openai::start();
    std::cout << "2" << std::endl;
    {
        auto transcription = openai::audio().transcribe(R"(
            {
            "file": "/home/jty6109/robot_ws/src/FutureTomorrowExperience_ROS2/openai_cpp_pkg/audio/question.mp3",
            "model": "whisper-1"
            }
        )"_json);
        std::cout << "3" << std::endl;
        std::cout << "Response is:\n" << transcription.dump(2) << '\n';
    }
}