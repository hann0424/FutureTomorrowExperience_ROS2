# FutureTomorrowExperience_ROS2

# openai 음성 대화 로봇

* 개발환경
  1. ubuntu linux 22.04
  2. turtlebot3
  3. raspberry pi 3
  4. ros2 humble
* 작업 분담
  * 최용한 : 로봇 이동 관련 기능 구현
  * 박도엽 : 마이크, 스피커 연동 기능 구현
  * 유재영 : 로봇 노드 구현(openai)
  * 정태양 : client 노드 구현(openai, pyqt)
  
---------------------------------------------
8/21(수)
- 오전
- 오후
---------------------------------------------
8/20(화)
- 작업 내용
  * 최용한
  
  * 박도엽
    * 라즈베리파이 마이크 연동 작업
  * 유재영
    * 답변 결과 음성파일로 변환하는 노드 구현
  * 정태양
    * chat_gpt.cpp gpt 답변 결과 발행하는 기능 구현
- 과제
  1. topic -> service로 바꿔야되는 부분 변경
  2. 어떤 시점에 gpt를 호출할 것인지
  3. 마이크 실시간 연동 - 음성 텍스트 변환 시점과, 방법
- 오전
  * 최용한
  * 박도엽
    * 라즈베리파이 마이크 연동 작업
  * 유재영
    * topic -> servicce로 변경
  * 정태양
    * topic -> servicce로 변경
- 오후
  * 최용한
    * 라즈베리파이 마이크 연동 작업
  * 박도엽
  * 유재영

  * 정태양
    * whisper.cpp response 이슈 해결

---------------------------------------------
8/19(월)
- 오전
  * 최용한
  * 박도엽
    * 라즈베리파이 마이크 연동 작업
  * 유재영
    * 음성파일에서 변환된 텍스트 데이터 구독, 발행하는 노드(기능) 구현 (question_bridge.cpp)
  * 정태양
    * 음성파일 텍스트 변환 후 publish하는 기능 구현 (whisper.cpp)

- 오후
  * 최용한
  * 박도엽
    * 라즈베리파이 마이크 연동 작업
  * 유재영
    * gpt답변 결과 구독하는 노드 구현 (answer_bridge.cpp)
    * 실행 테스트
  * 정태양
    * 음성질문 구독하여 gpt에 질문하는 노드 구현 (chat_apt.cpp)

---------------------------------------------
8/16(금)

1. openai c++라이브러리 사용하여 구현
  - libcurl
  1) libcurl 설치
    - sudo apt-get install libcurl4-openssl-dev
  2) CMake파일에 라이브러리 추가
    find_package(CURL REQUIRED)
    ament_target_dependencies(whisper rclcpp std_msgs CURL)
  3) openai cpp git 페이지에서 .hpp파일 다운로드후 프로잭트 패키지에 추가
    https://github.com/olrea/openai-cpp/tree/main/include/openai

---------------------------------------------
