# UPA projekt 1
# MongoDB
# Vojtech Orava, David Kedra, Hung Do
import json
import argparse
import codecs

parser = argparse.ArgumentParser(description='Converts original BRNO BRZO GeoJSON file to MongoDB GeoJSON import format')
parser.add_argument('--file', required=True, help='Path to original GeoJSON file')
args = parser.parse_args()

fout = codecs.open("brno.geojson", "w", "utf-8-sig")
fin = open(args.file, "r")

data = json.load(fin)
#print(data["features"][0])

for i in range(len(data["features"])):
    fout.write(json.dumps(data["features"][i], ensure_ascii=False))
    fout.write("\n")

fin.close()
fout.close()
