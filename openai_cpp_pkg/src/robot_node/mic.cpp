#include "openai_cpp_pkg/mic.hpp"

Mic::Mic()
    : Node("mic_node")
{
    _client_mic = this->create_client<openai_interface::srv::QaInterface>("whisper");
    record_audio();
}

void Mic::send_request()
{
    auto request = std::make_shared<openai_interface::srv::QaInterface::Request>();
    request->question = "question_answer record complete";  // 요청 필드에 전사된 텍스트 저장

    // 서비스가 준비될 때까지 기다림
    if (!_client_mic->wait_for_service(15s)) {
        RCLCPP_INFO(this->get_logger(), "서비스를 기다리는 중...");
        return;
    }

    // 비동기 방식으로 서비스 요청 전송
    auto result_future = _client_mic->async_send_request(
        request, std::bind(&Mic::response_callback, this,
        std::placeholders::_1));
}

void Mic::response_callback(rclcpp::Client<openai_interface::srv::QaInterface>::SharedFuture future)
{
    try {
        auto result = future.get();  // 응답 대기
        RCLCPP_INFO(get_logger(), "응답 메시지: %s", result->answer.c_str());  // 응답 필드 사용
    } catch (const std::exception &e) {
        RCLCPP_ERROR(get_logger(), "서비스 요청 중 오류 발생: %s", e.what());
    }
}

void Mic::record_audio()
{
    const int sample_rate = 44100;  // 44.1kHz
    const int channels = 1;  // 모노
    const int bits_per_sample = 16;  // 16비트 오디오
    const int duration_in_seconds = 10;  // 5초간 녹음
    const char* wav_filename = "/home/ubuntu/robot_ws/src/FutureTomorrowExperience_ROS2/openai_cpp_pkg/audio/question.mp3";
    int frames_per_buffer = 32;

    // ALSA 초기화
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;

    if (snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) {
        std::cerr << "Error opening PCM device." << std::endl;
        return;
    }

    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, (unsigned int*)&sample_rate, 0);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, channels);
    snd_pcm_hw_params(capture_handle, hw_params);
    snd_pcm_hw_params_free(hw_params);

    if (snd_pcm_prepare(capture_handle) < 0) {
        std::cerr << "Error preparing PCM device." << std::endl;
        return;
    }

    // WAV 파일 열기
    std::ofstream wav_file(wav_filename, std::ios::binary);
    int data_size = duration_in_seconds * sample_rate * channels * bits_per_sample / 8;
    write_wav_header(wav_file, sample_rate, bits_per_sample, channels, data_size);

    // 녹음 시작
    char *buffer = new char[frames_per_buffer * channels * bits_per_sample / 8];
    for (int i = 0; i < duration_in_seconds * (sample_rate / frames_per_buffer); ++i) {
        snd_pcm_readi(capture_handle, buffer, frames_per_buffer);
        wav_file.write(buffer, frames_per_buffer * channels * bits_per_sample / 8);
    }

    // 마무리
    delete[] buffer;
    wav_file.close();
    snd_pcm_close(capture_handle);

    std::cout << "Recording complete. Saved to " << wav_filename << std::endl;

    send_request();
}

void Mic::write_wav_header(std::ofstream &file, int sample_rate, int bits_per_sample, int channels, int data_size)
{
    file << "RIFF";  // ChunkID
    file.write(reinterpret_cast<const char*>(&data_size), 4);  // ChunkSize
    file << "WAVE";  // Format
    file << "fmt ";  // Subchunk1ID
    int subchunk1_size = 16;
    file.write(reinterpret_cast<const char*>(&subchunk1_size), 4);  // Subchunk1Size
    short audio_format = 1;  // PCM
    file.write(reinterpret_cast<const char*>(&audio_format), 2);  // AudioFormat
    file.write(reinterpret_cast<const char*>(&channels), 2);  // NumChannels
    file.write(reinterpret_cast<const char*>(&sample_rate), 4);  // SampleRate
    int byte_rate = sample_rate * channels * bits_per_sample / 8;
    file.write(reinterpret_cast<const char*>(&byte_rate), 4);  // ByteRate
    short block_align = channels * bits_per_sample / 8;
    file.write(reinterpret_cast<const char*>(&block_align), 2);  // BlockAlign
    file.write(reinterpret_cast<const char*>(&bits_per_sample), 2);  // BitsPerSample
    file << "data";  // Subchunk2ID
    file.write(reinterpret_cast<const char*>(&data_size), 4);  // Subchunk2Size
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Mic>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
