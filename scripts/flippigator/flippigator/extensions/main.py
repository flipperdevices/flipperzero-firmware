from proxmark_wrapper import proxmark_wrapper
from command_result import command_result
from command_result import proxmark_command
from command_result import Scenario

from types import SimpleNamespace
from typing import List
import json

PM3 = "C:\\Proxmark\\pm3.bat"

def print_result(result : List[command_result]):
 for r in result:                
        print(f"Command: {r.Command}\n Result: {r.Result}")  

def raw_proxmark_test():
    pm = proxmark_wrapper(PM3)
    result = pm.execute(["hf 14a raw -sc 3000\n","hf 14a raw -sc 3000\n","hf 14a raw -sc 3001\n"])
    print_result(result)
    print("-------------------------------------")

    result = pm.execute(["hf 14a raw -sc 300A\n"])
    print_result(result)    
    print("-------------------------------------")

    result = pm.execute(["hf 14a raw -sc 300A -k DEADBEAF\n"])
    print_result(result)
    print("-------------------------------------")

    result = pm.execute(["hf 14a raw -sck 1BDEADBEAF\n","hf 14a raw -ac 300A\n"])
    print_result(result)
    print("-------------------------------------")

    pm.shutdown()

def read_scenarios_from_file(file)->List[Scenario]:
    return read_scenarios_from_json(file)

def read_scenarios_from_json(file)->List[Scenario]:
    with open(file, "r") as file:    
        data = json.loads(file.read())
    
    scenarios_data = data["scenarios"]

    scenarios = []
    for data in scenarios_data:
       scenarios.append(Scenario(data))
    return scenarios


def main():
    '''
    with open("CMD_READ.json", "r") as file:    
        data = json.loads(file.read())
    
    scenarios_data = data["scenarios"]

    scenarios = []
    for data in scenarios_data:
       scenarios.append(Scenario(data))
    '''
    scenarios = read_scenarios_from_file("CMD_READ.json")
    for scenario in scenarios:
        run_scenario(scenario)
        

def run_scenario(scenario):
    pm3 = proxmark_wrapper(PM3)
    cmds = scenario.get_commands()
    results = pm3.execute(cmds)
    
    expected_len = len(scenario.Cmds)
    actual_len = len(results)
    passed = 1
    passed &= check_result(actual_len, expected_len)
    
    i = 0
    while i < actual_len:
       passed &= check_result(results[i].Command,  scenario.Cmds[i].Command)
       passed &= check_result(results[i].Result,  scenario.Cmds[i].Expected)
       i += 1

    if passed: 
        print("PASSED")
    else:
        print("FAILED")
    print("-----------------------------------------------------------------------------------")

def check_result(actual, expected):
    if actual != expected:
        print(f"{actual} != {expected}\tFAILED")
        return False    
    print(f"{actual} == {expected}\tPASSED")
    return True    

if __name__ == "__main__":
    #raw_proxmark_test()
    main()