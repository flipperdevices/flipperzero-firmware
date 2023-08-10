#wolfSSL Example

The Example contains of wolfSSL tls client demo.

1. `idf.py menuconfig` to config the project

      1-1. Example Configuration ->  

          Target host ip address : the host that you want to connect to.(default is 127.0.0.1)

     1-2. Example Connection Configuration ->
     
          WIFI SSID: your own WIFI, which is connected to the Internet.(default is "myssid")  
          WIFI Password: WIFI password, and default is "mypassword"
    
    
    Note: the example program uses 11111 port. If you want to use different port  
        , you need to modify DEFAULT_PORT definition in the code.

When you want to test the wolfSSL client

1. `idf.py -p <PORT> flash` and then `idf.py monitor` to load the firmware and see the context  
2. You can use <wolfssl>/examples/server/server program for test.  

         e.g. Launch ./examples/server/server -v 4 -b -i -d

See the README.md file in the upper level 'examples' directory for more information about examples.
