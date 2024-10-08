#include "openai_cpp_pkg/chat_gpt.hpp"

Gpt::Gpt()
    : Node("gpt_node")
{
    auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10));
    _pub = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", qos_profile);

    _service = this->create_service<openai_interface::srv::QaInterface>("gpt",
        std::bind(&Gpt::callback, this, std::placeholders::_1, std::placeholders::_2));

    std::string _content = "You are a chatbot that classifies responses. For every question, respond with a JSON object that includes the category and answer. - If the question is about general knowledge, use 'category: general'. - If the question is a greeting, use 'category: greeting'. - If the question relates to robot movement (like moving forward, turning, etc.), use 'category: movement'.";

    _x_val = 0.0;
    _movement_active = false;  // 추가: 움직임 상태 플래그
}

void Gpt::callback(const openai_interface::srv::QaInterface::Request::SharedPtr request,
    openai_interface::srv::QaInterface::Response::SharedPtr response)
{
    gpt(request->question);

    std::string keyword = "movement";
    std::string str = _answer;

    if (str.find(keyword) != std::string::npos) {
        std::cout << "The string contains '" << keyword << "'." << std::endl;

        size_t x_pos = str.find("x:");
        if (x_pos != std::string::npos) {
            size_t start = x_pos + 2;
            size_t end = str.find("}", start);

            std::string x_value_str = str.substr(start, end - start);
            x_value_str.erase(0, x_value_str.find_first_not_of(" \t"));
            x_value_str.erase(x_value_str.find_last_not_of(" \t") + 1);

            _x_val = std::stof(x_value_str);
            std::cout << "Extracted value of x: " << _x_val << std::endl;

            // 움직임 시작
            if (!_movement_active) {
                start_movement();
            }
        } else {
            std::cout << "x: not found in the string." << std::endl;
        }

        response->answer = keyword;
    } else {
        std::cout << "The string does not contain '" << keyword << "'." << std::endl;
        response->answer = _answer;
    }
}

void Gpt::gpt(const std::string str)
{
    nlohmann::json jsonObj = R"(
    {
        "model": "ft:gpt-3.5-turbo-0125:personal::A0HYXmGw",
        "max_tokens": 1000,
        "temperature": 0
    }
    )"_json;

    jsonObj["messages"].push_back({
        {"role", "system"},
        {"content", _content},
        {"role", "user"},
        {"content", str}
    });

    openai::start();
    auto chat = openai::chat().create(jsonObj);

    try {
        std::string str = chat.dump();
        nlohmann::json json_result = nlohmann::json::parse(str);
        std::string content = json_result["choices"][0]["message"]["content"];
        std::cout << "content: " << content << std::endl;
        _answer = content;
    } catch (const std::exception& e) {
        std::cerr << "JSON 파싱 에러: " << e.what() << std::endl;
    }
}

void Gpt::start_movement() {
    _movement_active = true;
    _timer = this->create_wall_timer(
        100ms, std::bind(&Gpt::publish_turtlesim_msg, this));

    // 5초 후에 움직임을 중지하도록 타이머 설정
    _stop_timer = this->create_wall_timer(
        5s, [this]() {
            stop_movement();
        });
}

void Gpt::stop_movement() {
    _movement_active = false;
    _timer->cancel();  // 움직임을 중지하기 위해 타이머 중지
    _x_val = 0.0;  // 움직임을 중지하기 위해 속도 0 설정
    publish_turtlesim_msg();  // 속도 0으로 메시지 발행
    std::cout << "Movement stopped after 5 seconds." << std::endl;
}

void Gpt::publish_turtlesim_msg()
{
    auto msg = geometry_msgs::msg::Twist();
    msg.linear.x = _x_val;

    _pub->publish(msg);
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Gpt>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
