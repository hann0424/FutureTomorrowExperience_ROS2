#include "openai_cpp_pkg/chat_gpt.hpp"

Gpt::Gpt()
    : Node("gpt_node")
{
    auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10));
    _sub_question = this->create_subscription<std_msgs::msg::String>("pub_question", qos_profile,
        std::bind(&Gpt::sub_question_msg, this, std::placeholders::_1));
}

void Gpt::sub_question_msg(const std_msgs::msg::String::SharedPtr msg)
{
    _question_gpt = msg->data;
    gpt(_question_gpt);

}

void Gpt::gpt(std::string question)
{
    nlohmann::json jsonObj = R"(
    {
        "model": "gpt-4o",
        "max_tokens": 7,
        "temperature": 0
    }
    )"_json;

    jsonObj["messages"].push_back({
        {"role", "user"},
        {"content", question}
    });

    openai::start();
    auto chat = openai::chat().create(jsonObj);  // API 호출의 반환 값 확인 필요

    // API 결과를 JSON으로 파싱
    try {
        std::string str = chat.dump();
        nlohmann::json json_result = nlohmann::json::parse(str);  // chat이 JSON 문자열인지 확인 필요
        std::string content = json_result["choices"][0]["message"]["content"];
        std::cout << "content: " << content << std::endl;
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
