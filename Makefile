MAKEFLAGS += -s

snepfetch:
	echo Started compiling...
	$(MAKE) -C src snepfetch
	echo Done!

format:
	echo Executing clang-format...
	$(MAKE) -C src format
	echo Done!

install:
	echo Installing...
	$(MAKE) -C src install
	echo Done!

clean:
	echo Cleaning up...
	$(MAKE) -C src clean
	echo Done!

.PHONY: snepfetch format install clean
