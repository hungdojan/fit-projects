# C project: DNS Tunnel
TARGET=dns_receiver dns_sender
SUBMIT_FILES=Makefile README.md manual.pdf include/* \
			 sender/Makefile sender/*.{c,h} receiver/*.{c,h} receiver/Makefile \
			 shared/*.c


#####################################

.PHONY: all run

all: sender receiver

#####################################

.PHONY: sender receiver

sender: sender/Makefile
	@$(MAKE) -Csender --no-print-directory

receiver: receiver/Makefile
	@$(MAKE) -Creceiver --no-print-directory

#####################################

.PHONY: clean pack

clean:
	@$(MAKE) -Csender clean --no-print-directory
	@$(MAKE) -Creceiver clean --no-print-directory
	rm -f ./**/*.o *.tar.gz

pack:
	rm -f xdohun00.tar.gz
	@tar -czvf xdohun00.tar.gz $(SUBMIT_FILES)
