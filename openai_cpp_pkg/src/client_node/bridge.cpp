#include "openai_cpp_pkg/bridge.hpp"

Bridge::Bridge()
    : Node("bdirdge_node")
{
    _service_whisper = this->create_service<openai_interface::srv::QaInterface>("bridge",
        std::bind(&Bridge::callback_whisper, this, std::placeholders::_1, std::placeholders::_2));
    _client_gpt = this->create_client<openai_interface::srv::QaInterface>("gpt");
    _client_tts = this->create_client<openai_interface::srv::QaInterface>("tts");

    _status = false;
}

void Bridge::callback_whisper(const openai_interface::srv::QaInterface::Request::SharedPtr request,
    openai_interface::srv::QaInterface::Response::SharedPtr response) {

    RCLCPP_INFO(get_logger(), "request bridge");
    _gpt_question = request->question;
    send_request_gpt();
    response->answer = _tts_answer;
}

void Bridge::response_callback_gpt_thread(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future)
{
    RCLCPP_INFO(get_logger(), "open thread");
    std::thread{std::bind(&Bridge::response_callback_gpt, this, std::placeholders::_1), future}.detach();
}

void Bridge::send_request_gpt()
{
    _status = true;
    auto request = std::make_shared<openai_interface::srv::QaInterface::Request>();
    request->question = _gpt_question;

    if (!_client_gpt->wait_for_service(15s)) {
        RCLCPP_INFO(this->get_logger(), "서비스를 기다리는 중...");
        return;
    }

    auto result_future = _client_gpt->async_send_request(
        request, std::bind(&Bridge::response_callback_gpt_thread, this,
        std::placeholders::_1));
}

void Bridge::response_callback_gpt(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future)
{
    try {
        RCLCPP_INFO(get_logger(), "bridge test");
        auto result = future.get();  // 응답 대기
        _gpt_answer = result->answer;
        RCLCPP_INFO(get_logger(), "응답 메시지: %s", result->answer.c_str());  // 응답 필드 사용
    } catch (const std::exception &e) {
        RCLCPP_ERROR(get_logger(), "서비스 요청 중 오류 발생: %s", e.what());
    }
    send_request_tts();
}

void Bridge::send_request_tts()
{
    _status = true;
    auto request = std::make_shared<openai_interface::srv::QaInterface::Request>();
    request->question = _gpt_answer;

    if (!_client_tts->wait_for_service(15s)) {
        RCLCPP_INFO(this->get_logger(), "서비스를 기다리는 중...");
        return;
    }

    auto result_future = _client_tts->async_send_request(
        request, std::bind(&Bridge::response_callback_tts, this,
        std::placeholders::_1));
}

void Bridge::response_callback_tts(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future)
{
    try {
        auto result = future.get();  // 응답 대기
        _tts_answer = result->answer;
        RCLCPP_INFO(get_logger(), "응답 메시지: %s", result->answer.c_str());  // 응답 필드 사용
    } catch (const std::exception &e) {
        RCLCPP_ERROR(get_logger(), "서비스 요청 중 오류 발생: %s", e.what());
    }
    _status = false;
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Bridge>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
