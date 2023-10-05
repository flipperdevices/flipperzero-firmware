# Multi Fuzzer
## Flipper Zero app for checking LF RFID readers .

**Not meant for any illegal activity/purposes.**

You can check your readers for vulnerabilities using the default UIDs and frequently used UIDs.

The application will also help to identify the "denial of service" vulnerability when providing an incorrect uid or when accessing the reader too often.

## Supported protocols
- EM4100
- HIDProx
- PAC/Stanley
- H10301

# Application Features
## Main screen
- **Header** - selected protocol
- **Menu** - available attacks
    - **Default Values** - Using the dictionary from the app
    - **Load UIDs from file** - Loading a custom dictionary from an SD card
    - **Load file** - Loading UID from ***FlipperFormat*** key file with the ability to edit and further iterate over the selected byte
    - **BFCustomer ID** - Iterates over the selected byte with the remaining bytes equal to zero

## Attack screen
- **Header** - selected attack
- **2nd line**
    - **Time delay (TD)** - idle time between UID submissions
    - **Emulation time (EmT)** - transmission time of one UID
- **3rd line** - Prtocol name
- **4th line** - Current UID