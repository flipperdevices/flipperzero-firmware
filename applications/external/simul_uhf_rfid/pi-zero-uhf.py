from __future__ import print_function
from datetime import datetime
import serial
import time
import mercury
import re
import time

# Defining the virtual serial port used for the M6E Nano Reader communication
reader_virtual_serial = "tmr:///dev/serial0"

# Defining the physical serial port used for the flipper zero Connection
serial_port = "/dev/ttySOFT0"

# Setting variables used for the reader
baud_rate = 9600
reader_power = 1500
connected = False
reader = "undefined"

# Lists for the epcs, reserved, and user memory
epc_list = []
res_list = []
user_mem_list = []


# Extracts the string for the EPC value
def parse_epc_string(s, epc_length=24):
    epc_start = 0
    prefix = s[:epc_start].strip()
    old_epc = s[epc_start : epc_start + epc_length]
    new_epc = s[epc_start + epc_length :].strip()
    return prefix, old_epc, new_epc[0:epc_length]


# Extracts the user memory value
def parse_mem_string(s, epc_length=40):
    epc_start = 0
    prefix = s[:epc_start].strip()
    old_epc = s[epc_start : epc_start + 24]
    new_epc = s[24:].strip()
    return prefix, old_epc, new_epc


# Parses the reserved memory string
def parse_res_string(s, epc_length=40):
    epc_start = 0
    prefix = s[:epc_start].strip()
    old_epc = s[epc_start : epc_start + 24]
    new_epc = s[24:].strip()
    return prefix, old_epc, new_epc


# Parses the TID string
def parse_tid_string(s, epc_length=40):
    epc_start = 0
    prefix = s[:epc_start].strip()
    old_epc = s[epc_start : epc_start + 24]
    new_epc = s[24:].strip()
    return prefix, old_epc, new_epc


# Connects to the reader
def reader_connect():
    global reader
    if reader == "undefined":
        try:
            reader = mercury.Reader(reader_virtual_serial, baudrate=115200)
            reader.set_region("NA2")
            reader.set_read_plan([1], "GEN2", read_power=reader_power)
            return True
        except:
            print("Error Connecting to reader\n")
            return False
    else:
        return True


# Formats the string to begin with a b
def extract_content_from_format(input_string):
    if (
        input_string.startswith("b'")
        and input_string.endswith("'")
        and len(input_string) > 3
    ):
        return input_string[2:-1]
    else:
        return ""


# Format the file to length X
def format_byte_array_to_X_bytes(byte_array, x):
    input_length = len(byte_array)

    if input_length == x:
        return byte_array.hex()

    elif input_length < x:
        padding_length = x - input_length
        padded_byte_array = byte_array + b"\x00" * padding_length
        return padded_byte_array.hex()

    else:
        truncated_byte_array = byte_array[:x]
        return truncated_byte_array.hex()


# Function to handle the serial commands
def handle_command(command, ser):
    global connected
    global epc_list
    global res_list
    global user_mem_list

    # Handle the write command for EPCs
    if command == "WRITE":
        if connected:
            value1 = ser.readline().decode().strip()
            prefix, old_epc, new_epc = parse_epc_string(value1)
            old_epc = old_epc.encode()
            new_epc = new_epc.encode()

            if reader.write(epc_code=new_epc, epc_target=old_epc):
                ser.write(b"EVOK\n")
            else:
                ser.write(b"EVBAD\n")
        else:
            print("error writing")

    # Handle the write TID command
    elif command == "WRITETID":
        if connected:
            value1 = ser.readline().decode().strip()
            prefix, old_epc, new_epc = parse_tid_string(value1)
            old_epc = old_epc.encode()
            new_epc = new_epc.encode()

            if reader.write_tag_mem(2, 0x00, new_epc, old_epc):
                ser.write(b"TVOK\n")
            else:
                ser.write(b"TVBAD\n")

    # Handle the Write User Memory Command
    elif command == "WRITEUSR":
        if connected:
            value1 = ser.readline().decode().strip()
            prefix, old_epc, new_epc = parse_mem_string(value1)
            byte_array = bytearray.fromhex(new_epc)
            old_epc = old_epc.encode()

            if reader.write_tag_mem(3, 0x00, byte_array, old_epc):
                ser.write(b"UVOK\n")
            else:
                ser.write(b"UVBAD\n")

        else:
            print("error writing")

    # Handle the write Reserved Memory Command
    elif command == "WRITERES":
        if connected:
            value1 = ser.readline().decode().strip()
            prefix, old_epc, new_epc = parse_res_string(value1)
            byte_array = bytearray.fromhex(new_epc)
            old_epc = old_epc.encode()

            if reader.write_tag_mem(1, 0x00, byte_array, old_epc):
                ser.write(b"RVOK\n")
            else:
                ser.write(b"RVBAD\n")

        else:
            print("error writing")

    # Handle the Read Command
    elif command == "READ":
        if connected:
            try:
                epcs = map(lambda tag: tag.epc, reader.read())

                epc_list = list(epcs)
                tid_list = []
                res_list = []
                user_mem_list = []
                for epc in epc_list:
                    input_byte_array = reader.read_tag_mem(2, 0x00, 20, epc)
                    input_reserved_arr = reader.read_tag_mem(0, 0x00, 8, epc)
                    input_user_mem_arr = reader.read_tag_mem(3, 0x00, 16, epc)

                    # Filling in any missing values if the Tag is locked
                    if input_byte_array != None:
                        formatted_byte_array_str = format_byte_array_to_X_bytes(
                            input_byte_array, 20
                        )
                        tid_list.append(formatted_byte_array_str)
                    else:
                        formatted_byte_array_str = (
                            "0000000000000000000000000000000000000000"
                        )
                        tid_list.append(formatted_byte_array_str)
                    if input_reserved_arr != None:
                        formatted_byte_array_res_str = format_byte_array_to_X_bytes(
                            input_reserved_arr, 8
                        )
                        res_list.append(formatted_byte_array_res_str)
                    else:
                        formatted_byte_array_res_str = "0000000000000000"
                        res_list.append(formatted_byte_array_res_str)

                    if input_user_mem_arr != None:
                        formatted_byte_array_mem_str = format_byte_array_to_X_bytes(
                            input_user_mem_arr, 16
                        )
                        user_mem_list.append(formatted_byte_array_mem_str)
                    else:
                        formatted_byte_array_mem_str = (
                            "00000000000000000000000000000000"
                        )
                        user_mem_list.append(formatted_byte_array_mem_str)

                ser.write(b"TID\n")
                number_of_tags_TID = (str(len(tid_list)) + "\n").encode()
                ser.write(number_of_tags_TID)

                for tid in tid_list:
                    tid = tid + "\n"
                    print(tid)
                    tid = tid.encode()
                    print(tid)
                    ser.write(tid)
                ser.write(b"end\n")

            except:
                print("Error Reading!")
        else:
            print("Please connect first")
        print("read here\n")
    # Handle the power command
    elif command == "POWER":
        value = ser.readline().decode().strip()
        if connected:
            try:
                reader.set_read_plan([1], "GEN2", read_power=int(value))
            except:
                print("Error setting reader power!")
        else:
            print("Please connect first")

    # Send the EPCS to the flipper app
    elif command == "EPCS":
        ser.write(b"TR\n")
        number_of_tags_read = (str(len(epc_list)) + "\n").encode()
        ser.write(number_of_tags_read)
        epc_values = [str(epc) for epc in epc_list]

        for epc in epc_values:
            epc = extract_content_from_format(epc)
            epc = epc + "b\n"
            epc = epc.encode()
            ser.write(epc)
        ser.write(b"end\n")
        epc_list = []

    # Send the reserved memory to the flipper app
    elif command == "RES":
        ser.write(b"RES\n")
        number_of_tags_read = (str(len(res_list)) + "\n").encode()
        ser.write(number_of_tags_read)
        res_values = [str(res) for res in res_list]

        for res in res_values:
            res = res + "\n"
            res = res.encode()
            ser.write(res)

        ser.write(b"end\n")
        res_list = []

    # Send the user memory to the flipper app
    elif command == "MEM":
        ser.write(b"MEM\n")
        number_of_tags_read = (str(len(user_mem_list)) + "\n").encode()
        ser.write(number_of_tags_read)
        mem_values = [str(mem) for mem in user_mem_list]

        for mem in mem_values:
            mem = mem + "\n"
            mem = mem.encode()
            ser.write(mem)

        ser.write(b"end\n")
        user_mem_list = []
    # This is just a place holder for now...
    elif command == "External":
        print("got a external\n")

    # This is just a place holder for now...
    elif command == "Internal":
        print("got a internal\n")

    # Connect to the reader
    elif command == "C":
        connected = reader_connect()
        ser.write(b"CONOK\n")

    # Disconnect from the reader
    elif command == "D":
        connected = False
        ser.write(b"DISOK\n")
    else:
        ser.write(b"unknown command\n")
        print(f"Unknown command {command}")


# Main function for the program that handles the M6E Nano Reader and communicates back with the flipper zero
def main():
    try:
        # Setup serial connection
        ser = serial.Serial(serial_port, baudrate=baud_rate, timeout=1)

        # Flush any existing input buffer
        ser.flushInput()

        while True:
            # Attempt to read a line of data from the RX buffer
            if ser.inWaiting() > 0:
                try:
                    # Attempt to decode the incoming data
                    incoming_data = ser.readline().decode("utf-8").rstrip()
                    handle_command(incoming_data, ser)
                except UnicodeDecodeError:
                    # Handle the decoding error
                    print(
                        "Error: Received data could not be decoded to UTF-8. Skipping this input."
                    )
                    continue

    except Exception as e:
        print(f"An error occurred: {str(e)}")

    finally:
        if "ser" in locals() or "ser" in globals():
            ser.close()
            print("Serial connection closed.")


if __name__ == "__main__":
    main()
