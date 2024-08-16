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


