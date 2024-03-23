/**
 * A ring buffer.  This is a circular buffer that can be used to store data until a
 * delimiter is found, at which point the line can be extracted.  The ring buffer is a
 * fixed size and will overwrite old data if the buffer is full.
 * 
 * @author CodeAllNight
*/

#include <furi.h>

/**
 * Ring buffer structure
*/
typedef struct RingBuffer RingBuffer;

/**
 * Allocates a new ring buffer.  This is a circular buffer that can be used to store data
 * until a delimiter is found, at which point the line can be extracted.  The ring buffer
 * is a fixed size and will overwrite old data if the buffer is full.
 * 
 * @return A new ring buffer
*/
RingBuffer* ring_buffer_alloc();

/**
 * Frees a ring buffer.
 * 
 * @param rb The ring buffer to free
*/
void ring_buffer_free(RingBuffer* rb);

/**
 * Sets the delimiter for the ring buffer.  The delimiter is the character that will be
 * searched for when extracting a line from the buffer.
 * 
 * @param rb                The ring buffer
 * @param delimiter         The delimiter character
 * @param include_delimiter If true, the delimiter will be included in the extracted line
*/
void ring_buffer_set_delimiter(RingBuffer* rb, char delimiter, bool include_delimiter);

/**
 * Returns the number of bytes available in the ring buffer.  This is the number of bytes
 * that can be added to the buffer before it is full.
 * 
 * @param rb The ring buffer
 * 
 * @return The number of bytes available in the ring buffer
*/
size_t ring_buffer_available(RingBuffer* rb);

/**
 * Adds data to the ring buffer.  If the buffer is full, the oldest data will be overwritten.
 * 
 * @param rb     The ring buffer
 * @param data   The data to add
 * @param length The length of the data to add
 * 
 * @return True if the data was added successfully, false if the buffer is full
*/
bool ring_buffer_add(RingBuffer* rb, uint8_t* data, size_t length);

/**
 * Searches the ring buffer for the delimiter.  If the delimiter is found, the index of the
 * delimiter is returned.  If the delimiter is not found, the function returns FURI_STRING_FAILURE
 * (-1).
 * 
 * @param rb The ring buffer
 * 
 * @return The index of the delimiter, or FURI_STRING_FAILURE if the delimiter is not found
*/
size_t ring_buffer_find_delim(RingBuffer* rb);

/**
 * Extracts a line from the ring buffer.  The line is stored in the provided FuriString.
 * 
 * @param rb          The ring buffer
 * @param delim_index The index of the delimiter
 * @param line        The FuriString to store the line in
*/
void ring_buffer_extract_line(RingBuffer* rb, size_t delim_index, FuriString* line);

/**
 * Clears the ring buffer.  This will remove all data from the buffer.
 * 
 * @param rb The ring buffer
*/
void ring_buffer_clear(RingBuffer* rb);