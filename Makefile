
all:
	cmake -B .cmake -G "Unix Makefiles"
	cd .cmake && make -j `nproc`

clean:
	rm -rf .cmake bin obj

version:
	make --version
	cmake --version

.PHONY: all clean version
