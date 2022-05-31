## FreeRTOS Partner Supported Ports

This repository contains FreeRTOS ports supported by FreeRTOS partners. Follow
the steps below to contribute a FreeRTOS port to this repository:

1. Write the FreeRTOS port for your Compiler and Architecture.
2. Create a project in the [FreeRTOS Partner Supported Demos Repository](https://github.com/FreeRTOS/FreeRTOS-Partner-Supported-Demos/tree/main)
   for your hardware for running tests as mentioned [here](https://github.com/FreeRTOS/FreeRTOS/blob/main/FreeRTOS/Demo/ThirdParty/Template/README.md).
3. Make sure all the tests pass. Add the test results in the Pull Request description.
4. Add a README file with the following information:
    1. How to use this port?
    2. Link to the test project created in Step 2.
    3. Any other relevant information.
5. Raise a PR to merge the FreeRTOS port.
6. Raise another PR to merge the test project in the [FreeRTOS-Partner-Supported-Demos Repository](https://github.com/FreeRTOS/FreeRTOS/tree/main/FreeRTOS/Demo/ThirdParty/Community-Supported).


## License

This repository contains multiple directories, each individually licensed. Please see the LICENSE file in each directory.
