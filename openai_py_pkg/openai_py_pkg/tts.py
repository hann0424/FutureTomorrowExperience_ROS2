import rclpy
from rclpy.node import Node
from openai_interface.srv import QaInterface  # C++ 패키지에서 빌드된 srv 인터페이스 사용
from openai import OpenAI
from pathlib import Path

class Tts(Node):
    def __init__(self):
        super().__init__('tts_node')
        self.tts_srv = self.create_service(QaInterface, 'tts', self.callback_tts)

    def callback_tts(self, request, response):
        self.tts_answer()
        self.answer = request.question
        response.answer = "answer recorded complete"
        return response

    def tts_answer(self):
        self.client = OpenAI()
        speech_file_path = Path("/home/jty6109/robot_ws/src/FutureTomorrowExperience_ROS2/result_audio").parent / "answer.mp3"
        response = self.client.audio.speech.create(
        model="tts-1",
        voice="alloy",
        input=self.answer
        )
        response.stream_to_file(speech_file_path)

def main(args=None):
    rclpy.init(args=args)
    node = Tts()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
