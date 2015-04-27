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

//Done Pack the read in image data into an integer for each pixel. One byte for each colour component, with the fourth byte (essentially alpha, being ignored.
//Done Unpack BGR chars to RGBA into an int. - use bit shifting.
//Done Pack RGBA int into an BGA chars
//Done Integrate data read and unpacking in same function.
//Done Integrate data packing and write in same function.
//Done Define the image processing spatial filter.
//Done Define image processing spatial filter in a configuration or properties file. File needs to end with a blank new line.
//Done Define the assumption on how image boundaries are processed. Will follow an 'extend' approach
//ToDo Consider defining an image processing buffer - a subset of the image data - to apply spatial filter over.
//ToDo Apply spatial filter to image.
//ToDo Add timing utilities.

SpatialFilter* setupSpatialFilter(int size);

int main(int argc, char **argv)
{
	// n6822_big.tga, wood.tga
	char* inputFilename = "resources/n6822_big.tga";
	TgaImage* tgaImage = readTGAFile(inputFilename);
	if (!tgaImage) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}

	char* spatialFilterFilename = "resources/spatialFilter.txt";
	SpatialFilter* spatialFilter = readSpatialFilterProprtyFile(spatialFilterFilename);
	if (!spatialFilter) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}

	char* outputFilename = "resources/output.tga";
	if (!successful(saveTGAImage(outputFilename, tgaImage))) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}

	cleanUpTgaImage(tgaImage);
	cleanUpSpatialFilter(spatialFilter);

	printf("Exiting Program.\n");
	return SUCCESS;
}

//-----------------------------------------------------------------------------

SpatialFilter* setupSpatialFilter(int size)
{
	//SpatialFilter* spatialFilter = createSpatialFilter(size);
/*
	if (!successful(setFilterElement(spatialFilter, 0, 0, -1))) {
		printf("ERROR: Failed to set filter element (0,0).\n");
		return NULL;
	}

	if (!successful(setFilterElement(spatialFilter, 0, 1, -1))) {
		printf("ERROR: Failed to set filter element (0,1).\n");
		return NULL;
	}

	if (!successful(setFilterElement(spatialFilter, 0, 2, -1))) {
		printf("ERROR: Failed to set filter element (0,2).\n");
		return NULL;
	}

	if (!successful(setFilterElement(spatialFilter, 1, 0, -1))) {
		printf("ERROR: Failed to set filter element (1,0).\n");
		return NULL;
	}

	if (!successful(setFilterElement(spatialFilter, 1, 1, 8))) {
		printf("ERROR: Failed to set filter element (1,1).\n");
		return NULL;
	}

	if (!successful(setFilterElement(spatialFilter, 1, 2, -1))) {
		printf("ERROR: Failed to set filter element (1,2).\n");
		return NULL;
	}

	if (!successful(setFilterElement(spatialFilter, 2, 0, -1))) {
		printf("ERROR: Failed to set filter element (2,0).\n");
		return NULL;
	}

	if (!successful(setFilterElement(spatialFilter, 2, 1, -1))) {
		printf("ERROR: Failed to set filter element (2,1).\n");
		return NULL;
	}

	if (!successful(setFilterElement(spatialFilter, 2, 2, -1))) {
		printf("ERROR: Failed to set filter element (2,2).\n");
		return NULL;
	}

	return spatialFilter;*/
	return NULL;
}
