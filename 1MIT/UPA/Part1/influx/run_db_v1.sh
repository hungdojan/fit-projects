#!/bin/sh

if [[ $# -lt 1 ]]; then
    podman run -d --rm \
        -p 8087:8086 \
        -v influx_db_volume_v1:/var/lib/influxdb2:Z \
        -v ./influx-configs:/etc/influxdb2/influx-configs:Z \
        -v ./data:/data:Z \
        --name influxdb_v1 \
        influxdb:1.8-alpine
elif [[ $1x == '-ex' ]]; then
    podman exec -it influxdb_v1 bash
elif [[ $1x == '-kx' ]]; then
    podman kill influxdb_v1
fi
