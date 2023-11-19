import os
import numpy as np
from PIL import Image
from pathlib import Path
import math


def fixImg(img: np.ndarray) -> np.ndarray:
    """
    This is a bit of a hacky fix. Basically it can only draw bitmaps with widths divisible by 8
    The function will add empty pixels to the width in order to get to a valid width
    """
    remainingWidth = (math.ceil(len(img[0]) / 8) * 8) - len(img[0])
    height = len(img)

    padding = np.zeros((height, remainingWidth, 4))

    fixedImg = np.hstack((img, padding))

    return fixedImg


def openImage(imgPath: str) -> tuple[np.ndarray, str]:
    """
    Open the image at the given path and return it as a numpy array

    Referenced from: https://stackoverflow.com/questions/25102461/python-rgb-matrix-of-an-image

    :param: imgPath: Path to the image to open
    :return: Numpy array of rgb values, name of file
    """
    img = Image.open(imgPath)
    imgArr = np.array(img)

    imgArr = fixImg(imgArr)

    # Get file name without extension
    name = Path(imgPath).stem

    return imgArr, name


def imgToBoolArr(img: np.ndarray, thresh: int) -> tuple[int, int, list[int]]:
    """
    Take the np array image, convert to grayscale, apply a threshold, then convert to boolean

    :param: img: NP Array containing the image data
    :param: thresh: Threshold pixel value (>= will be set on the flipper, < will be turned off)
    :return: Height, width, boolean array
    """
    height = len(img)
    width = len(img[0])

    boolImg = []

    for line in img:
        # line is now a single row of pixels in the image
        for pixel in line:
            # pixel is now each pixel with r, g, b, a
            # Now we calculate grayscale
            grayscalePixel = (int(pixel[0]) + int(pixel[1]) + int(pixel[2])) / 3

            pixOn = 1 if grayscalePixel >= thresh else 0

            boolImg.append(pixOn)

    # Now the list of booleans for each pixel is complete
    return height, width, boolImg


def chunkArr(boolArr: list[int], chunks: int):
    """
    Chunk an array of given size and make sublists of that size
    Referenced from: https://www.geeksforgeeks.org/break-list-chunks-size-n-python/
    """
    for i in range(0, len(boolArr), chunks):
        yield boolArr[i:i + chunks]


def boolArrToXBM(img: list[int]) -> list[int]:
    """
    Function to convert the boolean array into a bitmap (list of bytes)

    :param: img: Boolean array
    :return: List of bytes (8 pixels per)
    """
    xbm = []

    # Chunk list by byte, 8 bits (pixels) in a byte
    boolChunked = list(chunkArr(img, 8))

    for byte in boolChunked:
        byteVal = 0
        for i in range(len(byte)):
            bit = byte[i]
            byteVal |= (bit << i)
        
        # Now we have the assembled byte
        xbm.append(byteVal)
    
    return xbm


def saveXBM(path: str, imgName: str, height: int, width: int, xbm: list[int]) -> None:
    """
    Function to write the parameters to a file in C format that can be interpreted
    by the flipper code

    :param: path: Path to file to append to
    :param: imgName: Name to save the image as in the file
    :param: height: Height of the image
    :param: width: Width of the image
    :param: xbm: Bitmap of the image
    """
    with open(path, "a+") as saveFile:
        # Write height
        saveFile.write("#define {}_height {}\n".format(imgName, height))
        # Write width
        saveFile.write("#define {}_width {}\n".format(imgName, width))
        
        # Write beginning of bitmap
        saveFile.write("static uint8_t " + imgName + r"_xbm[] = {" + "\n")

        # Loop through each xbm and write it to the list
        for byte in xbm:
            saveFile.write("\t{},\n".format(hex(byte)))
        
        # Close the bitmap array
        saveFile.write("};\n")


def extractXBMCode(imgPath: str, xbmPath: str, thresh: int) -> None:
    """
    Extract the XBM from the image by path and save it to the given file

    :param: imgPath: Path to the image
    :param: xbmPath: Path to save the xbm c code
    :param: thresh: Theshold of what pixel value to consider on (>= is on)
    """
    imgArr, name = openImage(imgPath)
    height, width, boolArr = imgToBoolArr(imgArr, thresh)
    xbm = boolArrToXBM(boolArr)
    saveXBM(xbmPath, name, height, width, xbm)


def main():
    pass


if __name__ == "__main__":
    main()