import cv2 as cv
import numpy as np

def get_ref_from_string(phrase, font):
	result = font[ord(phrase[0])]
	for i in phrase[1:]:
		result = np.concatenate((result, font[ord(i)]), axis=1)
	return result

font_helvB08 = list()
for i in range(256):
	font_helvB08.append(cv.imread("./font_helvB08/helvB08_" + str(i) + ".png"))

font_HaxrCorp_4089 = list()
for i in range(256):
	font_HaxrCorp_4089.append(cv.imread("./font_HaxrCorp_4089/HaxrCorp_4089_" + str(i) + ".png"))

font_profont11 = list()
for i in range(256):
	font_profont11.append(cv.imread("./font_profont11/profont11_" + str(i) + ".png"))

font_profont22 = list()
for i in range(256):
	font_profont22.append(cv.imread("./font_profont22/profont22_" + str(i) + ".png"))

phrase1 = "Hello, world! This phrase is typed by font 'helvB08', like flipper's menu"
phrase2 = "And this phrase is typed by font 'HaxrCorp_4089', like flipper's file browser"
phrase3 = "Finaly, this is font 'profont11', just for example"
phrase4 = "And if you think that whole that fonts are too smal, we also have 'profont22'!"

cv.imshow('out1', get_ref_from_string(phrase1, font_helvB08))
cv.imshow('out2', get_ref_from_string(phrase2, font_HaxrCorp_4089))
cv.imshow('out3', get_ref_from_string(phrase3, font_profont11))
cv.imshow('out4', get_ref_from_string(phrase4, font_profont22))
cv.waitKey()