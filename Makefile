.PHONY: warning
warning:
	@echo Flipper Zero Firmware is no longer built with make
	@echo Use ./fbt

%:: warning
	$(error Please read documentation/fbt.md for details)
