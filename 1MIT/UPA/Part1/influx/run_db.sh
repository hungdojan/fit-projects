#!/bin/sh

if [[ $# -lt 1 ]]; then
    podman run -d --rm \
        -p 8086:8086 \
        -v influx_db_volume:/var/lib/influxdb2 \
        -v ./influx-configs:/etc/influxdb2/influx-configs \
        -v ./data:/data \
        --name influxdb \
        influxdb:latest
elif [[ $1x == '-ex' ]]; then
    podman exec -it influxdb bash
elif [[ $1x == '-kx' ]]; then
    podman kill influxdb
elif [[ $1x == '-sx' ]]; then 
    podman exec influxdb \
        influx write \
            -b upa-bucket \
            -f /data/air_quality_formatted.csv \
            --header "#datatype measurement,double,double,double,ignore,tag,string,tag,ignore,ignore,dateTime:2006-01-02 15:04:05-07:00,ignore"
fi
