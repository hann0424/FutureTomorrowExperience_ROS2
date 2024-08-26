from openai import OpenAI
import time

content = """
        You are a chatbot that classifies responses. For every question, respond with a JSON object that includes the category and answer.
        - If the question is about general knowledge, use 'category: general'.
        - If the question is a greeting, use 'category: greeting'.
        - If the question relates to robot movement (like moving forward, turning, etc.), use 'category: movement'.
        """

client = OpenAI()

try :
    response = client.files.create(
    file=open("../data/mydata.jsonl", "rb"),
    purpose="fine-tune"
    )

    file_id = response.id

    print(f"Uploaded File ID: {file_id}")
    response1 = client.fine_tuning.jobs.create(
    training_file=file_id,
    model="gpt-3.5-turbo"
    )

    fine_tune_id = response1.id
    client.fine_tuning.jobs.list(limit=10)


    # Retrieve the state of a fine-tune

    while True :
        response2 = client.fine_tuning.jobs.retrieve(fine_tune_id)
        print(response2.status, "\n")

        if response2.status == "succeeded":
            fine_tuned_model_id = response2.fine_tuned_model
            print(fine_tuned_model_id, "\n")
            break

        elif response2.status == "failed":
            print("Fine-tuning is not yet complete.")
            break

        time.sleep(10)

    # completion = client.chat.completions.create(
    # model=fine_tuned_model_id,
    # messages=[
    #     {"role": "system", "content": content},
    #     {"role": "user", "content": "안녕"}
    # ]
    # )
    # print(completion.choices[0].message)

    # completion = client.chat.completions.create(
    # model=fine_tuned_model_id,
    # messages=[
    #     {"role": "system", "content": "Marv is a chatbot that helps control robot movement."},
    #     {"role": "user", "content": "앞으로 가"}
    # ]
    # )
    # print(completion.choices[0].message)

    # completion = client.chat.completions.create(
    # model=fine_tuned_model_id,
    # messages=[
    #     {"role": "system", "content": "Marv is a chatbot that helps control robot movement."},
    #     {"role": "user", "content": "멈춰"}
    # ]
    # )
    # print(completion.choices[0].message)

    # completion = client.chat.completions.create(
    # model=fine_tuned_model_id,
    # messages=[
    #     {"role": "system", "content": content},
    #     {"role": "user", "content": "오늘 기분 어때?"}
    # ]
    # )
    # print(completion.choices[0].message)

except Exception as e:
    print(f"An unexpected error occurred: {e}")
