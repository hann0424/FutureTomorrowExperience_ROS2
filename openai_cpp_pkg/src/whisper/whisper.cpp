#include "openai_cpp_pkg/whisper.hpp"

using namespace std::chrono_literals;

Whisper::Whisper()
    : Node("whisper_node")
{
    _client_bridge = this->create_client<openai_interface::srv::QaInterface>("bridge");
    _service_mic = this->create_service<openai_interface::srv::QaInterface>("mic",
        std::bind(&Whisper::callback_mic, this, std::placeholders::_1, std::placeholders::_2));
    status = false;
    // 타이머 설정 (15초마다 서비스 요청)
    //_timer = this->create_wall_timer(5s, std::bind(&Whisper::send_request, this));
}

void Whisper::send_request()
{
    openai_whisper();

    // 서비스 요청 객체 생성 (QaInterface 사용)
    auto request = std::make_shared<openai_interface::srv::QaInterface::Request>();
    request->question = _question_msg.data;  // 요청 필드에 전사된 텍스트 저장

    // 서비스가 준비될 때까지 기다림
    if (!_client_bridge->wait_for_service(15s)) {
        RCLCPP_INFO(this->get_logger(), "서비스를 기다리는 중...");
        return;
    }

    // 비동기 방식으로 서비스 요청 전송
    auto result_future = _client_bridge->async_send_request(
        request, std::bind(&Whisper::response_callback, this,
        std::placeholders::_1));
}



void Whisper::response_callback(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future)
{
    try {
        auto result = future.get();  // 응답 대기
        _response_bridge = result->answer;
        RCLCPP_INFO(get_logger(), "응답 메시지: %s", result->answer.c_str());  // 응답 필드 사용
    } catch (const std::exception &e) {
        RCLCPP_ERROR(get_logger(), "서비스 요청 중 오류 발생: %s", e.what());
    }
}

void Whisper::callback_mic(const openai_interface::srv::QaInterface::Request::SharedPtr request,
    openai_interface::srv::QaInterface::Response::SharedPtr response)
{
    status = true;
    send_request();
    response->answer = _response_bridge;
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
