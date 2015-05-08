/*
 * proprtiesFileUtils.c
 *
 *  Created on: Apr 26, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globalDefines.h"
#include "proprtiesFileUtils.h"

extern int isLog;

SpatialFilter* readSpatialFilterProprtyFile(char* filename)
{
	if (isLog) { printf("Reading Spatial Filter property file: %s\n", filename); }

	FILE* filePtr = fopen(filename, "rt");
	if (!filePtr)
	{
		printf("ERROR: Problem opening Spatial Filter file: %s\n", filename);
		return NULL;
	}

	float num = 0;
	int size = -1;
	float scalar;
	int numIndex = 0;

	fscanf (filePtr, "%d", &size);

	float* numArray = (float*) malloc(size * size * sizeof(float));

	fscanf (filePtr, "%f", &scalar);
	fscanf (filePtr, "%f", &num);
	while (!feof(filePtr))
	{
		numArray[numIndex] = num;
		fscanf (filePtr, "%f", &num);
		numIndex++;
	}

	fclose(filePtr);

	SpatialFilter* spatialFilter = createSpatialFilter(size, scalar, numArray);
	if (!spatialFilter) {
		printf("ERROR: Failed to read spatial filter: %s\n", filename);
		return NULL;
	}

	printSpatialFilter(spatialFilter);

	if (isLog) { printf("Spatial Filter property file read.\n"); }

	return spatialFilter;
}

//-----------------------------------------------------------------------------


