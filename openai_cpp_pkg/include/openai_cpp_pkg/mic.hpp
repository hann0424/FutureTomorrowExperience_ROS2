#ifndef MIC_HPP
#define MIC_HPP

#include "rclcpp/rclcpp.hpp"
#include "openai_interface/srv/qa_interface.hpp"
#include <fstream>
#include <alsa/asoundlib.h>
#include <chrono>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
using namespace std::chrono_literals;

class Mic : public rclcpp::Node
{
private:
    rclcpp::Client<openai_interface::srv::QaInterface>::SharedPtr _client_mic;

    void response_callback(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future);
    void send_request();
    void record_audio();
    void write_wav_header(std::ofstream &file, int sample_rate, int bits_per_sample, int channels, int data_size);
    void set_nonblocking_input();
    void reset_terminal_input();
public:
    Mic();
};



#endif
