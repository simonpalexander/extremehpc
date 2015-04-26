/*
 * mainExtremeHPC.c
 *
 *  Created on: Apr 25, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>

#include "tgaUtils.h"

//Done Pack the read in image data into an integer for each pixel. One byte for each colour component, with the fourth byte (essentially alpha, being ignored.
//Done Unpack BGR chars to RGBA into an int. - use bit shifting.
//Done Pack RGBA int into an BGA chars
//ToDo Integrate data read and unpacking in same function.
//ToDo Integrate date pack and write in same function.
//ToDo Define the image processing map.
//ToDo Define the assumption on how image boundaries are processed.
//ToDo Apply map to image.


int main(int argc, char **argv)
{
	// n6822_big.tga, wood.tga
	char* inputFilename = "resources/n6822_big.tga";
	TgaImage* tgaImage = readTGAFile(inputFilename);

	//unpackImageData(tgaImage);
	//packImageData(tgaImage);

	char* outputFilename = "resources/output.tga";
	saveTGAImage(outputFilename, tgaImage);

	free(tgaImage->imageData);
	free(tgaImage);

	return 0;
}
