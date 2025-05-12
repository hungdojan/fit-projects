ARGS=-training=./dataset/openstack_normal2.log -testing=./dataset/openstack_abnormal.log --export-log-structure --export-event-matrices --export-anomaly --verbose

.PHONY: run_demo clean setup_env pack

run_demo: venv/
	. venv/bin/activate && python3 log-monitor $(ARGS)

setup_env: venv/

venv/: requirements.txt
	python3 -m venv venv && . venv/bin/activate && pip install -r requirements.txt

pack:
	rm -f *.zip
	zip -r xdohun00.zip dataset/ Makefile log-monitor Readme.txt requirements.txt

clean:
	rm -f *.csv *.zip
