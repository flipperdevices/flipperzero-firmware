#include "tcode.h"

static TCodeCommandArray decode_device_command(const uint8_t* buffer, uint16_t size) {
    TCodeCommandArray command_array;
    if(size < 2) {
        command_array.size = 0;
        return command_array;
    }

    command_array.size = 1;
    command_array.commands = malloc(sizeof(TCodeCommand));
    command_array.commands[0].command_type = Device;

    switch(buffer[1]) {
    case '0':
        command_array.commands[0].data.device_command = DeviceIdentification;
        break;
    case '1':
        command_array.commands[0].data.device_command = TCodeVersion;
        break;
    case '2':
        command_array.commands[0].data.device_command = ListAxesAndUserRangePreferences;
        break;
    case 'S':
        command_array.commands[0].data.device_command = Stop;
        break;
    default:
        command_array.size = 0;
        break;
    }

    return command_array;
}

static TCodeCommandArray decode_general_command(const uint8_t* buffer, uint16_t size) {
    // size of the array = amount of spaces + 1
    uint16_t counter = 1;
    for(uint16_t i = 0; i < size; i++) {
        if(buffer[i] == 32) {
            counter++;
        }
    }

    TCodeCommandArray commands_array;
    commands_array.size = counter;
    commands_array.commands = malloc(commands_array.size * sizeof(TCodeCommand));

    uint16_t position = 0;
    for(uint16_t i = 0; i < counter; i++) {
        TCodeCommand command;

        TCodeCommandMotionType motion_type;
        switch(buffer[position]) {
        case 'L':
        case 'l':
            motion_type = Linear;
            break;
        case 'R':
        case 'r':
            motion_type = Rotate;
            break;
        case 'V':
        case 'v':
            motion_type = Vibrate;
            break;
        case 'A':
        case 'a':
            motion_type = Auxiliary;
            break;
        default: // error
            return commands_array;
        }
        position++;

        uint16_t channel = buffer[position] - 48; // single ascii character 0-9
        position++;

        // X characters that are digits
        uint16_t current_position = position;
        while(buffer[position] >= 48 && buffer[position] <= 57 && position < size) {
            position++;
        }

        uint8_t* magnitude = malloc(2 + (position - current_position) + 1); // "0.XXXX\0"
        magnitude[0] = '0';
        magnitude[1] = '.';
        for(uint16_t x = 0; x < (position - current_position); x++) {
            magnitude[x + 2] = buffer[current_position + x];
        }
        magnitude[position - current_position + 2] = '\0';
        float magnitude_float = strtof((char*)magnitude, NULL);
        free(magnitude);

        if(position == size || buffer[position] == ' ') {
            command.command_type = Magnitude;
            command.data.magnitude_command.motion_type = motion_type;
            command.data.magnitude_command.channel_id = channel;
            command.data.magnitude_command.magnitude = magnitude_float;
            commands_array.commands[i] = command;
            position++;
            continue;
        }

        uint8_t current_step = buffer[position];
        position++;

        uint16_t int_value = 0;
        while(buffer[position] >= 48 && buffer[position] <= 57 && position < size) {
            int_value *= 10;
            int_value += buffer[position] - 48;
            position++;
        }

        command.data.magnitude_time_interval_command.motion_type = motion_type;
        command.data.magnitude_time_interval_command.channel_id = channel;
        command.data.magnitude_time_interval_command.magnitude = magnitude_float;

        if(current_step == 'I' || current_step == 'i') {
            command.command_type = MagnitudeTimeInterval;
            command.data.magnitude_time_interval_command.time_interval_milliseconds = int_value;
        }

        if(current_step == 'S' || current_step == 's') {
            command.command_type = MagnitudeSpeed;
            command.data.magnitude_speed_command.speed_per_hundred_milliseconds = int_value;
        }

        commands_array.commands[i] = command;
        position++;
        if(position >= size) {
            break;
        }
    }

    return commands_array;
}

TCodeCommandArray tcode_decode(uint8_t* buffer, uint16_t size) {
    switch(buffer[0]) {
    case 'd':
    case 'D':
        return decode_device_command(buffer, size);
    case 'l':
    case 'L':
    case 'r':
    case 'R':
    case 'v':
    case 'V':
    case 'a':
    case 'A':
        return decode_general_command(buffer, size);
    default: // error
    {
        TCodeCommandArray error;
        error.size = 0;
        error.commands = NULL;
        return error;
    }
    }
}