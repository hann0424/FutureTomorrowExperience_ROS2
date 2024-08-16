import sys
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QVBoxLayout
from PyQt5.QtCore import Qt
from openai import OpenAI
from pathlib import Path

client = OpenAI()

audio_file= open("/home/jty6109/robot_ws/src/FutureTomorrowExperience_ROS2/test_pkg/src/question.mp3", "rb")
transcription = client.audio.transcriptions.create(
  model="whisper-1",
  file=audio_file
)
print(transcription.text)

response2 = client.chat.completions.create(
  model="gpt-4o",
  messages=[
    {"role": "system", "content": "You are a helpful assistant."},
    {"role": "user", "content": transcription.text}
  ]
)

message = response2.choices[0].message.content
print(message)

speech_file_path = Path(__file__).parent / "answer.mp3"
response = client.audio.speech.create(
  model="tts-1",
  voice="alloy",
  input=message
)
response.stream_to_file(speech_file_path)

class MyApp(QWidget):

    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        label1 = QLabel(transcription.text, self)
        label1.setAlignment(Qt.AlignCenter)

        label2 = QLabel(message, self)
        label2.setAlignment(Qt.AlignVCenter)


        layout = QVBoxLayout()
        layout.addWidget(label1)
        layout.addWidget(label2)
        
        self.setLayout(layout)

        self.setWindowTitle('My First Application')
        self.move(300, 300)
        self.resize(400, 200)
        self.show()

if __name__ == '__main__':
   app = QApplication(sys.argv)
   ex = MyApp()
   sys.exit(app.exec_())
