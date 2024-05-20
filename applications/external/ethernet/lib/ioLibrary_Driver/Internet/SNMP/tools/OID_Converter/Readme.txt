============================================================
<OID encoding steps for WIZnet SNMP Agent>
============================================================

+ Example OID for convert
	1.3.6.1.4.1.19865.1.0

(1) Convert Usage
 	CMD>>oid 1.3.6.1.4.1.19865.1.0
 	=> 06 0A 2B 06 01 04 01 81 9B 19 01 00

	>> TLV(tag-length-value) Example OID
	(06) Tag
	(0A) Length
	[2B] 1(iso).3(identified-organization) (in ASN.1 BER encoding, i.e. 1*40+3 = 0x2b)
	[06] 6(dod)
	[01] 1(internet)
	[04] 4(private)
	[01] 1(enterprise)
	[81 9B 19] 19865(Vendor-Specific)
	...

(2) Add the entry to OID table in source code (DataEntryType, put the converted value to array)
	{0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0x9b, 0x19, 0x01, 0x00},
	SNMPDTYPE_OCTET_STRING, 30, {"long-length OID Test"},
	NULL, NULL},

============================================================
OID Converter: OID encoder/decoder
v1.3 - Matthias Gaertner 1999/2001 - Freeware
============================================================

The OID converter is a handy little tool to convert ASN.1 OIDs from readable dotted decimal notation to binary hexadecimal Distinguished Encoding Rules (DER) representation and vice versa. 
If you're into x.509 certificates, this may be useful to you, too.

Usage:
 OID [-c|-C] [-o] {-i|1.2.3.4}
   converts dotted form to ASCII HEX DER output.
 OID -x [-o] {-i|hex-digits}
   decodes ASCII HEX DER and gives dotted form.
   
If you need more information, please refer to Matthias Gaertner's page,
http://www.rtner.de/software/oid.html

===============================================================================================