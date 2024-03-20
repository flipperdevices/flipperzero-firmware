# FindMy

Query Apple's Find My network, allowing none Apple devices to retrieve the location reports.

This was made possible thanks to Chapoly1305 and his work bringing airtag tracking to non Apple devices:
1. https://github.com/Chapoly1305/FindMy
2. https://github.com/seemoo-lab/openhaystack/
3. https://github.com/biemster/FindMy
4. https://github.com/Dadoum/anisette-v3-server
5. https://github.com/mrmay-dev/owntags

Please give Chapoly1305 a star for his work, thanks!
He wrote these instructions and provided an in-depth tutorial on how to use it. I just adapted this for the FLipperZero

## Schematic and Usage

### generate_keys.py
Use the `generate_keys.py` script to generate the required keys. The script will generate a `.keys`
or multiple files for each device you want to use. Each `.keys` file will contain the private key, the public key
(also called advertisement key) and the hashed advertisement key. As the name suggests, the private key is a secret
and should not be shared. The public key (advertisement key) is used for broadcasting the BLE message, the hashed advertisement key is for requesting location
reports from Apple.

### request_reports.py
Use the `request_reports.py` script to request location reports from Apple. The script will read the `.keys` files and
request location reports for each device. The script will also attempt to log in and provided Apple account and save
the session cookies in `auth.json` file. The reports are stored in the `reports` database.

### web_service.py
Use the `web_service.py` script to start a web service that will serve the location reports via its API.

### anisette-v3-server

Q: What does this external project do? The SMS code is only asked once, in where and how is the information stored?

A: Anisette is similar to a device fingerprint. It is intended to be stored on an Apple device once it becomes trusted. 
Subsequent requests made by this "device" using this fingerprint and same Apple account will not trigger the 2FA again. 
The first call (icloud_login_mobileme) is used to obtain the search party token. The subsequent calls 
(generate_anisette_headers) use the cached search party token from the first call as the password and the dsid as the 
username. I (@biemster) have observed that the search party tokens change when using different sources for anisette data,
possibly due to various reasons. If it's deployed as a docker container, the storage location is $HOME/.config/anisette-v3/
adi.pb and device.json. These numbers together generate a validation code like OTP, which then undergoes a process of 
Mixed Boolean Arithmetic to produce two anisette headers for the request. One header represents a relatively static 
machine serial, while the other header contains a frequently changing OTP. 
If you switch to https://github.com/Dadoum/pyprovision, you will obtain the ADI data in the anisette folder. 
(Answer revised and organized from https://github.com/biemster/FindMy/issues/37#issuecomment-1840277808)

## Installation and initial setup
This project only need a free Apple ID with SMS 2FA properly setup. If you don't have any, follow one of the many 
guides found on the internet. 

**Using your personal Apple ID is strongly discouraged, I recommended to create a blank 
Apple ID for experimental purpose.**  If you ran into issue of "KeyError service-data", especially you are using an existing account rather than a new account, you may want to refer to https://github.com/Chapoly1305/FindMy/issues/9 .


📺 Installation and Walkthrough Video: https://youtu.be/yC2HIPDSxlM


### Steps

1. Install docker and Python3-pip. [How to Install Docker on Ubuntu](https://docs.docker.com/engine/install/ubuntu/). Python3-venv is also strongly recommended.

2. The anisetter service shall run on the same device of this project. For Linux system, deploy with docker is recommended.
If the system rebooted, this docker service will automatically start after reboot.

```bash
docker run -d --restart always --name anisette-v3 -p 6969:6969 dadoum/anisette-v3-server:latest
```
If docker method didn't work, see the end of this section for manual setup.

3. After deployed `anisette-v3-server`, you may validate the service is running by sending a `curl` request:

```bash
curl -I http://localhost:6969
```
4. Clone this repository, Navigate to `FindMy` directory, and install the required python packages:

```bash
git clone https://github.com/Chapoly1305/FindMy.git
cd FindMy
pip3 install -r requirements.txt
```


<br>

The [anisette-v3 docker image](https://hub.docker.com/r/dadoum/anisette-v3-server/tags) shall be available 
for x86, x86-64, arm, and arm64. In case of the docker container is not available, you may start it by setup 
anisette-v3-server manually.
Follow the installation instructions for [anisette-v3-server](https://github.com/Dadoum/anisette-v3-server
) project. 


## Run 
You may run this project as a local service, a web service, or both. 

### Key generation
```bash
./generate_keys.py # Without any arguments, it will generate a single key file and save under current directory.
```

Deploy your advertisement keys on devices supported by OpenHaystack. The ESP32 firmware is a mirror of the 
OpenHaystack binary, the Lenze 17H66 is found in many 1$ tags obtained from Ali. 
An nRF51 firmware can be found here: https://github.com/dakhnod/FakeTag

### as a local service and write to database

The anisetter docker service shall run on the same device of this project. 
If the anisetter has started, then run:

```bash
./request_reports.py # Without any arguments, it will read all the .keys files under current directory.
```

### as a web service
**You are advised to configure adequate firewall rules to protect the web service.**

To run as a web service, firstly install uvicorn by 
```
pip3 install uvicorn
```

then run 
```bash
uvicorn web_service:app --reload
```

or 
```bash
./web_service.py
```
The difference between the two commands is that the first one will reload the web service when the source code is changed. 
Each time the web_service.py is modified and saved, the web service will auto reload. Quite useful for development.

This web service will die if the shell exited or system reboot. You could use `nohup`, `screen`, or set up a systemd service to keep it alive.
```bash
nohup uvicorn web_service:app --reload &
```
---

<br>

## API Usage
The APIs are created with FastAPI, the documentations are written inline and can be accessed on website 
path http://127.0.0.1:8000/docs or http://127.0.0.1:8000/redoc. 
The project is currently under development, which may lead to frequent changes of the APIs. Please use with cautious.
