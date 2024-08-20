#include "openai_cpp_pkg/whisper.hpp"


using namespace std::chrono_literals;

Whisper::Whisper()
    : Node("whisper_node")
{
    client_ = this->create_client<openai_interface::srv::QaInterface>("question");

    // 타이머 설정 (15초마다 서비스 요청)
    _timer = this->create_wall_timer(5s, std::bind(&Whisper::send_request, this));
}

void Whisper::send_request()
{
    openai_whisper();

    // 서비스 요청 객체 생성 (QaInterface 사용)
    auto request = std::make_shared<openai_interface::srv::QaInterface::Request>();
    request->question = _question_msg.data;  // 요청 필드에 전사된 텍스트 저장

    // 서비스가 준비될 때까지 기다림
    if (!client_->wait_for_service(15s)) {
        RCLCPP_INFO(this->get_logger(), "서비스를 기다리는 중...");
        return;
    }

    // 비동기 방식으로 서비스 요청 전송
    auto result_future = client_->async_send_request(
        request, std::bind(&Whisper::response_callback, this,
        std::placeholders::_1));
}



void Whisper::response_callback(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future)
{
    try {
        auto result = future.get();  // 응답 대기
        RCLCPP_INFO(get_logger(), "응답 메시지: %s", result->answer.c_str());  // 응답 필드 사용
    } catch (const std::exception &e) {
        RCLCPP_ERROR(get_logger(), "서비스 요청 중 오류 발생: %s", e.what());
    }
}

void Whisper::openai_whisper()
{
    openai::start();
    auto transcription = openai::audio().transcribe(R"(
        {
        "file": "/home/jty6109/robot_ws/src/FutureTomorrowExperience_ROS2/openai_cpp_pkg/audio/question.mp3",
        "model": "whisper-1",
        "prompt": "question"
        }
    )"_json);

    _question_msg.data = transcription["text"];
}
