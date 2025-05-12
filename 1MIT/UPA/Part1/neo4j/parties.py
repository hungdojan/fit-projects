# UPA projekt 1
# Neo4j
# Vojtech Orava, David Kedra, Hung Do
import pandas as pd

data = pd.read_json("voting.json")
df = pd.json_normalize(data["data"])
dfp = pd.json_normalize(df["parties"])

parties = []
for i in range(7):
    dfp_parties = pd.json_normalize(dfp[i])
    dfp_parties["name"] = dfp_parties["name"].fillna("Nezařazení") # nahrazení nan
    for p in dfp_parties["name"].unique():
        parties.append(p)

f = open("parties.cypher", "w")

for i in range(len(parties)):

    party = {
        "id" : i,
        "name": parties[i],
    }

    f.write(
        f"MERGE (party:Party {{id: {party['id']}}}) " +
        f"ON CREATE SET party.name = '{party['name']}' RETURN party;\n"
    )


f.close()
