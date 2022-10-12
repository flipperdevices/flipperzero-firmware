all:
	gcc mfkeymin.c -o mfkey
test:
	echo "Expected result: 824b8305a6df"
	/usr/bin/time -v ./mfkey dee5251d f96eef89 3f7743ef e48f8835 4c50d4c6 a096efa5 7542b17c
clean:
	rm mfkey
