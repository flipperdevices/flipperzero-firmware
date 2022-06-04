#!/usr/bin/perl

# Detect comment blocks that are likely meant to be doxygen blocks but aren't.
#
# More precisely, look for normal comment block containing '\'.
# Of course one could use doxygen warnings, eg with:
#   sed -e '/EXTRACT/s/YES/NO/' doxygen/mbedtls.doxyfile | doxygen -
# but that would warn about any undocumented item, while our goal is to find
# items that are documented, but not marked as such by mistake.

use warnings;
use strict;
use File::Basename;

# C/header files in the following directories will be checked
my @directories = qw(include/mbedtls library doxygen/input);

# very naive pattern to find directives:
# everything with a backslach except '\0' and backslash at EOL
my $doxy_re = qr/\\(?!0|\n)/;

sub check_file {
    my ($fname) = @_;
    open my $fh, '<', $fname or die "Failed to open '$fname': $!\n";

    # first line of the last normal comment block,
    # or 0 if not in a normal comment block
    my $block_start = 0;
    while (my $line = <$fh>) {
        $block_start = $.   if $line =~ m/\/\*(?![*!])/;
        $block_start = 0    if $line =~ m/\*\//;
        if ($block_start and $line =~ m/$doxy_re/) {
            print "$fname:$block_start: directive on line $.\n";
            $block_start = 0; # report only one directive per block
        }
    }

    close $fh;
}

sub check_dir {
    my ($dirname) = @_;
    for my $file (<$dirname/*.[ch]>) {
        check_file($file);
    }
}

# locate root directory based on invocation name
my $root = dirname($0) . '/..';
chdir $root or die "Can't chdir to '$root': $!\n";

# just do it
for my $dir (@directories) {
    check_dir($dir)
}

__END__
