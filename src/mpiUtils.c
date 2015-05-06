/*
 * mpiUtilities.c
 *
 *  Created on: May 5, 2015
 *      Author: simon
 */

#include <stdlib.h>

#include "globalDefines.h"

#include "mpiUtils.h"

#include "mpi.h"

const int mpiTag = 2;

MPI_Datatype mpiSpatialFilterType;

void defineMpiDataTypes()
{
	// Setup MPI Spatial Filter Type.
	const int numOfItems=3;
	int blocklengths[3] = {1,1,1};
	MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_FLOAT};
	MPI_Aint offsets[3];

	offsets[0] = offsetof(SpatialFilter, size);
	offsets[1] = offsetof(SpatialFilter, requiredImageEdgeExtend);
	offsets[2] = offsetof(SpatialFilter, scalar);

	MPI_Type_create_struct(numOfItems, blocklengths, offsets, types, &mpiSpatialFilterType);
	MPI_Type_commit(&mpiSpatialFilterType);
}

//-----------------------------------------------------------------------------

void freeMpiDataTypes()
{
	MPI_Type_free(&mpiSpatialFilterType);
}

//-----------------------------------------------------------------------------

int mpiSendIsLog(int isLog, int srcNode)
{
	MPI_Bcast(&isLog, 1, MPI_INT, srcNode, MPI_COMM_WORLD);

	return SUCCESS;
}

//-----------------------------------------------------------------------------

int mpiReceiveIsLog(int srcNode, int mpiRank)
{
	int isLog;
	MPI_Bcast(&isLog, 1, MPI_INT, srcNode, MPI_COMM_WORLD);

	if (isLog)
	{
		printf("INFO:(%d): isLog: %d\n", mpiRank, isLog);
	}

	return isLog;
}

//-----------------------------------------------------------------------------

int mpiSendSpatialFilter(SpatialFilter* spatialFilter, int srcNode)
{
	MPI_Bcast(spatialFilter, 1, mpiSpatialFilterType, srcNode, MPI_COMM_WORLD);

	int numOfFilterElements = spatialFilter->size * spatialFilter->size;

	MPI_Bcast(spatialFilter->filter, numOfFilterElements, MPI_FLOAT, srcNode, MPI_COMM_WORLD);

	return SUCCESS;
}

//-----------------------------------------------------------------------------

SpatialFilter* mpiReceiveSpatialFilter(int srcNode, int mpiRank, int isLog)
{
	MPI_Status status;

	SpatialFilter* spatialFilter = (SpatialFilter*) malloc(sizeof(SpatialFilter));

	MPI_Bcast(spatialFilter, 1, mpiSpatialFilterType, srcNode, MPI_COMM_WORLD);

	int numOfFilterElements = spatialFilter->size * spatialFilter->size;

	spatialFilter->filter = (float*) malloc(numOfFilterElements * sizeof(float));

	MPI_Bcast(spatialFilter->filter, numOfFilterElements, MPI_FLOAT, srcNode, MPI_COMM_WORLD);

	if (isLog)
	{
		mpiPrintSpatialFilter(spatialFilter, mpiRank);
	}

	return spatialFilter;
}

//-----------------------------------------------------------------------------

void mpiPrintSpatialFilter(SpatialFilter* spatialFilter, int mpiRank)
{
	printf("INFO:(%d): --- Spatial Filter: ---\n", mpiRank);
	printf("INFO:(%d): Size: %d by %d\n", mpiRank, spatialFilter->size, spatialFilter->size);
	printf("INFO:(%d): Scalar: %f\n", mpiRank, spatialFilter->scalar);
	printf("INFO:(%d): Required Edge Extend: %d\n", mpiRank, spatialFilter->requiredImageEdgeExtend);

	printf("INFO:(%d): Filter:\n", mpiRank);

	int row, col;
	for (row = 0; row<spatialFilter->size ; row++)
	{
		printf("INFO:(%d): ", mpiRank);
		for (col = 0; col<spatialFilter->size ; col++)
		{
			printf("%g\t", spatialFilter->filter[row * spatialFilter->size + col]);
		}
		printf("\n");
	}
}

