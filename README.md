# FutureTomorrowExperience_ROS2

# openai 음성 대화 로봇

* 개발환경
  1. ubuntu linux 22.04
  2. turtlebot3
  3. raspberry pi 3
  4. ros2 humble

---------------------------------------------
8/16(금)

1. openai c++라이브러리 사용하여 구현
  - libcurl
  1) CMake파일에 라이브러리 추가 
    find_package(CURL REQUIRED)
    ament_target_dependencies(whisper rclcpp std_msgs CURL)
  2) openai cpp git 페이지에서 .hpp파일 다운로드후 프로잭트 패키지에 추가
    https://github.com/olrea/openai-cpp/tree/main/include/openai
  4) 
---------------------------------------------
