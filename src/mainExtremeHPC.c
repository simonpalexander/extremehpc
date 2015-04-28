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
//Done Define simply command line argument parsing..
//ToDo Add timing utilities.

// Ex: mainExtremeHPC.exe resources/wood.tga resources/spatialFilter.txt
// Ex: n6822_big.tga, wood.tga, 080913.ike.poster
char* inputFilename = "resources/input.tga";
char* outputFilename = "resources/output.tga";
char* spatialFilterFilename = "resources/spatialFilter.txt";

int processComandLineArguments(int argc, char **argv);

int main(int argc, char **argv)
{
	printf("INFOR: Starting Extreme HPC Project Program:\n");

	if (!successful(processComandLineArguments(argc, argv))) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}

	TgaImage* tgaImage = readTGAFile(inputFilename);
	if (!tgaImage) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}
	//printTgaImageDataValues(tgaImage);

	SpatialFilter* spatialFilter = readSpatialFilterProprtyFile(spatialFilterFilename);
	if (!spatialFilter) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}

	ImageStr* imageStr = createImageStrFromTgaImageForSpatialFilter(tgaImage, spatialFilter);
	if (!imageStr) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}
	cleanUpTgaImage(tgaImage);
	//printImageStrDataValues(imageStr);

	ImageStr* processedImageStr = createImageStr(imageStr);
	if (!processedImageStr) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}

	applySpatialFilterToImageStrComponentArray(spatialFilter, imageStr, processedImageStr, red);
	applySpatialFilterToImageStrComponentArray(spatialFilter, imageStr, processedImageStr, green);
	applySpatialFilterToImageStrComponentArray(spatialFilter, imageStr, processedImageStr, blue);
	//printImageStrDataValues(processedImageStr);
	cleanUpImageStr(imageStr);

	TgaImage* processedTgaImage = createTgaImageFromImageStr(processedImageStr);
	if (!successful(saveTGAImage(outputFilename, processedTgaImage))) {
		printf("ERROR: Exiting program.\n");
		return FAIL;
	}

	cleanUpTgaImage(processedTgaImage);
	cleanUpSpatialFilter(spatialFilter);
	cleanUpImageStr(processedImageStr);

	printf("Exiting Program.\n");
	return SUCCESS;
}

//-----------------------------------------------------------------------------

int processComandLineArguments(int argc, char **argv) {
	if (argc == 2) {
		inputFilename = argv[1];
	}
	else if (argc == 3) {
		inputFilename = argv[1];
		spatialFilterFilename = argv[2];
	}
	else if (argc == 4) {
		inputFilename = argv[1];
		spatialFilterFilename = argv[2];
		outputFilename = argv[3];
	}
	else if (argc>1 && argc > 4) {
		printf("ERROR: Problem parsing command line arguments:\n");
		printf("ERROR: 1) input filename : Default: resources/input.tga:\n");
		printf("ERROR: 2) spatial filter filename : Default: resources/spatialFilter.txt:\n");
		printf("ERROR: 3) output filename : Default: resources/output.tga:\n");
		return FAIL;
	}
	else {
		printf("The command line had no other arguments. Using defaults.\n");
	}

	printf("Command Line Arguments:\n");
	printf("Input: %s\n", inputFilename);
	printf("Spatial Filter: %s\n", spatialFilterFilename);
	printf("Output: %s\n", outputFilename);

	return SUCCESS;
}
