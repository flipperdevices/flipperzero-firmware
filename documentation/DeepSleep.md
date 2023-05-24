Settings -> System -> Debug = OFF
Settings -> System -> Log level = None
Settings -> System -> Heap trace = None
Settings -> System -> Sleep Method = Default

when debug is on, new sleep mode will not work
debug will make flipper waiting for debug session via swd and it will never go to sleep when debug is on 
- Thank you xMasterX for this info