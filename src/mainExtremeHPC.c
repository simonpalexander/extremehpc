/*
 * mainExtremeHPC.c
 *
 *  Created on: Apr 25, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>

#include "globalDefines.h"
#include "tgaUtils.h"

#include "proprtiesFileUtils.h"
#include "spatialFilterUtils.h"
#include "imageProcessingUtils.h"

//Done Pack the read in image data into an integer for each pixel. One byte for each colour component, with the fourth byte (essentially alpha, being ignored.
//Done Unpack BGR chars to RGBA into an int. - use bit shifting.
//Done Pack RGBA int into an BGA chars
//Done Integrate data read and unpacking in same function.
//Done Integrate data packing and write in same function.
//Done Define the image processing spatial filter.
//Done Define image processing spatial filter in a configuration or properties file. File needs to end with a blank new line.
//Done Define the assumption on how image boundaries are processed. Will follow a 'extend' approach.
//Done Consider defining an image processing buffer - a subset of the image data - to apply spatial filter over. Have decided not to implement this now unless needed.
//Done Apply spatial filter to image.
//ToDo Add timing utilities.

int main(int argc, char **argv)
{
	// n6822_big.tga, wood.tga
	char* inputFilename = "resources/n6822_big.tga";
	TgaImage* tgaImage = readTGAFile(inputFilename);
	if (!tgaImage) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}
	//printTgaImageDataValues(tgaImage);

	char* spatialFilterFilename = "resources/spatialFilter.txt";
	SpatialFilter* spatialFilter = readSpatialFilterProprtyFile(spatialFilterFilename);
	if (!spatialFilter) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}

	ImageStr* imageStr = createImageStrFromTgaImageForSpatialFilter(tgaImage, spatialFilter);
	//printImageStrDataValues(imageStr);

	ImageStr* processedImageStr = createImageStr(imageStr);

	applySpatialFilterToImageStrComponentArray(spatialFilter, imageStr, processedImageStr, red);
	applySpatialFilterToImageStrComponentArray(spatialFilter, imageStr, processedImageStr, green);
	applySpatialFilterToImageStrComponentArray(spatialFilter, imageStr, processedImageStr, blue);
	//printImageStrDataValues(processedImageStr);

	TgaImage* processedTgaImage = createTgaImageFromImageStr(processedImageStr, tgaImage);
	char* outputFilename = "resources/output.tga";
	if (!successful(saveTGAImage(outputFilename, processedTgaImage))) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}

	cleanUpTgaImage(tgaImage);
	cleanUpTgaImage(processedTgaImage);
	cleanUpSpatialFilter(spatialFilter);

	printf("Exiting Program.\n");
	return SUCCESS;
}

