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

SpatialFilter* createSpatialFilter(int size)
{
	SpatialFilter* spatialFilter = (SpatialFilter*) malloc(sizeof(SpatialFilter));

	spatialFilter->size = size;
	spatialFilter->filter = (int*) malloc(size * size * sizeof(int));

	return spatialFilter;
}

//-----------------------------------------------------------------------------

void cleanUpSpatialFilter(SpatialFilter* spatialFilter)
{
	free(spatialFilter->filter);
	free(spatialFilter);
}

//-----------------------------------------------------------------------------

int setFilterElement(SpatialFilter* spatialFilter, int row, int col, int value)
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

	int row, col;
	for (row = 0; row<spatialFilter->size ; row++)
	{
		for (col = 0; col<spatialFilter->size ; col++)
		{
			printf("%d ", spatialFilter->filter[row * spatialFilter->size + col]);
		}
		printf("\n");
	}
}
