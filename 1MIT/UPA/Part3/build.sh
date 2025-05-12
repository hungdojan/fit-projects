#!/bin/sh

if [ ! -d ".venv" ]; then
    python3.10 -m venv .venv
    source .venv/bin/activate
    if [ -f "requirements.txt" ]; then
        pip3.10 install --no-cache-dir -r requirements.txt
    fi
fi
