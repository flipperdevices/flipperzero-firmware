#!/usr/bin/env python3

import argparse
import xml.etree.ElementTree as XML
import sys

vendors = {
    "01": "SPIMemChipVendorCypress",
    "04": "SPIMemChipVendorFujitsu",
    "1C": "SPIMemChipVendorEon",
    "1F": "SPIMemChipVendorAtmel",
    "20": "SPIMemChipVendorMicron1",
    "37": "SPIMemChipVendorAmic",
    "52": "SPIMemChipVendorNormem",
    "62": "SPIMemChipVendorSanyo",
    "89": "SPIMemChipVendorIntel",
    "8C": "SPIMemChipVendorEsmt",
    "A1": "SPIMemChipVendorFudan1",
    "F8": "SPIMemChipVendorFudan2",
    "AD": "SPIMemChipVendorHyundai",
    "BF": "SPIMemChipVendorSst",
    "C2": "SPIMemChipVendorMicronix",
    "C8": "SPIMemChipVendorGigadevice1",
    "D5": "SPIMemChipVendorIssi",
    "EF": "SPIMemChipVendorWinbond",
    "68": "SPIMemChipVendorBoya",
    "0D": "SPIMemChipVendorAPMemory",
    "9D": "SPIMemChipVendorPMCSierra1",
    "7F": "SPIMemChipVendorPMCSierra2",
    "4A": "SPIMemChipVendorExcelSemi",
    "51": "SPIMemChipVendorGigadevice2",
    "2C": "SPIMemChipVendorMicron2",
    "5E": "SPIMemChipVendorZbitSemi",
    "E0": "SPIMemChipVendorBergMicro",
    "9B": "SPIMemChipVendorATO",
    "54": "SPIMemChipVendorDOUQI",
    "0E": "SPIMemChipVendorFremont",
}


def getArgs():
    parser = argparse.ArgumentParser(
        description="chiplist.xml to C array converter",
    )
    parser.add_argument("file", help="chiplist.xml file")
    return parser.parse_args()


def getXML(file):
    tree = XML.parse(file)
    root = tree.getroot()
    return root


def parseChip(cur, arr):
    chip = {}
    chipAttr = cur.attrib
    if "page" not in chipAttr: # chip without page size not supported
        return
    if "id" not in chipAttr:  # I2C not supported yet
        return
    if len(chipAttr["id"]) < 6:  # ID wihout capacity id not supported yet
        return
    vendor = chipAttr["id"][0] + chipAttr["id"][1]
    chip["modelName"] = cur.tag
    chip["vendorID"] = getVendorNameEnum(vendor)
    chip["typeID"] = chipAttr["id"][2] + chipAttr["id"][3]
    chip["capacityID"] = chipAttr["id"][4] + chipAttr["id"][5]
    chip["size"] = chipAttr["size"]
    if chipAttr["page"] == "SSTW":
        chip["writeMode"] = "SPIMemChipWriteModeAAIWord"
        chip["pageSize"] = "1"
    elif chipAttr["page"] == "SSTB":
        chip["writeMode"] = "SPIMemChipWriteModeAAIByte"
        chip["pageSize"] = "1"
    else:
        chip["writeMode"] = "SPIMemChipWriteModePage"
        chip["pageSize"] = chipAttr["page"]
    arr.append(chip)


def parseXML(xml, interface):
    arr = []
    for vendor in xml.find(interface):
        for cur in vendor:
            parseChip(cur, arr)
    return arr


def getVendorNameEnum(vendorID):
    try:
        return vendors[vendorID]
    except:
        print("Unknown vendor: " + vendorID)
        sys.exit(1)


def generateCArr(arr, filename):
    with open(filename, "w") as out:
        print('#include "spi_mem_chip_i.h"', file=out)
        print("const SPIMemChip SPIMemChips[] = {", file=out)
        for cur in arr:
            print("    {" + cur["vendorID"] + ",", file=out)
            print('    "' + cur["modelName"] + '",', file=out)
            print("    NULL,", file=out)
            print("    " + cur["size"] + ",", file=out)
            print("    " + cur["writeMode"] + ",", file=out)
            print("    0x" + cur["typeID"] + ",", file=out)
            print("    0x" + cur["capacityID"] + ",", file=out)
            if cur == arr[-1]:
                print("    " + cur["pageSize"] + "}};", file=out)
            else:
                print("    " + cur["pageSize"] + "},", file=out)


def main():
    filename = "spi_mem_chip_arr.c"
    args = getArgs()
    xml = getXML(args.file)
    chipArr = parseXML(xml, "SPI")
    generateCArr(chipArr, filename)


if __name__ == "__main__":
    main()
