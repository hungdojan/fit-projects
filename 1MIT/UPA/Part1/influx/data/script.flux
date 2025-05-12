// get measurements of the air quality in April 2023
// at the station Brno-Turany (BBNYA)
from(bucket: "upa-bucket")
    |> range(start: 2023-04-01, stop: 2023-05-01)
    |> filter(fn: (r) => 
        r._field == "value" and r.code == "BBNYA"
    )
    |> drop(columns: ["_start", "_stop", "result", "_field"])
    |> yield(name: "Basic")

// get the mean value for each type of measurement 
// in August 2023 in all the stations in Brno
from(bucket: "upa-bucket")
    |> range(start: 2023-08-01, stop: 2023-09-01)
    |> filter(fn: (r) => 
        r._field == "value"
    )
    |> group(columns: ["_measurement"])
    |> mean()
    |> drop(columns: ["_start", "_stop", "_field"])
    |> yield(name: "Advance")
