#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This experimental program is designed to extract a subset of interesting test
case snippets from the trezor-crypto test directory and output them as a
standard fuzzer dictionary file.

The program is built on quick-and-dirty regex matching that is known to be
incorrect for parsing code files, but is considered "good enough" for this
specific purpose.
Note that there are target-specific configurations and internal filter settings.
"""

import argparse
import binascii
import glob
import re

# re2 is considered for future use
# it requires a system installation and the google-re2 python package
# import re2


# Expected target format for strings in code:
# Most strings are defined in the general form "example"
# There are a few test vectors in crypto/tests/wycheproof/javascript/EcUtil.js
# with 'example' style string definitions, these are ignored for now

TARGET_DIR = "../tests"

# intentionally excluded file types that currently do not provide enough value:
# *.js, *.md, *.sh, *.html and others from the wycheproof subdirectory

targeted_filetypes_multiline_classA = ("*.c", "*.h", "*.py")
# Java files have different multiline strings that are handled differently
targeted_filetypes_multiline_classB = ("*.java",)
targeted_filetypes_multiline = (
    targeted_filetypes_multiline_classA + targeted_filetypes_multiline_classB
)

# files without multiline string content
# Note: consider switching to actual JSON parsing?
# Note: the wycheproof repository has a number of test cases for other
# cryptography such as DSA and RSA which are currently less interesting for the
# fuzzer dictionary and therefore excluded
targeted_filetypes_singleline = (
    "aes*.json",
    "ecdh*.json",
    "ecdsa*.json",
    "x25519*.json",
    "chacha20*.json",
    "kw*.json",
)

verbose = False

# patterns to extract
# singleline:
# "4a1e76f133afb"
# 0xAF8BBDFE8CDD5 and 0x0488b21e
# m/0'/2147483647'/1'/2147483646'/2' in test_check.c via m/[\d'/]+
#
# multiline:
# "fffc" \n "99"
# "dpubZ9169K" \n "bTYbcY"
# "\x65\xf9" \\n  "\xa0\x6a"
# { 0x086d8bd5, 0x1018f82f, \n 0xc55ece} , see rg "0x([a-zA-Z0-9])+"

# patterns to ignore
# lines with print statements
# lines with exceptions
# comments and other metadata in the testvector JSON files
# filenames
# import statements and other package names

# patterns to investigate further
# public keys with the form BEGIN PUBLIC KEY
# TODO "abc" + "def" string concatenation on the same line without newline
# strings in comments

# dictionary text export file format
# general description:
# https://github.com/AFLplusplus/AFLplusplus/blob/stable/dictionaries/README.md
#
# the exported file is primarly designed for use with a recent libFuzzer version
# and is known to be partially incompatible with other fuzzers that impose
# other limitations
#
# known incompatibilities:
# * honggfuzz only reads a limited number of dictionary entries (8192 with version 2.5)
# * afl++ only reads line content with up to 128 byte

# match everything in quotes that doesn't have an internal quote character and
# at least one internal character
regex_string_general_definition = r"\"[^\"]+\""
regex_string_general = re.compile(regex_string_general_definition)
# the capturing group ignores prefix and suffix outside of the quotes
# Note that this is prone to matching the last line of a C-style multiline string,
# which is addressed via extra state handling during the file processing
regex_oneline_string = re.compile(
    r"(" + regex_string_general_definition + r")\s*[\,\)]+"
)
# ignore lines that have a "+" character preceding a string
regex_oneline_string_java_ignore1 = re.compile(r"^\s*\+\s*\"")

regex_hex_character_segment_inner_definition = "[0-9a-fA-F]+"
regex_hex_character_input_complete = re.compile(
    '^"' + regex_hex_character_segment_inner_definition + '"$'
)
regex_hex_character_input_inner = re.compile(
    regex_hex_character_segment_inner_definition
)
# most constants are preceded by a space, but some have a "(" "[" or "{" before them
regex_hex_constant_singleline = re.compile(r"(?<=\(|\[|\{| )0x[a-fA-F0-9]+")

regex_c_style_multiline = re.compile(r"(?:\".+\"\s*\n\s*)+(?:\".+\")", re.MULTILINE)
regex_c_intermediary_content = re.compile(r"\"\s*\n\s*\"", re.MULTILINE)
# TODO how to prevent matching in the middle of a multi-line string concatenation?
# negative lookbehind for "+" is not possible generically and
# (?<!\+ ) and similar patterns are too static

regex_java_style_multiline = re.compile(
    r"(?:\".+\"\s*\n\s*\+\s*)+(?:\".+\")", re.MULTILINE
)
regex_java_intermediary_content = re.compile(r"\"\s*\n\s*\+\s*\"", re.MULTILINE)

regex_text_newline = re.compile(r"\\n")

# primitive regex that catches most filenames in the data set
regex_filename_heuristic = re.compile(r"\.[a-zA-Z]+")

counter_hex_content = 0
counter_wycheproof_hex_reconstruction = 0

# TODO add '"curve"' to capture algorithm names?
allowlist_keywords_json = (
    '"uncompressed"',
    '"wx"',
    '"wy"',
    '"msg"',
    '"sig"',
    '"key"',
    '"iv"',
    '"ct"',
    '"aad"',
    '"tag"',
    '"public"',
    '"private"',
    '"shared"',
    '"padding"',
    '"x"',
    '"d"',
)

# TODO the "keyPem" entry is only a workaround for an encoding issue
ignore_keywords_java = (
    "println(",
    "Exception(",
    '"keyPem"',
)
ignore_keywords_c = ("printf(",)


def ignore_single_line_json(data):
    """return True if the input should be ignored"""
    # ignore everything that is not matched by the allowlist
    for keyword in allowlist_keywords_json:
        if data.find(keyword) > -1:
            return False
    return True


def ignore_single_line_java(data):
    """return True if the input should be ignored"""
    for keyword in ignore_keywords_java:
        if data.find(keyword) > -1:
            return True
    return False


def ignore_single_line_c(data):
    """return True if the input should be ignored"""
    for keyword in ignore_keywords_c:
        if data.find(keyword) > -1:
            return True
    return False


def ignore_general(data):
    """return True if the input should be ignored"""
    if regex_filename_heuristic.search(data):
        return True
    return False


def encode_strings_for_dictionary(data):
    """
    Assumes that inputs are already in string quotes

    Handles dictionary-specific encoding steps
    """
    # libfuzzer does not like "\n" string patterns in dictionary files, replace
    # it with an encoded newline
    data = regex_text_newline.sub("\\\\x0a", data)
    return data


def detect_and_convert_hex(data):
    """
    Convert hex strings

    Directly pass through non-hex content
    """
    global counter_hex_content
    global counter_wycheproof_hex_reconstruction
    match_result1 = regex_hex_character_input_complete.search(data)
    if match_result1:

        match_result2 = regex_hex_character_input_inner.search(match_result1.string)
        isolated_substring = match_result2.group(0)
        if len(isolated_substring) % 2 == 1:
            # Note: the test cases in the wycheproof testvector JSON files have
            # a custom binary hex format to represent keys
            # among other things, this results in hex strings with an uneven
            # number of characters
            # see tests/wycheproof/java/com/google/security/wycheproof/JsonUtil.java
            # specifically the asBigInteger() function for more information
            if isolated_substring[0] >= "0" and isolated_substring[0] <= "7":
                isolated_substring = "0" + isolated_substring
            else:
                isolated_substring = "f" + isolated_substring
            counter_wycheproof_hex_reconstruction += 1

        converted_result = ""
        try:
            # test error-free conversion to binary
            binascii.unhexlify(isolated_substring)
            hex_with_c_style_formatting = ""
            pos = 0
            while pos < len(isolated_substring) - 1:
                hex_with_c_style_formatting += "\\x" + isolated_substring[pos : pos + 2]
                pos += 2

            converted_result = '"%s"' % hex_with_c_style_formatting
        # TODO binascii.Incomplete exception also relevant?
        except binascii.Error:
            # default to the original input
            return data
        counter_hex_content += 1
        return converted_result
    return data


def search_files_recursively(directory, filetype_glob):
    """returns glob search results"""
    target_files = []
    print_verbose("searching in %s" % directory)
    for filetype in filetype_glob:
        print_verbose("searching for %s" % filetype)
        target_files.extend(glob.glob(f"{directory}/**/{filetype}", recursive=True))
    return target_files


def print_verbose(text):
    """print wrapper"""
    if verbose:
        print(text)


def recursive_dictionary_extraction(directory):
    """handle the central extraction logic"""
    # TODO split this function up into subfunctions
    global counter_hex_content
    # handle as a set structure to de-duplicate results automatically
    candidate_lines = set()

    target_files = search_files_recursively(directory, targeted_filetypes_singleline)
    for filepath in target_files:
        per_file_result_counter = 0
        with open(filepath) as _file:
            print_verbose("processing %s" % filepath)
            for _, line in enumerate(_file.readlines()):
                if ignore_single_line_json(line):
                    continue
                results = regex_oneline_string.findall(line)
                for result in results:
                    candidate_lines.add(result)
                    per_file_result_counter += 1
            if per_file_result_counter > 0:
                print_verbose("results: %d" % per_file_result_counter)

    print_verbose("number of candidate entries: %d" % len(candidate_lines))

    target_files = search_files_recursively(directory, targeted_filetypes_multiline)
    for filepath in target_files:
        per_file_result_counter = 0
        with open(filepath) as _file:
            last_line_was_multiline_string = False
            print_verbose("processing %s for single-line strings" % filepath)
            for _, line in enumerate(_file.readlines()):
                if ignore_single_line_java(line):
                    last_line_was_multiline_string = False
                    continue
                if ignore_single_line_c(line):
                    last_line_was_multiline_string = False
                    continue
                if regex_oneline_string_java_ignore1.search(line):
                    last_line_was_multiline_string = True
                    if regex_oneline_string.search(line):
                        # the Java multiline string apparently ends on this line
                        last_line_was_multiline_string = False
                    continue

                result_general_string = regex_string_general.search(line)
                if result_general_string:
                    # at least one general string is matched, see if it is
                    # a single-line string
                    results = regex_oneline_string.findall(line)
                    for result in results:
                        if not last_line_was_multiline_string:
                            candidate_lines.add(result)
                            per_file_result_counter += 1
                        last_line_was_multiline_string = False
                    if len(results) == 0:
                        last_line_was_multiline_string = True
                else:
                    last_line_was_multiline_string = False

                # TODO split this into a separate loop?
                results = regex_hex_constant_singleline.findall(line)
                for result in results:
                    # remove the "0x" prefix, add quotes
                    candidate_lines.add('"%s"' % result[2:])
                    per_file_result_counter += 1

            if per_file_result_counter > 0:
                print_verbose("results: %d" % per_file_result_counter)

    target_files = search_files_recursively(
        directory, targeted_filetypes_multiline_classA
    )

    for filepath in target_files:
        with open(filepath) as _file:
            print_verbose("processing %s for C-style multi-line strings" % filepath)
            filecontent = _file.read()
            multiline_results = regex_c_style_multiline.findall(filecontent)
            if len(multiline_results) > 0:
                print_verbose("results: %d" % len(multiline_results))
            for result in multiline_results:
                cleanup = regex_c_intermediary_content.sub("", result)
                candidate_lines.add(cleanup)

    target_files = search_files_recursively(
        directory, targeted_filetypes_multiline_classB
    )

    for filepath in target_files:
        with open(filepath) as _file:
            print_verbose("processing %s for Java-style multi-line strings" % filepath)
            filecontent = _file.read()
            multiline_results = regex_java_style_multiline.findall(filecontent)
            if len(multiline_results) > 0:
                print_verbose("results: %d" % len(multiline_results))
            for result in multiline_results:
                cleanup = regex_java_intermediary_content.sub("", result)
                candidate_lines.add(cleanup)

    return candidate_lines


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dictionary_output_file", help="output file", type=str)
    parser.add_argument("--verbose", action="store_true", help="verbose stdout output")

    args = parser.parse_args()
    verbose = args.verbose

    collected_candidate_lines = recursive_dictionary_extraction(TARGET_DIR)
    sorted_candidate_lines = sorted(collected_candidate_lines)
    result_lines = []
    for candidate_line in sorted_candidate_lines:
        if ignore_general(candidate_line):
            continue
        result_lines.append(
            encode_strings_for_dictionary(detect_and_convert_hex(candidate_line))
        )

    print_verbose("counter_hex_content: %d" % counter_hex_content)
    print_verbose(
        "counter_wycheproof_hex_reconstruction: %d"
        % counter_wycheproof_hex_reconstruction
    )
    print_verbose("overall deduplicated entries: %d" % len(sorted_candidate_lines))

    with open(args.dictionary_output_file, "w") as _file:
        for result_line in result_lines:
            _file.write("%s\n" % result_line)
