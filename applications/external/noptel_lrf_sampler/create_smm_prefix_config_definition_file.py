#!/usr/bin/python3
"""Noptel LRF rangefinder sampler for the Flipper Zero
Version: 1.9

Companion utility to create a special definition file containing the
optional prefix sequence to send before SMM commands, and the associated
configuration menu label and choices to enable / disable it.
The file must be copied into the apps_data/noptel_lrf_sampler directory to
enable the undocumented function in the configuration menu.

This utility requires Noptel internal code.
"""

## Parameters
#

app_c_header_file = "common.h"


## Modules
#

import re
import sys
import argparse

from lrfclass.full.lrf import LRF


## Main routine
#


def main():
    # Load the app's C header file
    with open(app_c_header_file, "r") as f:
        h = f.read()

    # Get the name of the SMM prefix configuration definition file
    default_file = re.search('SMM_PREFIX_CONFIG_DEFINITION_FILE "([^\s]*?)"', h)[1]

    # Get the size of the SMM prefix configuration option label and choices
    sizeof_label = int(re.search("config_smm_pfx_label\[([0-9]+)\];", h)[1])
    sizeof_choices = int(re.search("config_smm_pfx_names.*?\[([0-9]+)\];", h)[1])

    # Parse the command line arguments
    argparser = argparse.ArgumentParser()

    argparser.add_argument(
        "-f",
        "--file",
        help="SMM prefix configuration definition file to create. "
        "Default: {}".format(default_file),
        type=str,
        default=default_file,
    )

    args = argparser.parse_args()

    # Get the SMM prefix sequence and description strings from the LRF class
    sd = LRF()._smm_prefix_and_use_descs_for_flipper_zero_noptel_lrf_sampler()

    prefix_bytes = sd["sequence"]
    cfg_label = sd["controls"]
    cfg_disable_choice = sd["notsent"]
    cfg_enable_choice = sd["sent"]

    print()

    # Warn the user if the strings returned by the LRF class are too long
    # This should NEVER happen!
    if len(cfg_label) >= sizeof_label:
        cfg_label = cfg_label[: sizeof_label - 1]
        print(
            'WARNING: SMM prefix configuration option label truncated to "{}"!'.format(
                cfg_label
            )
        )

    if len(cfg_disable_choice) >= sizeof_choices:
        cfg_disable_choice = cfg_disable_choice[: sizeof_choices - 1]
        print(
            "WARNING: SMM prefix disabled configuration option choice "
            'truncated to "{}"!'.format(cfg_disable_choice)
        )

    if len(cfg_enable_choice) >= sizeof_choices:
        cfg_enable_choice = cfg_enable_choice[: sizeof_choices - 1]
        print(
            "WARNING: SMM prefix enabled configuration option choice "
            'truncated to "{}"!'.format(cfg_enable_choice)
        )

    # Write out the SMM prefix configuration definition file
    with open(args.file, "wb") as f:
        # SMM prefix sequence
        f.write(prefix_bytes)

        # SMM prefix configuration option label
        f.write((cfg_label.encode("ascii") + (b"\x00" * sizeof_label))[:sizeof_label])

        # SMM prefix disabled configuration option choice
        f.write(
            (cfg_disable_choice.encode("ascii") + (b"\x00" * sizeof_choices))[
                :sizeof_choices
            ]
        )

        # SMM prefix enabled configuration option choice
        f.write(
            (cfg_enable_choice.encode("ascii") + (b"\x00" * sizeof_choices))[
                :sizeof_choices
            ]
        )

    print("File {} written".format(args.file))
    print(
        "Copy {} in /apps_data/noptel_lrf_sampler with qFlipper and restart "
        "the app to enable {} control settings".format(args.file, cfg_label.lower())
    )

    print()

    return 0


## Main program
#

if __name__ == "__main__":
    sys.exit(main())
