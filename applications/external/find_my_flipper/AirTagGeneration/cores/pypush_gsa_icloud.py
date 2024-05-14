from getpass import getpass
import plistlib as plist
import json
import uuid
import pbkdf2
import requests
import hashlib
import hmac
import base64
import locale
from datetime import datetime
import srp._pysrp as srp
from cryptography.hazmat.primitives import padding
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from Crypto.Hash import SHA256

# Created here so that it is consistent
USER_ID = uuid.uuid4()
DEVICE_ID = uuid.uuid4()

# Configure SRP library for compatibility with Apple's implementation
srp.rfc5054_enable()
srp.no_username_in_x()

# Disable SSL Warning
import urllib3

# urllib3.disable_warnings()

ANISETTE_URL = "http://localhost:6969"  # https://github.com/Dadoum/anisette-v3-server


def icloud_login_mobileme(username="", password="", second_factor="sms"):
    if not username:
        username = getpass("Apple ID: ")
    if not password:
        password = getpass("Password: ")

    g = gsa_authenticate(username, password, second_factor)
    pet = g["t"]["com.apple.gs.idms.pet"]["token"]
    adsid = g["adsid"]

    data = {
        "apple-id": username,
        "delegates": {"com.apple.mobileme": {}},
        "password": pet,
        "client-id": str(USER_ID),
    }
    data = plist.dumps(data)

    headers = {
        "X-Apple-ADSID": adsid,
        "User-Agent": "com.apple.iCloudHelper/282 CFNetwork/1408.0.4 Darwin/22.5.0",
        "X-Mme-Client-Info": "<MacBookPro18,3> <Mac OS X;13.4.1;22F8> <com.apple.AOSKit/282 (com.apple.accountsd/113)>",
    }
    headers.update(generate_anisette_headers())

    r = requests.post(
        "https://setup.icloud.com/setup/iosbuddy/loginDelegates",
        auth=(username, pet),
        data=data,
        headers=headers,
        verify=False,
    )

    return plist.loads(r.content)


def gsa_authenticate(username, password, second_factor="sms"):
    # Password is None as we'll provide it later
    usr = srp.User(username, bytes(), hash_alg=srp.SHA256, ng_type=srp.NG_2048)
    _, A = usr.start_authentication()

    r = gsa_authenticated_request(
        {"A2k": A, "ps": ["s2k", "s2k_fo"], "u": username, "o": "init"}
    )
    if "sp" not in r:
        print("Authentication Failed. Check your Apple ID and password.")
        raise Exception("AuthenticationError")
    if r["sp"] != "s2k" and r["sp"] != "s2k_fo":
        print(
            f"This implementation only supports s2k and s2k_fo. Server returned {r['sp']}"
        )
        return

    # Change the password out from under the SRP library, as we couldn't calculate it without the salt.
    usr.p = encrypt_password(password, r["s"], r["i"], r["sp"] == "s2k_fo")

    M = usr.process_challenge(r["s"], r["B"])

    # Make sure we processed the challenge correctly
    if M is None:
        print("Failed to process challenge")
        return

    r = gsa_authenticated_request(
        {"c": r["c"], "M1": M, "u": username, "o": "complete"}
    )

    # Make sure that the server's session key matches our session key (and thus that they are not an imposter)
    usr.verify_session(r["M2"])
    if not usr.authenticated():
        print("Failed to verify session")
        return

    spd = decrypt_cbc(usr, r["spd"])
    # For some reason plistlib doesn't accept it without the header...
    PLISTHEADER = b"""\
<?xml version='1.0' encoding='UTF-8'?>
<!DOCTYPE plist PUBLIC '-//Apple//DTD PLIST 1.0//EN' 'http://www.apple.com/DTDs/PropertyList-1.0.dtd'>
"""
    spd = plist.loads(PLISTHEADER + spd)

    if "au" in r["Status"] and r["Status"]["au"] in [
        "trustedDeviceSecondaryAuth",
        "secondaryAuth",
    ]:
        print("2FA required, requesting code")
        # Replace bytes with strings
        for k, v in spd.items():
            if isinstance(v, bytes):
                spd[k] = base64.b64encode(v).decode()
        if second_factor == "sms":
            sms_second_factor(spd["adsid"], spd["GsIdmsToken"])
        elif second_factor == "trusted_device":
            trusted_second_factor(spd["adsid"], spd["GsIdmsToken"])
        return gsa_authenticate(username, password)
    elif "au" in r["Status"]:
        print(f"Unknown auth value {r['Status']['au']}")
        return
    else:
        return spd


def gsa_authenticated_request(parameters):
    body = {
        "Header": {"Version": "1.0.1"},
        "Request": {"cpd": generate_cpd()},
    }
    body["Request"].update(parameters)

    headers = {
        "Content-Type": "text/x-xml-plist",
        "Accept": "*/*",
        "User-Agent": "akd/1.0 CFNetwork/978.0.7 Darwin/18.7.0",
        "X-MMe-Client-Info": "<MacBookPro18,3> <Mac OS X;13.4.1;22F8> <com.apple.AOSKit/282 (com.apple.dt.Xcode/3594.4.19)>",
    }

    resp = requests.post(
        "https://gsa.apple.com/grandslam/GsService2",
        headers=headers,
        data=plist.dumps(body),
        verify=False,
        timeout=5,
    )

    return plist.loads(resp.content)["Response"]


def generate_cpd():
    cpd = {
        # Many of these values are not strictly necessary, but may be tracked by Apple
        "bootstrap": True,  # All implementations set this to true
        "icscrec": True,  # Only AltServer sets this to true
        "pbe": False,  # All implementations explicitly set this to false
        "prkgen": True,  # I've also seen ckgen
        "svct": "iCloud",  # In certian circumstances, this can be 'iTunes' or 'iCloud'
    }

    cpd.update(generate_anisette_headers())
    return cpd


def generate_anisette_headers():
    try:
        import pyprovision
        from ctypes import c_ulonglong
        import secrets

        adi = pyprovision.ADI("./anisette/")
        adi.provisioning_path = "./anisette/"
        device = pyprovision.Device("./anisette/device.json")
        if not device.initialized:
            # Pretend to be a MacBook Pro
            device.server_friendly_description = "<MacBookPro13,2> <macOS;13.1;22C65> <com.apple.AuthKit/1 (com.apple.dt.Xcode/3594.4.19)>"
            device.unique_device_identifier = str(uuid.uuid4()).upper()
            device.adi_identifier = secrets.token_hex(8).lower()
            device.local_user_uuid = secrets.token_hex(32).upper()
        adi.identifier = device.adi_identifier
        dsid = c_ulonglong(-2).value
        is_prov = adi.is_machine_provisioned(dsid)
        if not is_prov:
            print("provisioning...")
            provisioning_session = pyprovision.ProvisioningSession(adi, device)
            provisioning_session.provision(dsid)
        otp = adi.request_otp(dsid)
        a = {
            "X-Apple-I-MD": base64.b64encode(bytes(otp.one_time_password)).decode(),
            "X-Apple-I-MD-M": base64.b64encode(bytes(otp.machine_identifier)).decode(),
        }
    except ImportError:
        print(
            f"pyprovision is not installed, querying {ANISETTE_URL} for an anisette server"
        )
        h = json.loads(requests.get(ANISETTE_URL, timeout=5).text)
        a = {"X-Apple-I-MD": h["X-Apple-I-MD"], "X-Apple-I-MD-M": h["X-Apple-I-MD-M"]}
    a.update(generate_meta_headers(user_id=USER_ID, device_id=DEVICE_ID))
    return a


def generate_meta_headers(serial="0", user_id=uuid.uuid4(), device_id=uuid.uuid4()):
    return {
        "X-Apple-I-Client-Time": datetime.utcnow().replace(microsecond=0).isoformat()
        + "Z",
        "X-Apple-I-TimeZone": str(datetime.utcnow().astimezone().tzinfo),
        "loc": locale.getdefaultlocale()[0] or "en_US",
        "X-Apple-Locale": locale.getdefaultlocale()[0] or "en_US",
        "X-Apple-I-MD-RINFO": "17106176",  # either 17106176 or 50660608
        "X-Apple-I-MD-LU": base64.b64encode(str(user_id).upper().encode()).decode(),
        "X-Mme-Device-Id": str(device_id).upper(),
        "X-Apple-I-SRL-NO": serial,  # Serial number
    }


def encrypt_password(password, salt, iterations, hex=False):
    hash = hashlib.sha256(password.encode("utf-8"))
    p = hash.hexdigest() if hex else hash.digest()
    return pbkdf2.PBKDF2(p, salt, iterations, SHA256).read(32)


def create_session_key(usr, name):
    k = usr.get_session_key()
    if k is None:
        raise Exception("No session key")
    return hmac.new(k, name.encode(), hashlib.sha256).digest()


def decrypt_cbc(usr, data):
    extra_data_key = create_session_key(usr, "extra data key:")
    extra_data_iv = create_session_key(usr, "extra data iv:")
    # Get only the first 16 bytes of the iv
    extra_data_iv = extra_data_iv[:16]

    # Decrypt with AES CBC
    cipher = Cipher(algorithms.AES(extra_data_key), modes.CBC(extra_data_iv))
    decryptor = cipher.decryptor()
    data = decryptor.update(data) + decryptor.finalize()
    # Remove PKCS#7 padding
    padder = padding.PKCS7(128).unpadder()
    return padder.update(data) + padder.finalize()


def trusted_second_factor(dsid, idms_token):
    identity_token = base64.b64encode((dsid + ":" + idms_token).encode()).decode()

    headers = {
        "Content-Type": "text/x-xml-plist",
        "User-Agent": "Xcode",
        "Accept": "text/x-xml-plist",
        "Accept-Language": "en-us",
        "X-Apple-Identity-Token": identity_token,
        "X-Apple-App-Info": "com.apple.gs.xcode.auth",
        "X-Xcode-Version": "11.2 (11B41)",
        "X-Mme-Client-Info": "<MacBookPro18,3> <Mac OS X;13.4.1;22F8> <com.apple.AOSKit/282 (com.apple.dt.Xcode/3594.4.19)>",
    }

    headers.update(generate_anisette_headers())

    # This will trigger the 2FA prompt on trusted devices
    # We don't care about the response, it's just some HTML with a form for entering the code
    # Easier to just use a text prompt
    requests.get(
        "https://gsa.apple.com/auth/verify/trusteddevice",
        headers=headers,
        verify=False,
        timeout=10,
    )

    # Prompt for the 2FA code. It's just a string like '123456', no dashes or spaces
    code = getpass("Enter 2FA code: ")
    headers["security-code"] = code

    # Send the 2FA code to Apple
    resp = requests.get(
        "https://gsa.apple.com/grandslam/GsService2/validate",
        headers=headers,
        verify=False,
        timeout=10,
    )
    if resp.ok:
        print("2FA successful")


def sms_second_factor(dsid, idms_token):
    identity_token = base64.b64encode((dsid + ":" + idms_token).encode()).decode()

    # TODO: Actually do this request to get user prompt data
    # a = requests.get("https://gsa.apple.com/auth", verify=False)
    # This request isn't strictly necessary though,
    # and most accounts should have their id 1 SMS, if not contribute ;)

    headers = {
        "User-Agent": "Xcode",
        "Accept-Language": "en-us",
        "X-Apple-Identity-Token": identity_token,
        "X-Apple-App-Info": "com.apple.gs.xcode.auth",
        "X-Xcode-Version": "11.2 (11B41)",
        "X-Mme-Client-Info": "<MacBookPro18,3> <Mac OS X;13.4.1;22F8> <com.apple.AOSKit/282 (com.apple.dt.Xcode/3594.4.19)>",
    }

    headers.update(generate_anisette_headers())

    # TODO: Actually get the correct id, probably in the above GET
    body = {"phoneNumber": {"id": 1}, "mode": "sms"}

    # This will send the 2FA code to the user's phone over SMS
    # We don't care about the response, it's just some HTML with a form for entering the code
    # Easier to just use a text prompt
    t = requests.put(
        "https://gsa.apple.com/auth/verify/phone/",
        json=body,
        headers=headers,
        verify=False,
        timeout=5,
    )
    # Prompt for the 2FA code. It's just a string like '123456', no dashes or spaces
    code = input("Enter 2FA code: ")

    body["securityCode"] = {"code": code}

    # Send the 2FA code to Apple
    resp = requests.post(
        "https://gsa.apple.com/auth/verify/phone/securitycode",
        json=body,
        headers=headers,
        verify=False,
        timeout=5,
    )
    if resp.ok:
        print("2FA successful")
