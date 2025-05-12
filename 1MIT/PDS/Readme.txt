# Log File Analysis and Anomaly Detection

Author: Hung Do
Date: 22/04/2024

This tool serves as a submission to an assignment of the class PDS at FIT, BUT 2023/24.
Our goal was to create a tool that could detect anomalies in the log files. My tool uses
Isolation Forest method.

## List of submitted files
- `Readme.txt` - this file
- `Makefile` - a script that can be used to install dependencies or run a demo program
- `log-monitor` - a Python script that serves as a submission to the assignment
- `dataset/` - a folder with OpenStack log files downloaded from the official 
[https://github.com/logpai/loghub](LogHub) page
- `requirements.txt` - a list of packages required by the Python script to run
- `xdohun00.pdf` - a project report written in Czech

## Running the tool
You can use the provided `Makefile` to execute the tool using `make run_demo`
command. This command will automatically install dependencies if missing by generating
a Python *virtual environment* and installing missing modules using `pip` command, and
run a demo (same command was used to generate results in the report).

If you would like to run tool manually, you can copy paste this set of bash commands in
order to setup your working Python environment.

```bash
python -m venv venv

source venv/bin/activate
pip install -r requirements.txt

```

After that is done feel free to use `log-monitor` tool (the only requirement is that you
need to have your virtual environment activated). A list of tool options are listed in the
next section.

```bash
# source into the virtual environment if not done already
source venv/bin/activate
python log-monitor -training <file> -testing <file>
```

## Tool options
Apart from mandatory options (`-training` and `-testing`) the tool supports another
3 types of options.

There are two options that manipulates with model training. `-c CONTAMINATION` option
expects a value between 0 and 0.5 and it tells the model what is the percentage of
outliers in the training dataset (0.05 means that 5% of the given dataset are outliers).
`-rs RANDOM_STATE` is a magical constant chosen by the user for the purpose of
reproducing an identical model every time user feeds the tool with the same input datasets.

There are three optional *export* options that user can set:
- `-el, --export-log-structure`  - tool will generate a CSV files with parsed log files,
- `-ee, --export-event-matrices` - tool will generate a CSV files with event matrices,
- `-ea, --export-anomaly`        - tool will generate a CSV files where each instance is
  classified as either abnormal or normal by the model.

Lastly, there are two options that the user can use to choose what the tool will display.
`--verbose` option will display some basic logs information about the program. User can 
use this option to observe what the tool is doing at the moment. The second option, `-th`
is used to filter out final anomaly listing output. The user can choose filter out
instances based on their anomaly score.

The tool usage can be seen below (use `-h, --help` option to display it yourself).
```
usage: log-monitor [-h] -training TRAINING -testing TESTING [-c CONTAMINATION] [-rs RANDOM_STATE]
                   [-th ANOMALY_SCORE_THRESHOLD] [-el] [-ee] [-ea] [-v]

options:
  -h, --help            show this help message and exit
  -training TRAINING    Log file containing training data.
  -testing TESTING      Log file containing testing data.
  -c CONTAMINATION, --contamination CONTAMINATION
                        Percentage of anomalies in training data (range <0; 0.5>). Defaults to 0.05.
  -rs RANDOM_STATE, --random-state RANDOM_STATE
                        A constant used as a seed for isolation forest algorithm. Same seed value with
                        same input data will generate identical model. Defaults to 42.
  -th ANOMALY_SCORE_THRESHOLD, --anomaly-score-threshold ANOMALY_SCORE_THRESHOLD
                        Show instances that have anomaly score lower than the given value (range <-1;
                        0>). Defaults to 0.
  -el, --export-log-structure
                        Export log structures to CSV file.
  -ee, --export-event-matrices
                        Export event matrices of training and testing data to CSV file.
  -ea, --export-anomaly
                        Export anomaly instances to CSV file.
  -v, --verbose         Show logging prints.
```
