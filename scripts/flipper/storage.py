import os
import serial
import time
import hashlib
import math


def timing(func):
    """
    Speedometer decorator
    """

    def wrapper(*args, **kwargs):
        time1 = time.monotonic()
        ret = func(*args, **kwargs)
        time2 = time.monotonic()
        print(
            "{:s} function took {:.3f} ms".format(
                func.__name__, (time2 - time1) * 1000.0
            )
        )
        return ret

    return wrapper


class BufferedRead:
    def __init__(self, stream):
        self.buffer = bytearray()
        self.stream = stream

    def until(self, eol="\n", cut_eol=True):
        eol = eol.encode()
        while True:
            # search in buffer
            i = self.buffer.find(eol)
            if i >= 0:
                if cut_eol:
                    read = self.buffer[:i]
                else:
                    read = self.buffer[: i + len(eol)]
                self.buffer = self.buffer[i + len(eol) :]
                return read

            # read and append to buffer
            i = max(1, self.stream.in_waiting)
            data = self.stream.read(i)
            self.buffer.extend(data)


class FlipperStorage:
    CLI_PROMPT = ">: "
    CLI_EOL = "\r\n"

    def __init__(self, portname: str):
        self.port = serial.Serial()
        self.port.port = portname
        self.port.timeout = 2
        self.port.baudrate = 115200
        self.read = BufferedRead(self.port)
        self.last_error = ""

    def start(self):
        self.port.open()
        self.port.reset_input_buffer()

        # Send a command with a known syntax to make sure the buffer is flushed
        self.send("device_info\r")
        self.read.until("hardware_ver        : ")
        # And read buffer until we get prompt
        self.read.until(self.CLI_PROMPT)

    def stop(self):
        self.port.close()

    def send(self, line):
        self.port.write(line.encode())

    def send_and_wait_eol(self, line):
        self.send(line)
        return self.read.until(self.CLI_EOL)

    def send_and_wait_prompt(self, line):
        self.send(line)
        return self.read.until(self.CLI_PROMPT)

    # Is data has error
    def has_error(self, data):
        if data.find(b"Storage error") != -1:
            return True
        else:
            return False

    # Extract error text from data and print it
    def get_error(self, data):
        error, error_text = data.decode("ascii").split(": ")
        return error_text.strip()

    # List files and dirs on Flipper
    def list_tree(self, path="/", level=0):
        path = path.replace("//", "/")

        self.send_and_wait_eol('storage list "' + path + '"\r')

        data = self.read.until(self.CLI_PROMPT)
        lines = data.split(b"\r\n")

        for line in lines:
            try:
                # TODO: better decoding, considering non-ascii characters
                line = line.decode("ascii")
            except:
                continue

            line = line.strip()

            if len(line) == 0:
                continue

            if self.has_error(line.encode()):
                print(self.get_error(line.encode()))
                continue

            if line == "Empty":
                continue

            type, info = line.split(" ", 1)
            if type == "[D]":
                # Print directory name
                print((path + "/" + info).replace("//", "/"))
                # And recursively go inside
                self.list_tree(path + "/" + info, level + 1)
            elif type == "[F]":
                name, size = info.rsplit(" ", 1)
                # Print file name and size
                print((path + "/" + name).replace("//", "/") + ", size " + size)
            else:
                # Something wrong, pass
                pass

    def walk(self, path="/"):
        dirs = []
        nondirs = []
        walk_dirs = []

        path = path.replace("//", "/")
        self.send_and_wait_eol('storage list "' + path + '"\r')
        data = self.read.until(self.CLI_PROMPT)
        lines = data.split(b"\r\n")

        for line in lines:
            try:
                # TODO: better decoding, considering non-ascii characters
                line = line.decode("ascii")
            except:
                continue

            line = line.strip()

            if len(line) == 0:
                continue

            if self.has_error(line.encode()):
                continue

            if line == "Empty":
                continue

            type, info = line.split(" ", 1)
            if type == "[D]":
                # Print directory name
                dirs.append(info)
                walk_dirs.append((path + "/" + info).replace("//", "/"))

            elif type == "[F]":
                name, size = info.rsplit(" ", 1)
                # Print file name and size
                nondirs.append(name)
            else:
                # Something wrong, pass
                pass

        # topdown walk, yield before recursy
        yield path, dirs, nondirs
        for new_path in walk_dirs:
            yield from self.walk(new_path)

    # Send file from local device to Flipper
    def send_file(self, filename_from, filename_to):
        self.remove(filename_to)

        file = open(filename_from, "rb")
        filesize = os.fstat(file.fileno()).st_size

        buffer_size = 512
        while True:
            filedata = file.read(buffer_size)
            size = len(filedata)
            if size == 0:
                break

            self.send_and_wait_eol(
                'storage write_chunk "' + filename_to + '" ' + str(size) + "\r"
            )
            answer = self.read.until(self.CLI_EOL)
            if self.has_error(answer):
                self.last_error = self.get_error(answer)
                self.read.until(self.CLI_PROMPT)
                file.close()
                return False

            self.port.write(filedata)
            self.read.until(self.CLI_PROMPT)

            percent = str(math.ceil(file.tell() / filesize * 100))
            total_chunks = str(math.ceil(filesize / buffer_size))
            current_chunk = str(math.ceil(file.tell() / buffer_size))
            print(
                percent + "%, chunk " + current_chunk + " of " + total_chunks, end="\r"
            )
        file.close()
        print()
        return True

    # Receive file from Flipper, and get filedata (bytes)
    def read_file(self, filename):
        buffer_size = 512
        self.send_and_wait_eol(
            'storage read_chunks "' + filename + '" ' + str(buffer_size) + "\r"
        )
        answer = self.read.until(self.CLI_EOL)
        filedata = bytearray()
        if self.has_error(answer):
            self.last_error = self.get_error(answer)
            self.read.until(self.CLI_PROMPT)
            return filedata
        size = int(answer.split(b": ")[1])
        readed_size = 0

        while readed_size < size:
            self.read.until("Ready?" + self.CLI_EOL)
            self.send("y")
            read_size = min(size - readed_size, buffer_size)
            filedata.extend(self.port.read(read_size))
            readed_size = readed_size + read_size

            percent = str(math.ceil(readed_size / size * 100))
            total_chunks = str(math.ceil(size / buffer_size))
            current_chunk = str(math.ceil(readed_size / buffer_size))
            print(
                percent + "%, chunk " + current_chunk + " of " + total_chunks, end="\r"
            )
        print()
        self.read.until(self.CLI_PROMPT)
        return filedata

    # Receive file from Flipper to local storage
    def receive_file(self, filename_from, filename_to):
        with open(filename_to, "wb") as file:
            data = self.read_file(filename_from)
            if not data:
                return False
            else:
                file.write(data)
                return True

    # Is file or dir exist on Flipper
    def exist(self, path):
        self.send_and_wait_eol('storage stat "' + path + '"\r')
        answer = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        if self.has_error(answer):
            self.last_error = self.get_error(answer)
            return False
        else:
            return True

    # Is dir exist on Flipper
    def exist_dir(self, path):
        self.send_and_wait_eol('storage stat "' + path + '"\r')
        answer = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        if self.has_error(answer):
            self.last_error = self.get_error(answer)
            return False
        else:
            if answer.find(b"Directory") != -1:
                return True
            elif answer.find(b"Storage") != -1:
                return True
            else:
                return False

    # Is file exist on Flipper
    def exist_file(self, path):
        self.send_and_wait_eol('storage stat "' + path + '"\r')
        answer = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        if self.has_error(answer):
            self.last_error = self.get_error(answer)
            return False
        else:
            if answer.find(b"File, size:") != -1:
                return True
            else:
                return False

    # file size on Flipper
    def size(self, path):
        self.send_and_wait_eol('storage stat "' + path + '"\r')
        answer = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        if self.has_error(answer):
            self.last_error = self.get_error(answer)
            return False
        else:
            if answer.find(b"File, size:") != -1:
                size = int(
                    "".join(
                        ch for ch in answer.split(b": ")[1].decode() if ch.isdigit()
                    )
                )
                return size
            else:
                self.last_error = "access denied"
                return -1

    # Create a directory on Flipper
    def mkdir(self, path):
        self.send_and_wait_eol('storage mkdir "' + path + '"\r')
        answer = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        if self.has_error(answer):
            self.last_error = self.get_error(answer)
            return False
        else:
            return True

    # Remove file or directory on Flipper
    def remove(self, path):
        self.send_and_wait_eol('storage remove "' + path + '"\r')
        answer = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        if self.has_error(answer):
            self.last_error = self.get_error(answer)
            return False
        else:
            return True

    # Hash of local file
    def hash_local(self, filename):
        hash_md5 = hashlib.md5()
        with open(filename, "rb") as f:
            for chunk in iter(lambda: f.read(4096), b""):
                hash_md5.update(chunk)
        return hash_md5.hexdigest()

    # Get hash of file on Flipper
    def hash_flipper(self, filename):
        self.send_and_wait_eol('storage md5 "' + filename + '"\r')
        hash = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        if self.has_error(hash):
            self.last_error = self.get_error(hash)
            return ""
        else:
            return hash.decode("ascii")
