import unittest
from proxmark_wrapper import proxmark_wrapper
from command_result import command_result

PM3 = "C:\\Proxmark\\pm3.bat"

class TestCmdRead(unittest.TestCase):
    def setUp(self) -> None:
        self.pm = proxmark_wrapper(PM3)

    def test_Read_Page0(self):
        print("Test: 1")
        cmd = "hf 14a raw -sc 3000\n"
        expected = "04 BA FF C9 CA 4D 5D 80 5A 48 00 00 E1 10 12 00 [ F5 67 ]"
        cmds = [cmd]
        result = self.pm.execute(cmds)
        self.pm.shutdown()

        self.assertEqual(result[0].Command, cmd.removesuffix("\n"))
        self.assertEqual(result[0].Result, expected)

if __name__ == "__main__":
  unittest.main()