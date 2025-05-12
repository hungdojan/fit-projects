import csv
import requests

url = "https://data.brno.cz/datasets/mestobrno::nehody-s-%C3%BA%C4%8Dast%C3%AD-chodc%C5%AF-pedestrian-accidents.csv?where=1=1&outSR=%7B%22latestWkid%22%3A32633%2C%22wkid%22%3A326"
filename = "nehody.csv"

response = requests.get(url)
with open(filename, 'wb') as f:
    # Save the original file nehody.csv
    f.write(response.content)

output_file = "modif_" + filename


with open(filename, 'r') as input_csv, \
    open(output_file, 'w', newline='') as output_csv:

    reader = csv.reader(input_csv)
    writer = csv.writer(output_csv)

    for row in reader:
        original_date = row[6]
        # Convert slashes to dashes
        date = original_date.replace('/', '-')
        # Replace the original date
        row[6] = date
        writer.writerow(row)

