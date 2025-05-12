TARGET=flp23-log
SRC=proj2.pl

all: $(TARGET)

test: $(TARGET)
	./$< < ./tests/scrambled_numbered.in

$(TARGET): $(SRC)
	swipl -q -g start -o $@ -c $<

run_repl: $(TARGET)
	swipl $<

# containers

build_container:
	podman build -t my-swipl .

run_container:
	podman run --rm -it -v .:/data -v ./bashrc:/root/.bashrc localhost/my-swipl:latest

################

pack: clean
	zip -r flp-log-xdohun00.zip Makefile proj2.pl README.md tests

clean:
	rm -f $(TARGET) *.zip
