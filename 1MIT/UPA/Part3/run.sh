#!/bin/sh

source ./.venv/bin/activate
if [[ ! -f ./url_test.txt ]]; then
    python3.10 ./get_url.py > url_test.txt
fi
head -n10 ./url_test.txt | python3.10 ./extract_data.py
