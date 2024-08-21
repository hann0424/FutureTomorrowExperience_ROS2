#include <iostream>
#include <fstream>
#include <portaudio.h>
#include <cstdint>  // For fixed-width integer types like int16_t, int32_t
#include <curl/curl.h>
#include <cstdlib>  // for getenv()

const int SAMPLE_RATE = 16000;
const int FRAMES_PER_BUFFER = 1024;
const int NUM_CHANNELS = 1;
const int BITS_PER_SAMPLE = 16;

// 녹음된 오디오 파일의 내용을 Whisper API로 전송해 텍스트로 변환하기 위한 콜백 함수
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t new_length = size * nmemb;
    s->append(static_cast<char*>(contents), new_length);
    return new_length;
}

class MicrophoneTest {
public:
    MicrophoneTest() {
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        }

        err = Pa_OpenDefaultStream(&stream, NUM_CHANNELS, 0, paInt16, SAMPLE_RATE, FRAMES_PER_BUFFER, nullptr, nullptr);
        if (err != paNoError) {
            std::cerr << "Failed to open stream: " << Pa_GetErrorText(err) << std::endl;
        }

        err = Pa_StartStream(stream);
        if (err != paNoError) {
            std::cerr << "Failed to start stream: " << Pa_GetErrorText(err) << std::endl;
        }
    }

    ~MicrophoneTest() {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        Pa_Terminate();
    }

    void record_to_wav(const std::string& filename, int duration_seconds) {
        std::ofstream file(filename, std::ios::binary);

        // WAV 헤더 작성
        write_wav_header(file);

        int16_t buffer[FRAMES_PER_BUFFER];
        int total_frames = SAMPLE_RATE * duration_seconds / FRAMES_PER_BUFFER;

        for (int i = 0; i < total_frames; ++i) {
            Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);
            file.write(reinterpret_cast<char*>(buffer), sizeof(buffer));
        }

        // 파일 크기 수정
        update_wav_header(file);
        file.close();
    }

private:
    PaStream* stream;

    void write_wav_header(std::ofstream& file) {
        // RIFF 헤더
        file.write("RIFF", 4);
        int32_t file_size_placeholder = 0;  // 나중에 업데이트될 파일 크기
        file.write(reinterpret_cast<const char*>(&file_size_placeholder), 4);
        file.write("WAVE", 4);

        // fmt 서브청크
        file.write("fmt ", 4);
        int32_t subchunk1_size = 16;  // PCM 포맷에서는 항상 16
        int16_t audio_format = 1;  // PCM = 1
        file.write(reinterpret_cast<const char*>(&subchunk1_size), 4);
        file.write(reinterpret_cast<const char*>(&audio_format), 2);
        file.write(reinterpret_cast<const char*>(&NUM_CHANNELS), 2);
        file.write(reinterpret_cast<const char*>(&SAMPLE_RATE), 4);

        int32_t byte_rate = SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE / 8;
        int16_t block_align = NUM_CHANNELS * BITS_PER_SAMPLE / 8;
        file.write(reinterpret_cast<const char*>(&byte_rate), 4);
        file.write(reinterpret_cast<const char*>(&block_align), 2);
        file.write(reinterpret_cast<const char*>(&BITS_PER_SAMPLE), 2);

        // data 서브청크
        file.write("data", 4);
        int32_t data_size_placeholder = 0;  // 나중에 업데이트될 데이터 크기
        file.write(reinterpret_cast<const char*>(&data_size_placeholder), 4);
    }

    void update_wav_header(std::ofstream& file) {
        std::streampos file_length = file.tellp();

        // 파일 크기 갱신 (RIFF 청크 크기)
        int32_t file_size = static_cast<int32_t>(file_length) - 8;
        file.seekp(4);
        file.write(reinterpret_cast<const char*>(&file_size), 4);

        // 데이터 청크 크기 갱신
        int32_t data_size = static_cast<int32_t>(file_length) - 44;
        file.seekp(40);
        file.write(reinterpret_cast<const char*>(&data_size), 4);
    }
};

// Whisper API를 호출해 녹음된 오디오 파일을 텍스트로 변환하는 함수
std::string transcribe_audio(const std::string& filepath) {
    const char* api_key = std::getenv("OPENAI_API_KEY");  // 환경 변수에서 API 키 가져오기
    if (!api_key) {
        std::cerr << "API key not found. Please set OPENAI_API_KEY environment variable." << std::endl;
        return "";
    }

    CURL* curl;
    CURLcode res;
    curl_mime* form = nullptr;
    curl_mimepart* field = nullptr;
    std::string response_string;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        form = curl_mime_init(curl);

        // 오디오 파일 추가
        field = curl_mime_addpart(form);
        curl_mime_name(field, "file");
        curl_mime_filedata(field, filepath.c_str());

        // 모델 선택
        field = curl_mime_addpart(form);
        curl_mime_name(field, "model");
        curl_mime_data(field, "whisper-1", CURL_ZERO_TERMINATED);

        // 요청 설정
        struct curl_slist* headers = nullptr;
        std::string auth_header = "Authorization: Bearer " + std::string(api_key);
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/audio/transcriptions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // 요청 수행
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // 클린업
        curl_slist_free_all(headers);
        curl_mime_free(form);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return response_string;
}

int main() {
    const std::string audio_filename = "audio.wav";

    MicrophoneTest mic;
    std::cout << "Recording..." << std::endl;
    mic.record_to_wav(audio_filename, 5);  // 5초간 녹음
    std::cout << "Recorded to " << audio_filename << std::endl;

    // Whisper API를 호출하여 텍스트로 변환
    std::cout << "Transcribing audio file..." << std::endl;
    std::string transcription = transcribe_audio(audio_filename);

    // 텍스트 변환 결과 출력
    std::cout << "Transcription result: " << transcription << std::endl;

    return 0;
}

