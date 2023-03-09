import os
import sys
import serial
import time
import hashlib
import math
import logging
import posixpath


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
        eol = eol.encode("ascii")
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

    def __init__(self, portname: str, chunk_size: int = 8192):
        self.port = serial.Serial()
        self.port.port = portname
        self.port.timeout = 2
        self.port.baudrate = 115200  # Doesn't matter for VCP
        self.read = BufferedRead(self.port)
        self.chunk_size = chunk_size

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.stop()

    def start(self):
        self.port.open()
        self.port.reset_input_buffer()
        # Send a command with a known syntax to make sure the buffer is flushed
        self.send("device_info\r")
        self.read.until("hardware_model")
        # And read buffer until we get prompt
        self.read.until(self.CLI_PROMPT)

    def stop(self):
        self.port.close()

    def send(self, line):
        self.port.write(line.encode("ascii"))

    def send_and_wait_eol(self, line):
        self.send(line)
        return self.read.until(self.CLI_EOL)

    def send_and_wait_prompt(self, line):
        self.send(line)
        return self.read.until(self.CLI_PROMPT)

    def has_error(self, data):
        """Is data an error message"""
        return data.find(b"Storage error:") != -1

    def get_error(self, data):
        """Extract error text from data and print it"""
        _, error_text = data.decode("ascii").split(": ")
        return error_text.strip()

    def list_tree(self, path="/", level=0):
        """List files and dirs on Flipper"""
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

            if self.has_error(line.encode("ascii")):
                print(self.get_error(line.encode("ascii")))
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
        self.send_and_wait_eol(f'storage list "{path}"\r')
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

            if self.has_error(line.encode("ascii")):
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

        # topdown walk, yield before recursing
        yield path, dirs, nondirs
        for new_path in walk_dirs:
            yield from self.walk(new_path)

    def send_file(self, filename_from, filename_to):
        """Send file from local device to Flipper"""
        if self.exist_file(filename_to):
            self.remove(filename_to)

        with open(filename_from, "rb") as file:
            filesize = os.fstat(file.fileno()).st_size

            buffer_size = self.chunk_size
            while True:
                filedata = file.read(buffer_size)
                size = len(filedata)
                if size == 0:
                    break

                self.send_and_wait_eol(f'storage write_chunk "{filename_to}" {size}\r')
                answer = self.read.until(self.CLI_EOL)
                if self.has_error(answer):
                    last_error = self.get_error(answer)
                    self.read.until(self.CLI_PROMPT)
                    raise FlipperStorageException(last_error)

                self.port.write(filedata)
                self.read.until(self.CLI_PROMPT)

                percent = str(math.ceil(file.tell() / filesize * 100))
                total_chunks = str(math.ceil(filesize / buffer_size))
                current_chunk = str(math.ceil(file.tell() / buffer_size))
                sys.stdout.write(
                    f"\r{percent}%, chunk {current_chunk} of {total_chunks}"
                )
                sys.stdout.flush()
        print()

    def read_file(self, filename):
        """Receive file from Flipper, and get filedata (bytes)"""
        buffer_size = self.chunk_size
        self.send_and_wait_eol(
            'storage read_chunks "' + filename + '" ' + str(buffer_size) + "\r"
        )
        answer = self.read.until(self.CLI_EOL)
        filedata = bytearray()
        if self.has_error(answer):
            last_error = self.get_error(answer)
            self.read.until(self.CLI_PROMPT)
            raise FlipperStorageException(last_error)
            # return filedata
        size = int(answer.split(b": ")[1])
        read_size = 0

        while read_size < size:
            self.read.until("Ready?" + self.CLI_EOL)
            self.send("y")
            chunk_size = min(size - read_size, buffer_size)
            filedata.extend(self.port.read(chunk_size))
            read_size = read_size + chunk_size

            percent = str(math.ceil(read_size / size * 100))
            total_chunks = str(math.ceil(size / buffer_size))
            current_chunk = str(math.ceil(read_size / buffer_size))
            sys.stdout.write(f"\r{percent}%, chunk {current_chunk} of {total_chunks}")
            sys.stdout.flush()
        print()
        self.read.until(self.CLI_PROMPT)
        return filedata

    def receive_file(self, filename_from, filename_to):
        """Receive file from Flipper to local storage"""
        with open(filename_to, "wb") as file:
            data = self.read_file(filename_from)
            file.write(data)

    def exist(self, path):
        """Does file or dir exist on Flipper"""
        self.send_and_wait_eol('storage stat "' + path + '"\r')
        response = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        return not self.has_error(response)

    def exist_dir(self, path):
        """Does dir exist on Flipper"""
        self.send_and_wait_eol('storage stat "' + path + '"\r')
        response = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        return response.find(b"Directory") != -1 or response.find(b"Storage") != -1

    def exist_file(self, path):
        """Does file exist on Flipper"""
        self.send_and_wait_eol('storage stat "' + path + '"\r')
        response = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        return response.find(b"File, size:") != -1

    def _check_no_error(self, response):
        if self.has_error(response):
            raise FlipperStorageException(self.get_error(response))

    def size(self, path):
        """file size on Flipper"""
        self.send_and_wait_eol('storage stat "' + path + '"\r')
        response = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        self._check_no_error(response)
        if response.find(b"File, size:") != -1:
            size = int(
                "".join(
                    ch
                    for ch in response.split(b": ")[1].decode("ascii")
                    if ch.isdigit()
                )
            )
            return size

        raise FlipperStorageException("Not a file")

    def mkdir(self, path):
        """Create a directory on Flipper"""
        self.send_and_wait_eol('storage mkdir "' + path + '"\r')
        response = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        self._check_no_error(response)

    def format_ext(self):
        """Create a directory on Flipper"""
        self.send_and_wait_eol("storage format /ext\r")
        self.send_and_wait_eol("y\r")
        response = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        self._check_no_error(response)

    def remove(self, path):
        """Remove file or directory on Flipper"""
        self.send_and_wait_eol('storage remove "' + path + '"\r')
        response = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        self._check_no_error(response)

    def hash_local(self, filename):
        """Hash of local file"""
        hash_md5 = hashlib.md5()
        with open(filename, "rb") as f:
            for chunk in iter(lambda: f.read(self.chunk_size), b""):
                hash_md5.update(chunk)
        return hash_md5.hexdigest()

    def hash_flipper(self, filename):
        """Get hash of file on Flipper"""
        self.send_and_wait_eol('storage md5 "' + filename + '"\r')
        hash = self.read.until(self.CLI_EOL)
        self.read.until(self.CLI_PROMPT)

        self._check_no_error(hash)
        return hash.decode("ascii")


class FlipperStorageException(Exception):
    pass


class FlipperStorageOperations:
    def __init__(self, storage):
        self.storage: FlipperStorage = storage
        self.logger = logging.getLogger("FStorageOps")

    def send_file_to_storage(self, flipper_file_path, local_file_path, force):
        self.logger.debug(
            f"send_file_to_storage:  {local_file_path}->{flipper_file_path}, {force=}"
        )
        exists = self.storage.exist_file(flipper_file_path)
        do_upload = not exists
        if exists:
            hash_local = self.storage.hash_local(local_file_path)
            hash_flipper = self.storage.hash_flipper(flipper_file_path)
            self.logger.debug(f"hash check: local {hash_local}, flipper {hash_flipper}")
            do_upload = force or (hash_local != hash_flipper)

        if do_upload:
            self.logger.info(f'Sending "{local_file_path}" to "{flipper_file_path}"')
            self.storage.send_file(local_file_path, flipper_file_path)

    # make directory with exist check
    def mkdir_on_storage(self, flipper_dir_path):
        if not self.storage.exist_dir(flipper_dir_path):
            self.logger.debug(f'"{flipper_dir_path}" does not exist, creating')
            self.storage.mkdir(flipper_dir_path)
        else:
            self.logger.debug(f'"{flipper_dir_path}" already exists')

    # send file or folder recursively
    def send_to_storage(self, flipper_path, local_path, force):
        if not os.path.exists(local_path):
            raise FlipperStorageException(f'Error: "{local_path}" is not exist')

        if os.path.isdir(local_path):
            # create parent dir
            self.mkdir_on_storage(flipper_path)

            for dirpath, dirnames, filenames in os.walk(local_path):
                self.logger.debug(f'Processing directory "{os.path.normpath(dirpath)}"')
                dirnames.sort()
                filenames.sort()
                rel_path = os.path.relpath(dirpath, local_path)

                # create subdirs
                for dirname in dirnames:
                    flipper_dir_path = os.path.join(flipper_path, rel_path, dirname)
                    flipper_dir_path = os.path.normpath(flipper_dir_path).replace(
                        os.sep, "/"
                    )
                    self.mkdir_on_storage(flipper_dir_path)

                # send files
                for filename in filenames:
                    flipper_file_path = os.path.join(flipper_path, rel_path, filename)
                    flipper_file_path = os.path.normpath(flipper_file_path).replace(
                        os.sep, "/"
                    )
                    local_file_path = os.path.normpath(os.path.join(dirpath, filename))
                    self.send_file_to_storage(flipper_file_path, local_file_path, force)
        else:
            self.send_file_to_storage(flipper_path, local_path, force)

    def receive(self, flipper_path, local_path):
        if self.storage.exist_dir(flipper_path):
            for dirpath, dirnames, filenames in self.storage.walk(flipper_path):
                self.logger.debug(
                    f'Processing directory "{os.path.normpath(dirpath)}"'.replace(
                        os.sep, "/"
                    )
                )
                dirnames.sort()
                filenames.sort()

                rel_path = os.path.relpath(dirpath, flipper_path)

                for dirname in dirnames:
                    local_dir_path = os.path.join(local_path, rel_path, dirname)
                    local_dir_path = os.path.normpath(local_dir_path)
                    os.makedirs(local_dir_path, exist_ok=True)

                for filename in filenames:
                    local_file_path = os.path.join(local_path, rel_path, filename)
                    local_file_path = os.path.normpath(local_file_path)
                    flipper_file_path = os.path.normpath(
                        os.path.join(dirpath, filename)
                    ).replace(os.sep, "/")
                    self.logger.info(
                        f'Receiving "{flipper_file_path}" to "{local_file_path}"'
                    )
                    self.storage.receive_file(flipper_file_path, local_file_path)

        else:
            self.logger.info(f'Receiving "{flipper_path}" to "{local_path}"')
            self.storage.receive_file(flipper_path, local_path)
