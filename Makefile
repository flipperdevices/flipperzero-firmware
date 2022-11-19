build:
	cd ../.. && ./fbt fap_usb_hid_autofire

build-launch:
	cd ../.. && ./fbt launch_app APPSRC=usb_hid_autofire
