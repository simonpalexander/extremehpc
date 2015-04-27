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

SpatialFilter* readSpatialFilterProprtyFile(char* filename)
{
	printf("Reading Spatial Filter property file: %s\n", filename);

	FILE* filePtr = fopen(filename, "rt");
	if (!filePtr)
	{
		printf("ERROR: Problem opening Spatial Filter file: %s\n", filename);
		return NULL;
	}

	float num = 0;
	int size = -1;
	int numIdx = 0;

	fscanf (filePtr, "%d", &size);

	float* numArray = (float*) malloc(size * size * sizeof(float));

	printf("Size: %d\n", size);
	fscanf (filePtr, "%f", &num);
	while (!feof(filePtr))
	{
		printf ("%d: %f ", numIdx, num);
		numArray[numIdx] = num;
		fscanf (filePtr, "%f", &num);
		numIdx++;
	}

	fclose(filePtr);

	SpatialFilter* spatialFilter = createSpatialFilter(size, numArray);
	if (!spatialFilter) {
		printf("ERROR: Failed to read spatial filter: %s\n", filename);
		return NULL;
	}

	printSpatialFilter(spatialFilter);

	printf("Spatial Filter property file read.");

	return spatialFilter;
}

//-----------------------------------------------------------------------------


