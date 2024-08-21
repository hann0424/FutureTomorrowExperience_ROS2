cmake_minimum_required(VERSION 3.5)
project(test_cpp_pkg)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(CURL REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
# PortAudio 경로 설정 (변수명 일관성 유지)
set(PORTAUDIO_INCLUDE_DIR /usr/include)
set(PORTAUDIO_LIBRARY /usr/lib/x86_64-linux-gnu/libportaudio.so)

include_directories(${PORTAUDIO_INCLUDE_DIR})  # PortAudio 헤더 파일 포함

# add_executable으로 타겟 생성
add_executable(mike_node src/mike_node/mike.cpp)
# 타겟에 대한 종속성 추가
ament_target_dependencies(mike_node rclcpp std_msgs CURL)

target_link_libraries(mike_node ${PORTAUDIO_LIBRARY})  # PortAudio 라이브러리 링크

# install 설정
install(TARGETS
  mike_node
  DESTINATION lib/${PROJECT_NAME}
)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_cpplint_FOUND TRUE)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
