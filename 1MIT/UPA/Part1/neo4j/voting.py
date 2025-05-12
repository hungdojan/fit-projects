# UPA projekt 1
# Neo4j
# Vojtech Orava, David Kedra, Hung Do
import pandas as pd

data = pd.read_json("voting.json")

df = pd.json_normalize(data["data"])

f = open("voting.cypher", "w")

for i in range(len(df)):

    voting = {
        "code": df["code"][i],
        "number": df["number"][i],
        "date": df["datetime"][i].split("T")[0],
        "time": df["datetime"][i].split("T")[1],
        "subject": df["subject"][i],
        "result": df["result"][i]
    }

    print(voting)

    f.write(
        f"MERGE (voting:Voting {{code: '{voting['code']}', number: '{voting['number']}'}}) " +
        f"ON CREATE SET voting.subject = '{voting['subject']}', voting.date = '{voting['date']}', " +
        f"voting.time = '{voting['time']}', voting.result = '{voting['result']}' RETURN voting;\n"
    )


f.close()
