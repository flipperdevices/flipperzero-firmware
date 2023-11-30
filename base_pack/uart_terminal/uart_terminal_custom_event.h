#pragma once

typedef enum {
    UART_TerminalEventRefreshConsoleOutput = 0,
    UART_TerminalEventSetup,
    UART_TerminalEventStartConsole,
    UART_TerminalEventStartKeyboardText,
    UART_TerminalEventStartKeyboardHex,
    UART_TerminalEventStartHelp,
} UART_TerminalCustomEvent;
