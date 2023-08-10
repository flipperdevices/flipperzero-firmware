#!/usr/bin/perl

# genhexbuf.pl
# Copyright (C) 2020 wolfSSL Inc.
#

use strict;
use warnings;

# ---- SCRIPT SETTINGS -------------------------------------------------------

# output C header file to write cert/key buffers to
my $outputFile = "certs_sig_data.h";

# rsa keys and certs to be converted

my @fileList = (
# please add your der file and name of the data for C language
        #   der file name     name of the data
        #[ "./yourder.der",    "your_der_name_in_C" ],
        [ "./ca-cert.der",    "ca_cert_der" ],
        [ "./ca-cert.der.sign",    "ca_cert_der_sign" ],
        );


# ----------------------------------------------------------------------------

my $num = @fileList;

# open our output file, "+>" creates and/or truncates
open OUT_FILE, "+>", $outputFile  or die $!;

print OUT_FILE "/* certs_sig_data.h */\n\n";
print OUT_FILE "#ifndef WOLFSSL_CERTS_SIG_DATA_H\n";
print OUT_FILE "#define WOLFSSL_CERTS_SIG_DATA_H\n\n";


# convert and print 1024-bit cert/keys
for (my $i = 0; $i < $num; $i++) {

    my $fname = $fileList[$i][0];
    my $sname = $fileList[$i][1];

    print OUT_FILE "/* $fname,  */\n";
    print OUT_FILE "static const unsigned char $sname\[] =\n";
    print OUT_FILE "{\n";
    file_to_hex($fname);
    print OUT_FILE "};\n";
    print OUT_FILE "static const int sizeof_$sname = sizeof($sname);\n\n";
}
print OUT_FILE "#endif /* WOLFSSL_CERTS_SIG_DATA_H */\n\n";


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
