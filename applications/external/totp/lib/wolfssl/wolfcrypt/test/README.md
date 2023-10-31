# wolfCrypt Test

Tool for performing cryptographic algorithm testing.

## Example Output

Run on Intel(R) Core(TM) i7-7920HQ CPU @ 3.10GHz.

```sh
./configure --enable-intelasm --enable-aesni --enable-sp --enable-sp-asm && make

./wolfcrypt/test/testwolfcrypt
------------------------------------------------------------------------------
 wolfSSL version 4.0.0
------------------------------------------------------------------------------
error    test passed!
MEMORY   test passed!
base64   test passed!
asn      test passed!
MD5      test passed!
SHA      test passed!
SHA-224  test passed!
SHA-256  test passed!
SHA-384  test passed!
SHA-512  test passed!
SHA-3    test passed!
Hash     test passed!
HMAC-MD5 test passed!
HMAC-SHA test passed!
HMAC-SHA224 test passed!
HMAC-SHA256 test passed!
HMAC-SHA384 test passed!
HMAC-SHA512 test passed!
HMAC-SHA3   test passed!
GMAC     test passed!
Chacha   test passed!
POLY1305 test passed!
ChaCha20-Poly1305 AEAD test passed!
AES      test passed!
AES192   test passed!
AES256   test passed!
AES-GCM  test passed!
RANDOM   test passed!
RSA      test passed!
DH       test passed!
ECC      test passed!
logging  test passed!
mutex    test passed!
memcb    test passed!
Test complete
```


## Windows Visual Studio

For building wolfCrypt test project in Visual Studio open the `test.sln`. For newer Visual Studio version it may prompt for a one-way upgrade. Then you may have to right-click on the solution and choose `Retarget solution` to update the project files for your Visual Studio version. 

If you see an error about `rc.exe` then you'll need to update the "Target Platform Version". You can do this by right-clicking on the test project -> General -> "Target Platform Version" and changing to 8.1 (needs to match the wolfssl library project).

This solution includes the wolfSSL library project at `<wolfssl-root>wolfssl.vcxproj` and will compile the library, then the test project.
