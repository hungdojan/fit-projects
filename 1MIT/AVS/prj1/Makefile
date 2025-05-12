run-container:
	podman run -it --rm \
		-e DISPLAY \
		-v .:/data:Z \
		--net=host \
		hungdojan/avs-fit-vut:latest bash

clean:
	rm -rf build
