# flipperzero-e2e-py
```
           .-._   _ _ _ _ _ _ _ _
.-''-.__.-'00  '-' ' ' ' ' ' ' ' '-.
'.___ '    .   .--_'-' '-' '-' _'-' '._
 V: V 'vv-'   '_   '.       .'  _..' '.'.
   '=.____.=_.--'   :_.__.__:_   '.   : :
           (((____.-'        '-.  /   : :
                             (((-'\ .' /
                           _____..'  .'
                          '-._____.-'
```
Internal tool for hardware e2e testing of flipperzero on pytest.


### Running the tests
To run use the following command:
```
pytest
```
Additionally, you can specify port:
```
pytest --p COM3
```
Or you can run tests for specific module:
``` 
pytest --m nfc
```
Or you can search for a test or a test class:
``` 
pytest -k TestNfc
```
You can also write allure report:
```
pytest -p COM3 --alluredir=allure-report
```
And finally you can serve allure report if you have allure installed:
```
allure serve allure-report
```

### Writing tests
All tests should be placed in testing folder, specified by the module
in case of changes, write task ID into docstring of the test class, so we can know what made you do the changes.

### Running tests in parallel
To run tests in parallel, you need to install pytest-xdist and we need to finish writing device manager :)
