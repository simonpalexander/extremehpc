/*
 * spatialFilterUtils.c
 *
 *  Created on: Apr 25, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>

#include "globalDefines.h"
#include "spatialFilterUtils.h"

SpatialFilter* createSpatialFilter(int size, float* numArray)
{
	SpatialFilter* spatialFilter = (SpatialFilter*) malloc(sizeof(SpatialFilter));

	spatialFilter->size = size;
	spatialFilter->filter = (float*) malloc(size * size * sizeof(float));

	int numberOfElements = size * size;

	int idx;
	for (idx=0 ; idx<numberOfElements ; idx++)
	{
		spatialFilter->filter[idx] = numArray[idx];
	}

	return spatialFilter;
}

//-----------------------------------------------------------------------------

void cleanUpSpatialFilter(SpatialFilter* spatialFilter)
{
	free(spatialFilter->filter);
	free(spatialFilter);
}

//-----------------------------------------------------------------------------

int setFilterElement(SpatialFilter* spatialFilter, int row, int col, float value)
{
	if (spatialFilter)
	{
		if (row>=spatialFilter->size)
		{
			printf("ERROR: row index %d is out of range of spatial filter size %d.\n", row, spatialFilter->size);
			return FAIL;
		}
		if (col>=spatialFilter->size)
		{
			printf("ERROR: col index %d is out of range of spatial filter size %d.\n", col, spatialFilter->size);
			return FAIL;
		}
		spatialFilter->filter[row * spatialFilter->size + col] = value;
	}
	else
	{
		printf("ERROR: SpatialFilter pointer is null.\n");
		return FAIL;
	}

	return SUCCESS;
}

//-----------------------------------------------------------------------------

void printSpatialFilter(SpatialFilter* spatialFilter)
{
	printf("\n--- Spatial Filter: ---\n");
	printf("Size: [%d by %d]\n", spatialFilter->size, spatialFilter->size);
	int row, col;
	for (row = 0; row<spatialFilter->size ; row++)
	{
		for (col = 0; col<spatialFilter->size ; col++)
		{
			printf("%g\t", spatialFilter->filter[row * spatialFilter->size + col]);
		}
		printf("\n");
	}
	printf("-----------------------\n");
}
