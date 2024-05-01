import argparse
import mido
import serial
import time


def midi_to_serial(port_name, file_path, baud_rate, tempo_override=None):
    try:
        with serial.Serial(port_name, baud_rate) as ser:
            print(f"Opening MIDI file: {file_path}")
            midi_file = mido.MidiFile(file_path)

            print(f"Sending MIDI messages to serial port: {port_name}")

            # Override the tempo if specified
            if tempo_override is not None:
                original_tempo = midi_file.ticks_per_beat
                new_tempo = int(
                    original_tempo
                    * (
                        midi_file.ticks_per_beat
                        / (tempo_override * midi_file.ticks_per_beat / 60)
                    )
                )
                midi_file.ticks_per_beat = new_tempo

            # Iterate through the MIDI messages in real-time
            for msg in midi_file.play():
                if msg.type == "note_on" or msg.type == "note_off":
                    channel = msg.channel
                    note = msg.note
                    velocity = msg.velocity

                    if not (
                        0 <= channel <= 15 and 0 <= note <= 127 and 0 <= velocity <= 127
                    ):
                        print(f"Invalid MIDI message: {msg}")
                        continue

                    if msg.type == "note_on":
                        # Note On message
                        status_byte = 0x90
                        midi_bytes = [status_byte | channel, note, velocity]
                        ser.write(bytes(midi_bytes))
                        print(
                            f"Sent message: {msg.type}, {[hex(b) for b in midi_bytes]}"
                        )
                    elif msg.type == "note_off":
                        # Note Off message
                        status_byte = 0x80
                        midi_bytes = [status_byte | channel, note, velocity]
                        ser.write(bytes(midi_bytes))
                        print(
                            f"Sent message: {msg.type}, {[hex(b) for b in midi_bytes]}"
                        )

            print("Finished sending MIDI messages.")
    except (OSError, serial.SerialException) as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Convert MIDI file to serial messages and send them to a serial port."
    )
    parser.add_argument("-p", "--port", required=True, help="Serial port name")
    parser.add_argument("-i", "--input", required=True, help="MIDI file path")
    parser.add_argument(
        "-b", "--baud", type=int, default=31250, help="Baud rate (default: 31250)"
    )
    parser.add_argument("-t", "--tempo", type=float, help="Override tempo (in BPM)")
    args = parser.parse_args()

    midi_to_serial(args.port, args.input, args.baud, args.tempo)
