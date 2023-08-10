#!/usr/bin/perl

# dertoc.pl
# version 1.0
# Updated 07/31/2018
#
# Copyright (C) 2006-2018 wolfSSL Inc.
#

use strict;
use warnings;

my $num_args = $#ARGV + 1;
if ($num_args != 3 ) {
    print "usage: ./scripts/dertoc.pl ./certs/server-cert.der server_cert_der_2048 dertoc.c\n";
    exit;
}

my $inFile = $ARGV[0];
my $outName = $ARGV[1];
my $outputFile = $ARGV[2];

# open our output file, "+>" creates and/or truncates
open OUT_FILE, "+>", $outputFile  or die $!;

print OUT_FILE "/* $outputFile */\n\n";

print OUT_FILE "static const unsigned char $outName\[] =\n";
print OUT_FILE "{\n";
file_to_hex($inFile);
print OUT_FILE "};\n";
print OUT_FILE "static const int sizeof_$outName = sizeof($outName);\n\n";

# close file
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
            print OUT_FILE "\t";
        }
        read($fp, $byte, 1) or die "Error reading $fileName";
        my $output = sprintf("0x%02X", ord($byte));
        print OUT_FILE $output;

        if ($i != ($fileLen - 1)) {
            print OUT_FILE ", ";
        }

        if ($j == 10) {
            $j = 0;
            print OUT_FILE "\n";
        }
    }

    print OUT_FILE "\n";

    close($fp);
}
