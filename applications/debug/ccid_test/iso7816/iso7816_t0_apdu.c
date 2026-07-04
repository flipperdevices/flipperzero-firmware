/* Implements rudimentary iso7816-3 support for APDU (T=0) */
#include <stdint.h>
#include <string.h>
#include "iso7816_t0_apdu.h"

//reads pc_to_reader_datablock_len with pc_to_reader_datablock_len size, translate it into a ISO7816_Command_APDU type
//extra data will be pointed to commandDataBuffer
uint8_t iso7816_read_command_apdu(
    ISO7816_Command_APDU* command,
    const uint8_t* pc_to_reader_datablock,
    uint32_t pc_to_reader_datablock_len,
    uint32_t max_apdu_size) {
    // Minimum APDU is 4 bytes (header only)
    if(pc_to_reader_datablock_len < 4) {
        return ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LENGTH;
    }

    command->CLA = pc_to_reader_datablock[0];
    command->INS = pc_to_reader_datablock[1];
    command->P1 = pc_to_reader_datablock[2];
    command->P2 = pc_to_reader_datablock[3];

    // Case 1: No data, no response (4 bytes: CLA INS P1 P2)
    if(pc_to_reader_datablock_len == 4) {
        command->Lc = 0;
        command->Le = 0;
        command->LePresent = false;
        return ISO7816_READ_COMMAND_APDU_OK;
    }

    // Check for extended APDU format (byte 4 = 0x00 and length > 5)
    if(pc_to_reader_datablock_len > 5 && pc_to_reader_datablock[4] == 0x00) {
        // Extended APDU format
        return iso7816_read_command_apdu_extended(
            command, pc_to_reader_datablock, pc_to_reader_datablock_len, max_apdu_size);
    }

    // Short APDU format
    // Case 2S: No data, short Le (5 bytes: CLA INS P1 P2 Le)
    if(pc_to_reader_datablock_len == 5) {
        command->Lc = 0;
        command->Le = pc_to_reader_datablock[4];
        command->LePresent = true;
        return ISO7816_READ_COMMAND_APDU_OK;
    }

    // Case 3S/4S: Short Lc with data
    if(pc_to_reader_datablock_len > 5) {
        command->Lc = pc_to_reader_datablock[4];

        if(command->Lc == 0 || command->Lc > max_apdu_size) {
            return ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LENGTH;
        }

        memcpy(command->Data, &pc_to_reader_datablock[5], command->Lc);

        // Case 3S: Data, no Le (5+Lc bytes)
        if(pc_to_reader_datablock_len == (uint32_t)(command->Lc + 5)) {
            command->Le = 0;
            command->LePresent = false;
            return ISO7816_READ_COMMAND_APDU_OK;
        }

        // Case 4S: Data with short Le (6+Lc bytes)
        if(pc_to_reader_datablock_len == (uint32_t)(command->Lc + 6)) {
            command->Le = pc_to_reader_datablock[pc_to_reader_datablock_len - 1];
            command->LePresent = true;
            return ISO7816_READ_COMMAND_APDU_OK;
        }

        return ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LENGTH;
    }

    return ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LENGTH;
}

uint8_t iso7816_read_command_apdu_extended(
    ISO7816_Command_APDU* command,
    const uint8_t* pc_to_reader_datablock,
    uint32_t pc_to_reader_datablock_len,
    uint32_t max_apdu_size) {
    // Extended APDU: byte 4 is 0x00, followed by 2-byte Lc or Le

    // Case 2E: No data, extended Le (7 bytes: CLA INS P1 P2 00 Le1 Le2)
    if(pc_to_reader_datablock_len == 7) {
        command->Lc = 0;
        command->Le =
            (pc_to_reader_datablock[5] << 8) | pc_to_reader_datablock[6];
        command->LePresent = true;
        // Le = 0x0000 means 65536, but we limit to max_apdu_size
        return ISO7816_READ_COMMAND_APDU_OK;
    }

    // Case 3E/4E: Extended Lc with data
    if(pc_to_reader_datablock_len >= 7) {
        command->Lc =
            (pc_to_reader_datablock[5] << 8) | pc_to_reader_datablock[6];

        if(command->Lc == 0 || command->Lc > max_apdu_size) {
            return ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LENGTH;
        }

        // Check we have enough data
        if(pc_to_reader_datablock_len < (uint32_t)(7 + command->Lc)) {
            return ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LENGTH;
        }

        memcpy(command->Data, &pc_to_reader_datablock[7], command->Lc);

        // Case 3E: Data, no Le (7+Lc bytes)
        if(pc_to_reader_datablock_len == (uint32_t)(7 + command->Lc)) {
            command->Le = 0;
            command->LePresent = false;
            return ISO7816_READ_COMMAND_APDU_OK;
        }

        // Case 4E: Data with extended Le (9+Lc bytes)
        if(pc_to_reader_datablock_len == (uint32_t)(9 + command->Lc)) {
            command->Le =
                (pc_to_reader_datablock[pc_to_reader_datablock_len - 2] << 8) |
                pc_to_reader_datablock[pc_to_reader_datablock_len - 1];
            command->LePresent = true;
            return ISO7816_READ_COMMAND_APDU_OK;
        }

        return ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LENGTH;
    }

    return ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LENGTH;
}

//data buffer contains the whole APU response (response + trailer (SW1+SW2))
void iso7816_write_response_apdu(
    const ISO7816_Response_APDU* response,
    uint8_t* reader_to_pc_datablock,
    uint32_t* reader_to_pc_datablock_len) {
    uint32_t responseDataBufferIndex = 0;

    //response body
    if(response->DataLen > 0) {
        while(responseDataBufferIndex < response->DataLen) {
            reader_to_pc_datablock[responseDataBufferIndex] =
                response->Data[responseDataBufferIndex];
            responseDataBufferIndex++;
        }
    }

    //trailer
    reader_to_pc_datablock[responseDataBufferIndex] = response->SW1;
    responseDataBufferIndex++;

    reader_to_pc_datablock[responseDataBufferIndex] = response->SW2;
    responseDataBufferIndex++;

    *reader_to_pc_datablock_len = responseDataBufferIndex;
}
