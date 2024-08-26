cmake_minimum_required(VERSION 3.8)
project(openai_cpp_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(CURL REQUIRED)
find_package(openai_interface REQUIRED)
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

include_directories(include)
include_directories(audio)



add_executable(whisper src/whisper/whisper.cpp src/whisper/main.cpp)
ament_target_dependencies(whisper rclcpp std_msgs openai_interface CURL)

add_executable(question_bridge src/robot_node/question_bridge.cpp)
ament_target_dependencies(question_bridge rclcpp std_msgs )

add_executable(answer_bridge src/robot_node/answer_bridge.cpp)
ament_target_dependencies(answer_bridge rclcpp std_msgs )

add_executable(chat_gpt src/client_node/chat_gpt.cpp)
ament_target_dependencies(chat_gpt rclcpp std_msgs CURL openai_interface geometry_msgs)

add_executable(time src/client_node/time.cpp)
ament_target_dependencies(time rclcpp std_msgs)

add_executable(bridge src/client_node/bridge.cpp)
ament_target_dependencies(bridge rclcpp std_msgs openai_interface)

add_executable(speaker src/robot_node/speaker.cpp)
ament_target_dependencies(speaker rclcpp std_msgs openai_interface SFML)

add_executable(mic src/robot_node/mic.cpp)
ament_target_dependencies(mic rclcpp std_msgs openai_interface)

target_link_libraries(speaker
  sfml-system
  sfml-window
  sfml-graphics
  sfml-audio  # 오디오 라이브러리 추가
)

target_link_libraries(mic
  ${ALSA_LIBRARIES}
)

target_include_directories(mic PUBLIC
  ${ALSA_INCLUDE_DIRS}
)

#install
install(TARGETS
  whisper
  question_bridge
  answer_bridge
  chat_gpt
  time
  bridge
  speaker
  mic
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  param
  DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
