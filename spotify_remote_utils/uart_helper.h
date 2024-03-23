/**
 * UartHelper is a utility class that helps with reading lines of data from a UART.
 * It uses a stream buffer to receive data from the UART ISR, and a worker thread
 * to dequeue data from the stream buffer and process it.  The worker thread uses
 * a ring buffer to hold data until a delimiter is found, at which point the line
 * is extracted and the process_line callback is invoked.
 * 
 * @author CodeAllNight
*/

#include <furi.h>

/**
 * A helper class for parsing data received over UART.
*/
typedef struct UartHelper UartHelper;

/**
 * Callback function for processing a line of data.
 * 
 * @param line The line of data to process.
*/
typedef void (*ProcessLine)(FuriString* line, void* context);

/**
 * Allocates a new UartHelper.  The UartHelper will be initialized with a baud rate of 115200.
 * Log messages will be disabled since they also use the UART.
 * 
 * IMPORTANT -- FURI_LOG_x calls will not work!
 * 
 * @return A new UartHelper.
*/
UartHelper* uart_helper_alloc();

/**
 * Sets the delimiter to use when parsing data.  The default delimeter is '\n' and
 * the default value for include_delimiter is false.
 * 
 * @param helper            The UartHelper.
 * @param delimiter         The delimiter to use.
 * @param include_delimiter If true, the delimiter will be included in the line of data passed to the callback function.
*/
void uart_helper_set_delimiter(UartHelper* helper, char delimiter, bool include_delimiter);

/**
 * Sets the callback function to be called when a line of data is received.
 * 
 * @param helper        The UartHelper.
 * @param process_line  The callback function.
 * @param context       The context to pass to the callback function.
*/
void uart_helper_set_callback(UartHelper* helper, ProcessLine process_line, void* context);

/**
 * Sets the baud rate for the UART.  The default is 115200.
 * 
 * @param helper    The UartHelper.
 * @param baud_rate The baud rate.
*/
void uart_helper_set_baud_rate(UartHelper* helper, uint32_t baud_rate);

/**
 * Sends data over the UART TX pin.
*/
void uart_helper_send(UartHelper* helper, const char* data, size_t length);

/**
 * Sends a string over the UART TX pin.
*/
void uart_helper_send_string(UartHelper* helper, FuriString* string);

/**
 * Frees the UartHelper & enables log messages.
*/
void uart_helper_free(UartHelper* helper);