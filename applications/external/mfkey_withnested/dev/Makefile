all:
	gcc mfkeymin.c -o mfkey
test:
	@/usr/bin/echo -e "Expected result:\na0a1a2a3a4a5\n65e26477b84b\n2a2c13cc242a"
	/usr/bin/time -v ./mfkey
clean:
	rm mfkey
