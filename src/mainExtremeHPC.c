/*
 * mainExtremeHPC.c
 *
 *  Created on: Apr 25, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>

#include "tgaUtils.h"

/*
 * ToDo:
 * 1) Pack the read in image data into an integer for each pixel. One byte for
 * each colour component, with the fourth byte (essentially alpha, being ignored.
 * 1.1) Unpack BGR chars to RGBA into an int. - use bit shifting.
 * 1.2) Pack RGBA int into an BGA chars
 * 2) Define the image processing map.
 * 3) Define the assumption on how image boundaries are processed.
 * 4) Apply map to image.
 */

int main(int argc, char **argv)
{
	// n6822_big.tga, wood.tga
	char* inputFilename = "resources/wood.tga";
	TgaImage* tgaImage = readTGAFile(inputFilename);

	unpackImageData(tgaImage);
	packImageData(tgaImage);

	char* outputFilename = "resources/output.tga";
	saveTGAImage(outputFilename, tgaImage);

	free(tgaImage->image);
	free(tgaImage);

	return 0;
}
