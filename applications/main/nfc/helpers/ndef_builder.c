#include "ndef_builder.h"

#include <string.h>

// NFC Forum URI RTD prefix table. Index = identifier code; entry = prefix string.
// Order matters: when matching a URI we walk the table top-to-bottom and prefer the
// first match. Putting "https://www." before "https://" yields shorter encodings.
static const struct {
    uint8_t code;
    const char* prefix;
} ndef_uri_prefixes[] = {
    {0x01, "http://www."},
    {0x02, "https://www."},
    {0x03, "http://"},
    {0x04, "https://"},
    {0x05, "tel:"},
    {0x06, "mailto:"},
    {0x07, "ftp://anonymous:anonymous@"},
    {0x08, "ftp://ftp."},
    {0x09, "ftps://"},
    {0x0A, "sftp://"},
    {0x0B, "smb://"},
    {0x0C, "nfs://"},
    {0x0D, "ftp://"},
    {0x0E, "dav://"},
    {0x0F, "news:"},
    {0x10, "telnet://"},
    {0x11, "imap:"},
    {0x12, "rtsp://"},
    {0x13, "urn:"},
    {0x14, "pop:"},
    {0x15, "sip:"},
    {0x16, "sips:"},
    {0x17, "tftp:"},
    {0x18, "btspp://"},
    {0x19, "btl2cap://"},
    {0x1A, "btgoep://"},
    {0x1B, "tcpobex://"},
    {0x1C, "irdaobex://"},
    {0x1D, "file://"},
};

// NDEF record header flag bits
#define NDEF_MB 0x80 // Message Begin
#define NDEF_ME 0x40 // Message End
#define NDEF_SR 0x10 // Short Record (1-byte payload length)
#define NDEF_TNF_WELL_KNOWN 0x01
#define NDEF_TNF_MIME_MEDIA 0x02

// TLV markers
#define NDEF_TLV_MESSAGE 0x03
#define NDEF_TLV_TERMINATOR 0xFE

static uint8_t ndef_uri_match_prefix(const char** uri_io) {
    const char* uri = *uri_io;
    for(size_t i = 0; i < sizeof(ndef_uri_prefixes) / sizeof(ndef_uri_prefixes[0]); i++) {
        size_t plen = strlen(ndef_uri_prefixes[i].prefix);
        if(strncmp(uri, ndef_uri_prefixes[i].prefix, plen) == 0) {
            *uri_io = uri + plen;
            return ndef_uri_prefixes[i].code;
        }
    }
    return 0x00; // No prefix; URI is written verbatim
}

// Wrap a built NDEF message body (header + record bytes) with the TLV envelope
// (0x03, length, body, 0xFE). For length < 0xFF we use the short form (1 byte);
// otherwise the long form (0xFF + 2 bytes big-endian).
//
// `body` already lives in `out` at offset `body_start`. We shift it forward as
// needed and write the TLV header/terminator around it. Returns total size.
static bool ndef_tlv_wrap(
    uint8_t* out,
    size_t out_capacity,
    size_t body_start,
    size_t body_len,
    size_t* out_size) {
    size_t header_len = (body_len < 0xFF) ? 2 : 4;
    size_t total = header_len + body_len + 1; // +1 terminator
    if(total > out_capacity) return false;

    // If body isn't already at header_len, shift it.
    if(body_start != header_len) {
        memmove(out + header_len, out + body_start, body_len);
    }

    out[0] = NDEF_TLV_MESSAGE;
    if(body_len < 0xFF) {
        out[1] = (uint8_t)body_len;
    } else {
        out[1] = 0xFF;
        out[2] = (uint8_t)(body_len >> 8);
        out[3] = (uint8_t)(body_len & 0xFF);
    }
    out[header_len + body_len] = NDEF_TLV_TERMINATOR;
    *out_size = total;
    return true;
}

bool ndef_builder_build_uri(
    const char* uri,
    uint8_t* out,
    size_t out_capacity,
    size_t* out_size) {
    if(!uri || !out || !out_size) return false;

    const char* uri_body = uri;
    uint8_t prefix_code = ndef_uri_match_prefix(&uri_body);
    size_t uri_body_len = strlen(uri_body);
    size_t payload_len = 1 + uri_body_len; // 1 byte prefix code + URI string

    // Record layout: [hdr][type_len][payload_len][type='U'][prefix_code][uri_body]
    // = 4 header bytes + 1 type + payload_len
    size_t record_len = 4 + 1 + payload_len;

    // Reserve space at the start for the TLV header (worst case 4 bytes).
    // We always write the body starting at offset 4 and let tlv_wrap shift it
    // down to offset 2 for the short-form case.
    const size_t body_start = 4;
    if(body_start + record_len + 1 > out_capacity) return false;

    uint8_t* p = out + body_start;
    *p++ = NDEF_MB | NDEF_ME | NDEF_SR | NDEF_TNF_WELL_KNOWN;
    *p++ = 1; // Type length
    *p++ = (uint8_t)payload_len;
    *p++ = 'U';
    *p++ = prefix_code;
    memcpy(p, uri_body, uri_body_len);

    return ndef_tlv_wrap(out, out_capacity, body_start, record_len, out_size);
}

bool ndef_builder_build_text(
    const char* text,
    uint8_t* out,
    size_t out_capacity,
    size_t* out_size) {
    if(!text || !out || !out_size) return false;

    static const char lang[] = "en";
    const size_t lang_len = sizeof(lang) - 1;
    const size_t text_len = strlen(text);
    // Payload: [status_byte][lang_code][text]
    // status_byte: bit7=0 (UTF-8), low 6 bits = lang code length
    const size_t payload_len = 1 + lang_len + text_len;
    if(payload_len > 0xFF) return false; // We only support short records

    const size_t record_len = 4 + 1 + payload_len; // header + 'T' + payload
    const size_t body_start = 4;
    if(body_start + record_len + 1 > out_capacity) return false;

    uint8_t* p = out + body_start;
    *p++ = NDEF_MB | NDEF_ME | NDEF_SR | NDEF_TNF_WELL_KNOWN;
    *p++ = 1; // Type length
    *p++ = (uint8_t)payload_len;
    *p++ = 'T';
    *p++ = (uint8_t)lang_len; // status byte (UTF-8, lang length)
    memcpy(p, lang, lang_len);
    p += lang_len;
    memcpy(p, text, text_len);

    return ndef_tlv_wrap(out, out_capacity, body_start, record_len, out_size);
}

// WPS attribute IDs (big-endian on the wire)
#define WPS_ATTR_CREDENTIAL       0x100E
#define WPS_ATTR_NETWORK_INDEX    0x1026
#define WPS_ATTR_SSID             0x1045
#define WPS_ATTR_AUTH_TYPE        0x1003
#define WPS_ATTR_ENCRYPT_TYPE     0x100F
#define WPS_ATTR_NETWORK_KEY      0x1027
#define WPS_ATTR_MAC_ADDRESS      0x1020

#define WPS_AUTH_WPA2_PSK         0x0020
#define WPS_ENCRYPT_AES           0x0008

static uint8_t* wps_put_u8(uint8_t* p, uint16_t attr, uint8_t value) {
    *p++ = (uint8_t)(attr >> 8);
    *p++ = (uint8_t)(attr & 0xFF);
    *p++ = 0x00;
    *p++ = 0x01;
    *p++ = value;
    return p;
}

static uint8_t* wps_put_u16(uint8_t* p, uint16_t attr, uint16_t value) {
    *p++ = (uint8_t)(attr >> 8);
    *p++ = (uint8_t)(attr & 0xFF);
    *p++ = 0x00;
    *p++ = 0x02;
    *p++ = (uint8_t)(value >> 8);
    *p++ = (uint8_t)(value & 0xFF);
    return p;
}

static uint8_t* wps_put_bytes(uint8_t* p, uint16_t attr, const uint8_t* data, uint16_t len) {
    *p++ = (uint8_t)(attr >> 8);
    *p++ = (uint8_t)(attr & 0xFF);
    *p++ = (uint8_t)(len >> 8);
    *p++ = (uint8_t)(len & 0xFF);
    memcpy(p, data, len);
    return p + len;
}

bool ndef_builder_build_wifi_wpa2(
    const char* ssid,
    const char* password,
    uint8_t* out,
    size_t out_capacity,
    size_t* out_size) {
    if(!ssid || !password || !out || !out_size) return false;

    size_t ssid_len = strlen(ssid);
    size_t pass_len = strlen(password);
    if(ssid_len == 0 || ssid_len > 32) return false;
    if(pass_len == 0 || pass_len > 63) return false;

    // Inner credential payload size:
    // Network Index TLV (5) + SSID TLV (4 + ssid_len) +
    // Auth Type TLV (6) + Encrypt Type TLV (6) + Network Key TLV (4 + pass_len) +
    // MAC Address TLV (4 + 6) = 35 + ssid_len + pass_len
    size_t cred_len = 5 + (4 + ssid_len) + 6 + 6 + (4 + pass_len) + (4 + 6);

    // Outer Credential TLV: 4 byte header + cred_len
    size_t payload_len = 4 + cred_len;
    if(payload_len > 0xFF) return false;

    static const char mime[] = "application/vnd.wfa.wsc";
    const size_t mime_len = sizeof(mime) - 1;

    // Record layout: [hdr][type_len][payload_len][mime...][payload]
    size_t record_len = 1 + 1 + 1 + mime_len + payload_len;

    const size_t body_start = 4;
    if(body_start + record_len + 1 > out_capacity) return false;

    uint8_t* p = out + body_start;
    *p++ = NDEF_MB | NDEF_ME | NDEF_SR | NDEF_TNF_MIME_MEDIA;
    *p++ = (uint8_t)mime_len;
    *p++ = (uint8_t)payload_len;
    memcpy(p, mime, mime_len);
    p += mime_len;

    // Outer Credential TLV header
    *p++ = (uint8_t)(WPS_ATTR_CREDENTIAL >> 8);
    *p++ = (uint8_t)(WPS_ATTR_CREDENTIAL & 0xFF);
    *p++ = (uint8_t)(cred_len >> 8);
    *p++ = (uint8_t)(cred_len & 0xFF);

    p = wps_put_u8(p, WPS_ATTR_NETWORK_INDEX, 0x01);
    p = wps_put_bytes(p, WPS_ATTR_SSID, (const uint8_t*)ssid, (uint16_t)ssid_len);
    p = wps_put_u16(p, WPS_ATTR_AUTH_TYPE, WPS_AUTH_WPA2_PSK);
    p = wps_put_u16(p, WPS_ATTR_ENCRYPT_TYPE, WPS_ENCRYPT_AES);
    p = wps_put_bytes(p, WPS_ATTR_NETWORK_KEY, (const uint8_t*)password, (uint16_t)pass_len);
    static const uint8_t bcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    p = wps_put_bytes(p, WPS_ATTR_MAC_ADDRESS, bcast_mac, 6);

    return ndef_tlv_wrap(out, out_capacity, body_start, record_len, out_size);
}
