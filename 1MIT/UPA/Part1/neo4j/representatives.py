# UPA projekt 1
# Neo4j
# Vojtech Orava, David Kedra, Hung Do

import pandas as pd
import codecs


data = pd.read_json("voting.json")
df = pd.json_normalize(data["data"])
dfp = pd.json_normalize(df["parties"])
for i in range(7):
    dfp_parties = pd.json_normalize(dfp[i])
    dfp_parties["name"] = dfp_parties["name"].fillna("Nezařazení")

df = df.to_numpy()

f = codecs.open("how_voted.csv", "w", "utf-8-sig")
f.write("code;number;party;name;surname;option\n")
for i in range(len(df)):
    vote = {
        "code": "",
        "number": 0
    }
    vote["code"] = df[i][0]
    vote["number"] = df[i][1]


    # [i] - jednotliva hlasovani
    # [5] - hlasy dle stran
    # [j] - jednotlive strany
    # [votes] - hlasovani
    # [q] - jednotlivi politici
    # [voter] - konkretni politik
    # 0 - jmeno
    # 1 - prijmeni
    # text Ano/ne/nepritomen
    for j in range(len(df[i][5])):

        for q in range(len(df[i][5][j]["votes"])):
            f.write(f"{vote['code']};{vote['number']};")
            f.write(f'{df[i][5][j]["name"]};')
            f.write(f'{df[i][5][j]["votes"][q]["voter"].split(" ")[0]};')
            f.write(f'{df[i][5][j]["votes"][q]["voter"].split(" ")[1]};')
            f.write(f'{df[i][5][j]["votes"][q]["option"]}')
            f.write("\n")

f.close()

df2 = pd.read_csv("how_voted.csv", delimiter=";")
df3 = df2.drop_duplicates(subset=["name", "surname"])
df3 = df3[["name", "surname", "party"]]
df3["id"] = [i+1 for i in range(len(df3))]

df3.to_csv("politicians.csv", sep=";", encoding="utf-8-sig", index=False)
