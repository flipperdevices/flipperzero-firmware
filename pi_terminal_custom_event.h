#pragma once

typedef enum {
    Pi_TerminalEventRefreshConsoleOutput = 0,
    Pi_TerminalEventSetup,
    Pi_TerminalEventStartConsole,
    Pi_TerminalEventStartKeyboardText,
    Pi_TerminalEventStartKeyboardHex,
    Pi_TerminalEventStartHelp,
} Pi_TerminalCustomEvent;
