#!/usr/bin/perl

# gencertbuf.pl
# version 1.1
# Updated 07/01/2014
#
# Copyright (C) 2006-2015 wolfSSL Inc.
#

use strict;
use warnings;

# ---- SCRIPT SETTINGS -------------------------------------------------------

# output C header file to write cert/key buffers to
my $outputFile = "./wolfssl/certs_test.h";

# ecc keys and certs to be converted
# Used with HAVE_ECC && USE_CERT_BUFFERS_256

my @fileList_ecc = (
        [ "./certs/ecc-client-key.der",              "ecc_clikey_der_256" ],
        [ "./certs/ecc-client-keyPub.der",           "ecc_clikeypub_der_256" ],
        [ "./certs/client-ecc-cert.der",             "cliecc_cert_der_256" ],
        [ "./certs/ecc-key.der",                     "ecc_key_der_256" ],
        [ "./certs/ecc-keyPub.der",                  "ecc_key_pub_der_256" ],
        [ "./certs/statickeys/ecc-secp256r1.der",    "ecc_secp_r1_statickey_der_256" ],
        [ "./certs/server-ecc-comp.der",             "serv_ecc_comp_der_256" ],
        [ "./certs/server-ecc-rsa.der",              "serv_ecc_rsa_der_256" ],
        [ "./certs/server-ecc.der",                  "serv_ecc_der_256" ],
        [ "./certs/ca-ecc-key.der",                  "ca_ecc_key_der_256" ],
        [ "./certs/ca-ecc-cert.der",                 "ca_ecc_cert_der_256" ],
        [ "./certs/ca-ecc384-key.der",               "ca_ecc_key_der_384" ],
        [ "./certs/ca-ecc384-cert.der",              "ca_ecc_cert_der_384" ]
        );


# ed25519 keys and certs
# Used with HAVE_ED25519 define.
my @fileList_ed = (
        [ "./certs/ed25519/server-ed25519.der",     "server_ed25519_cert" ],
        [ "./certs/ed25519/server-ed25519-key.der", "server_ed25519_key" ],
        [ "./certs/ed25519/ca-ed25519.der",         "ca_ed25519_cert" ],
        [ "./certs/ed25519/client-ed25519.der",     "client_ed25519_cert" ],
        [ "./certs/ed25519/client-ed25519-key.der", "client_ed25519_key" ]
        );

# x25519 keys and certs
# Used with USE_CERT_BUFFERS_25519 define.
my @fileList_x = (
        [ "./certs/statickeys/x25519.der",      "x25519_statickey_der" ],
        [ "./certs/statickeys/x25519-pub.der",  "x25519_pub_statickey_der" ]
        );


# 1024-bit certs/keys to be converted
# Used with USE_CERT_BUFFERS_1024 define.

my @fileList_1024 = (
        [ "./certs/1024/client-key.der", "client_key_der_1024" ],
        [ "./certs/1024/client-keyPub.der", "client_keypub_der_1024" ],
        [ "./certs/1024/client-cert.der", "client_cert_der_1024" ],
        [ "./certs/1024/dh1024.der", "dh_key_der_1024" ],
        [ "./certs/1024/dsa1024.der", "dsa_key_der_1024" ],
        [ "./certs/1024/rsa1024.der", "rsa_key_der_1024" ],
        [ "./certs/1024/ca-key.der", "ca_key_der_1024"],
        [ "./certs/1024/ca-cert.der", "ca_cert_der_1024" ],
        [ "./certs/1024/server-key.der", "server_key_der_1024" ],
        [ "./certs/1024/server-cert.der", "server_cert_der_1024" ]
        );

# 2048-bit certs/keys to be converted
# Used with USE_CERT_BUFFERS_2048 define.
my @fileList_2048 = (
        [ "./certs/client-key.der", "client_key_der_2048" ],
        [ "./certs/client-keyPub.der", "client_keypub_der_2048" ],
        [ "./certs/client-cert.der", "client_cert_der_2048" ],
        [ "./certs/dh2048.der", "dh_key_der_2048" ],
        [ "./certs/dh-pubkey-2048.der", "dh_pub_key_der_2048" ],
        [ "./certs/statickeys/dh-ffdhe2048.der", "dh_ffdhe_statickey_der_2048" ],
        [ "./certs/statickeys/dh-ffdhe2048-pub.der", "dh_ffdhe_pub_statickey_der_2048" ],
        [ "./certs/dsa-pubkey-2048.der", "dsa_pub_key_der_2048" ],
        [ "./certs/dsa2048.der", "dsa_key_der_2048" ],
        [ "./certs/rsa2048.der", "rsa_key_der_2048" ],
        [ "./certs/ca-key.der", "ca_key_der_2048" ],
        [ "./certs/ca-cert.der", "ca_cert_der_2048" ],
        [ "./certs/ca-cert-chain.der", "ca_cert_chain_der" ],
        [ "./certs/server-key.der", "server_key_der_2048" ],
        [ "./certs/server-cert.der", "server_cert_der_2048" ]
        );

# 3072-bit certs/keys to be converted
# Used with USE_CERT_BUFFERS_3072 define.
my @fileList_3072 = (
        [ "./certs/dh3072.der", "dh_key_der_3072" ],
        [ "./certs/dsa3072.der", "dsa_key_der_3072" ],
        [ "./certs/rsa3072.der", "rsa_key_der_3072" ],
        [ "./certs/3072/client-key.der", "client_key_der_3072" ],
        [ "./certs/3072/client-keyPub.der", "client_keypub_der_3072" ],
        [ "./certs/3072/client-cert.der", "client_cert_der_3072" ],
        );

# 4096-bit certs/keys to be converted
# Used with USE_CERT_BUFFERS_4096 define.
my @fileList_4096 = (
        [ "./certs/4096/client-key.der", "client_key_der_4096" ],
        [ "./certs/4096/client-keyPub.der", "client_keypub_der_4096" ],
        [ "./certs/4096/client-cert.der", "client_cert_der_4096" ],
        [ "./certs/dh4096.der", "dh_key_der_4096" ],
        );

#Falcon Post-Quantum Keys
#Used with HAVE_PQC
my @fileList_falcon = (
        ["certs/falcon/bench_falcon_level1_key.der", "bench_falcon_level1_key" ],
        ["certs/falcon/bench_falcon_level5_key.der", "bench_falcon_level5_key" ],
        );

#Dilithium Post-Quantum Keys
#Used with HAVE_PQC
my @fileList_dilithium = (
        ["certs/dilithium/bench_dilithium_level2_key.der", "bench_dilithium_level2_key" ],
        ["certs/dilithium/bench_dilithium_level3_key.der", "bench_dilithium_level3_key" ],
        ["certs/dilithium/bench_dilithium_level5_key.der", "bench_dilithium_level5_key" ],
        );

#Sphincs+ Post-Quantum Keys
#Used with HAVE_PQC
my @fileList_sphincs = (
        ["certs/sphincs/bench_sphincs_fast_level1_key.der", "bench_sphincs_fast_level1_key" ],
        ["certs/sphincs/bench_sphincs_fast_level3_key.der", "bench_sphincs_fast_level3_key" ],
        ["certs/sphincs/bench_sphincs_fast_level5_key.der", "bench_sphincs_fast_level5_key" ],
        ["certs/sphincs/bench_sphincs_small_level1_key.der", "bench_sphincs_small_level1_key" ],
        ["certs/sphincs/bench_sphincs_small_level3_key.der", "bench_sphincs_small_level3_key" ],
        ["certs/sphincs/bench_sphincs_small_level5_key.der", "bench_sphincs_small_level5_key" ],
        );


# ----------------------------------------------------------------------------

my $num_ecc = @fileList_ecc;
my $num_ed = @fileList_ed;
my $num_x = @fileList_x;
my $num_1024 = @fileList_1024;
my $num_2048 = @fileList_2048;
my $num_3072 = @fileList_3072;
my $num_4096 = @fileList_4096;
my $num_falcon = @fileList_falcon;
my $num_dilithium = @fileList_dilithium;
my $num_sphincs = @fileList_sphincs;

# open our output file, "+>" creates and/or truncates
open OUT_FILE, "+>", $outputFile  or die $!;

print OUT_FILE "/* certs_test.h */\n";
print OUT_FILE "/* This file was generated using: ./gencertbuf.pl */\n\n";
print OUT_FILE "#ifndef WOLFSSL_CERTS_TEST_H\n";
print OUT_FILE "#define WOLFSSL_CERTS_TEST_H\n\n";

# convert and print 1024-bit cert/keys
print OUT_FILE "#ifdef USE_CERT_BUFFERS_1024\n\n";
for (my $i = 0; $i < $num_1024; $i++) {

    my $fname = $fileList_1024[$i][0];
    my $sname = $fileList_1024[$i][1];

    print OUT_FILE "/* $fname, 1024-bit */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}
print OUT_FILE "#endif /* USE_CERT_BUFFERS_1024 */\n\n";


# convert and print 2048-bit certs/keys
print OUT_FILE "#ifdef USE_CERT_BUFFERS_2048\n\n";
for (my $i = 0; $i < $num_2048; $i++) {

    my $fname = $fileList_2048[$i][0];
    my $sname = $fileList_2048[$i][1];

    print OUT_FILE "/* $fname, 2048-bit */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}


print OUT_FILE "#endif /* USE_CERT_BUFFERS_2048 */\n\n";


# convert and print 3072-bit certs/keys
print OUT_FILE "#ifdef USE_CERT_BUFFERS_3072\n\n";
for (my $i = 0; $i < $num_3072; $i++) {

    my $fname = $fileList_3072[$i][0];
    my $sname = $fileList_3072[$i][1];

    print OUT_FILE "/* $fname, 3072-bit */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}

print OUT_FILE "#endif /* USE_CERT_BUFFERS_3072 */\n\n";


# convert and print 4096-bit certs/keys
print OUT_FILE "#ifdef USE_CERT_BUFFERS_4096\n\n";
for (my $i = 0; $i < $num_4096; $i++) {

    my $fname = $fileList_4096[$i][0];
    my $sname = $fileList_4096[$i][1];

    print OUT_FILE "/* $fname, 4096-bit */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}

print OUT_FILE "#endif /* USE_CERT_BUFFERS_4096 */\n\n";

# convert and print falcon keys
print OUT_FILE "#if defined(HAVE_PQC) && defined(HAVE_FALCON)\n\n";
for (my $i = 0; $i < $num_falcon; $i++) {

    my $fname = $fileList_falcon[$i][0];
    my $sname = $fileList_falcon[$i][1];

    print OUT_FILE "/* $fname */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}

print OUT_FILE "#endif /* HAVE_PQC && HAVE_FALCON */\n\n";

# convert and print dilithium keys
print OUT_FILE "#if defined (HAVE_PQC) && defined(HAVE_DILITHIUM)\n\n";
for (my $i = 0; $i < $num_dilithium; $i++) {

    my $fname = $fileList_dilithium[$i][0];
    my $sname = $fileList_dilithium[$i][1];

    print OUT_FILE "/* $fname */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}

print OUT_FILE "#endif /* HAVE_PQC && HAVE_DILITHIUM */\n\n";

# convert and print sphincs keys
print OUT_FILE "#if defined(HAVE_PQC) && defined(HAVE_SPHINCS)\n\n";
for (my $i = 0; $i < $num_sphincs; $i++) {

    my $fname = $fileList_sphincs[$i][0];
    my $sname = $fileList_sphincs[$i][1];

    print OUT_FILE "/* $fname */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}

print OUT_FILE "#endif /* HAVE_PQC && HAVE_SPHINCS */\n\n";

# convert and print 256-bit cert/keys
print OUT_FILE "#if defined(HAVE_ECC) && defined(USE_CERT_BUFFERS_256)\n\n";
for (my $i = 0; $i < $num_ecc; $i++) {

    my $fname = $fileList_ecc[$i][0];
    my $sname = $fileList_ecc[$i][1];

    print OUT_FILE "/* $fname, ECC */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}
print OUT_FILE "#endif /* HAVE_ECC && USE_CERT_BUFFERS_256 */\n\n";


print OUT_FILE "/* dh1024 p */
static const unsigned char dh_p[] =
{
    0xE6, 0x96, 0x9D, 0x3D, 0x49, 0x5B, 0xE3, 0x2C, 0x7C, 0xF1, 0x80, 0xC3,
    0xBD, 0xD4, 0x79, 0x8E, 0x91, 0xB7, 0x81, 0x82, 0x51, 0xBB, 0x05, 0x5E,
    0x2A, 0x20, 0x64, 0x90, 0x4A, 0x79, 0xA7, 0x70, 0xFA, 0x15, 0xA2, 0x59,
    0xCB, 0xD5, 0x23, 0xA6, 0xA6, 0xEF, 0x09, 0xC4, 0x30, 0x48, 0xD5, 0xA2,
    0x2F, 0x97, 0x1F, 0x3C, 0x20, 0x12, 0x9B, 0x48, 0x00, 0x0E, 0x6E, 0xDD,
    0x06, 0x1C, 0xBC, 0x05, 0x3E, 0x37, 0x1D, 0x79, 0x4E, 0x53, 0x27, 0xDF,
    0x61, 0x1E, 0xBB, 0xBE, 0x1B, 0xAC, 0x9B, 0x5C, 0x60, 0x44, 0xCF, 0x02,
    0x3D, 0x76, 0xE0, 0x5E, 0xEA, 0x9B, 0xAD, 0x99, 0x1B, 0x13, 0xA6, 0x3C,
    0x97, 0x4E, 0x9E, 0xF1, 0x83, 0x9E, 0xB5, 0xDB, 0x12, 0x51, 0x36, 0xF7,
    0x26, 0x2E, 0x56, 0xA8, 0x87, 0x15, 0x38, 0xDF, 0xD8, 0x23, 0xC6, 0x50,
    0x50, 0x85, 0xE2, 0x1F, 0x0D, 0xD5, 0xC8, 0x6B,
};

/* dh1024 g */
static const unsigned char dh_g[] =
{
  0x02,
};\n\n";

print OUT_FILE "#if defined(NO_ASN) && defined(WOLFSSL_SP_MATH)\n";
print OUT_FILE "/* dh2048 p */
static const unsigned char dh2048_p[] =
{
    0xb0, 0xa1, 0x08, 0x06, 0x9c, 0x08, 0x13, 0xba, 0x59, 0x06, 0x3c, 0xbc,
    0x30, 0xd5, 0xf5, 0x00, 0xc1, 0x4f, 0x44, 0xa7, 0xd6, 0xef, 0x4a, 0xc6,
    0x25, 0x27, 0x1c, 0xe8, 0xd2, 0x96, 0x53, 0x0a, 0x5c, 0x91, 0xdd, 0xa2,
    0xc2, 0x94, 0x84, 0xbf, 0x7d, 0xb2, 0x44, 0x9f, 0x9b, 0xd2, 0xc1, 0x8a,
    0xc5, 0xbe, 0x72, 0x5c, 0xa7, 0xe7, 0x91, 0xe6, 0xd4, 0x9f, 0x73, 0x07,
    0x85, 0x5b, 0x66, 0x48, 0xc7, 0x70, 0xfa, 0xb4, 0xee, 0x02, 0xc9, 0x3d,
    0x9a, 0x4a, 0xda, 0x3d, 0xc1, 0x46, 0x3e, 0x19, 0x69, 0xd1, 0x17, 0x46,
    0x07, 0xa3, 0x4d, 0x9f, 0x2b, 0x96, 0x17, 0x39, 0x6d, 0x30, 0x8d, 0x2a,
    0xf3, 0x94, 0xd3, 0x75, 0xcf, 0xa0, 0x75, 0xe6, 0xf2, 0x92, 0x1f, 0x1a,
    0x70, 0x05, 0xaa, 0x04, 0x83, 0x57, 0x30, 0xfb, 0xda, 0x76, 0x93, 0x38,
    0x50, 0xe8, 0x27, 0xfd, 0x63, 0xee, 0x3c, 0xe5, 0xb7, 0xc8, 0x09, 0xae,
    0x6f, 0x50, 0x35, 0x8e, 0x84, 0xce, 0x4a, 0x00, 0xe9, 0x12, 0x7e, 0x5a,
    0x31, 0xd7, 0x33, 0xfc, 0x21, 0x13, 0x76, 0xcc, 0x16, 0x30, 0xdb, 0x0c,
    0xfc, 0xc5, 0x62, 0xa7, 0x35, 0xb8, 0xef, 0xb7, 0xb0, 0xac, 0xc0, 0x36,
    0xf6, 0xd9, 0xc9, 0x46, 0x48, 0xf9, 0x40, 0x90, 0x00, 0x2b, 0x1b, 0xaa,
    0x6c, 0xe3, 0x1a, 0xc3, 0x0b, 0x03, 0x9e, 0x1b, 0xc2, 0x46, 0xe4, 0x48,
    0x4e, 0x22, 0x73, 0x6f, 0xc3, 0x5f, 0xd4, 0x9a, 0xd6, 0x30, 0x07, 0x48,
    0xd6, 0x8c, 0x90, 0xab, 0xd4, 0xf6, 0xf1, 0xe3, 0x48, 0xd3, 0x58, 0x4b,
    0xa6, 0xb9, 0xcd, 0x29, 0xbf, 0x68, 0x1f, 0x08, 0x4b, 0x63, 0x86, 0x2f,
    0x5c, 0x6b, 0xd6, 0xb6, 0x06, 0x65, 0xf7, 0xa6, 0xdc, 0x00, 0x67, 0x6b,
    0xbb, 0xc3, 0xa9, 0x41, 0x83, 0xfb, 0xc7, 0xfa, 0xc8, 0xe2, 0x1e, 0x7e,
    0xaf, 0x00, 0x3f, 0x93
};

/* dh2048 g */
static const unsigned char dh2048_g[] =
{
  0x02,
};\n";
print OUT_FILE "#endif\n\n";


# convert and print ed25519 cert/keys
print OUT_FILE "#if defined(HAVE_ED25519)\n\n";
for (my $i = 0; $i < $num_ed; $i++) {

    my $fname = $fileList_ed[$i][0];
    my $sname = $fileList_ed[$i][1];

    print OUT_FILE "/* $fname, ED25519 */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}
print OUT_FILE "#endif /* HAVE_ED25519 */\n\n";


# convert and print CURVE25519 cert/keys
print OUT_FILE "#if defined(USE_CERT_BUFFERS_25519)\n\n";
for (my $i = 0; $i < $num_x; $i++) {

    my $fname = $fileList_x[$i][0];
    my $sname = $fileList_x[$i][1];

    print OUT_FILE "/* $fname, CURVE25519 */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}
print OUT_FILE "#endif /* USE_CERT_BUFFERS_25519 */\n\n";


print OUT_FILE "#endif /* WOLFSSL_CERTS_TEST_H */\n\n";

# close certs_test.h file
close OUT_FILE or die $!;

# print file as hex, comma-separated, as needed by C buffer
sub file_to_hex {
    my $fileName = $_[0];

    open my $fp, "<", $fileName or die $!;
    binmode($fp);

    my $fileLen = -s $fileName;
    my $byte;

    for (my $i = 0, my $j = 1; $i < $fileLen; $i++, $j++)
    {
        if ($j == 1) {
            print OUT_FILE "        ";
        }
        if ($j != 1) {
            print OUT_FILE " ";
        }
        read($fp, $byte, 1) or die "Error reading $fileName";
        my $output = sprintf("0x%02X", ord($byte));
        print OUT_FILE $output;

        if ($i != ($fileLen - 1)) {
            print OUT_FILE ",";
        }

        if ($j == 10) {
            $j = 0;
            print OUT_FILE "\n";
        }
    }

    print OUT_FILE "\n";

    close($fp);
}
