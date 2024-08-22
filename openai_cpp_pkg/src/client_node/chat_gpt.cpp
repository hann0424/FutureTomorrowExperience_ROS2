#include "openai_cpp_pkg/chat_gpt.hpp"


Gpt::Gpt()
    : Node("gpt_node")
{
    _service = this->create_service<openai_interface::srv::QaInterface>("gpt",
        std::bind(&Gpt::callback, this, std::placeholders::_1, std::placeholders::_2));
}

void Gpt::callback(const openai_interface::srv::QaInterface::Request::SharedPtr request,
    openai_interface::srv::QaInterface::Response::SharedPtr response)
{
    gpt(request->question);
    response->answer = _answer;
}

void Gpt::gpt(const std::string str)
{
    nlohmann::json jsonObj = R"(
    {
        "model": "gpt-4o",
        "max_tokens": 1000,
        "temperature": 0
    }
    )"_json;

    jsonObj["messages"].push_back({
        {"role", "user"},
        {"content", str}
    });

    openai::start();
    auto chat = openai::chat().create(jsonObj);  // API 호출의 반환 값 확인 필요

    // API 결과를 JSON으로 파싱
    try {
        std::string str = chat.dump();
        nlohmann::json json_result = nlohmann::json::parse(str);  // chat이 JSON 문자열인지 확인 필요
        std::string content = json_result["choices"][0]["message"]["content"];
        std::cout << "content: " << content << std::endl;
        _answer = content;
    } catch (const std::exception& e) {
        std::cerr << "JSON 파싱 에러: " << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Gpt>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
