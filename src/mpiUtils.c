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

ImageStr* subDivideAndSendImageStr(ImageStr* imageStr, int numOfProcessors, int srcNode, int mpiRank, int isLog)
{
	// Currently have the assumption that the number of rows in the image
	// is equal or greater than the number of processors.
	int sectionRowThickness = ((imageStr->height - (2 * imageStr->rowExtend)) / numOfProcessors) + 1;
	int finalSectionRowThickness = (imageStr->height - (2 * imageStr->rowExtend)) - (sectionRowThickness * (numOfProcessors - 1) );
	int sectionRowThicknessWithExtend = sectionRowThickness + (2 * imageStr->rowExtend);
	int finalSectionRowThicknessWithExtend = finalSectionRowThickness + (2 * imageStr->rowExtend);

	//if (isLog)
	{
		printf("INFO:(%d): SubDivide: Num of Processors: %d\n", mpiRank, numOfProcessors);
		printf("INFO:(%d): SubDivide: Image Height: %d : Width %d\n", mpiRank, imageStr->height, imageStr->width);
		printf("INFO:(%d): SubDivide: Rows from 0 to %d\n", mpiRank, imageStr->height - 1);
		printf("INFO:(%d): SubDivide: Image Row Extend: %d\n", mpiRank, imageStr->rowExtend);
		printf("INFO:(%d): SubDivide: Number of Elements: %d\n", mpiRank, (imageStr->width * imageStr->height));
		printf("INFO:(%d): SubDivide: Section Row Thickness: %d : With Extend %d\n", mpiRank, sectionRowThickness, sectionRowThicknessWithExtend);
		printf("INFO:(%d): SubDivide: Final Section Row Thickness: %d : With Extend %d\n", mpiRank, finalSectionRowThickness, finalSectionRowThicknessWithExtend);
	}

	int startRowIndex, fromRowIndex, toRowIndex, startElement, toElement;
	int processorIndex;
	for (processorIndex = 0 ; processorIndex < (numOfProcessors - 1) ; processorIndex++)
	{
		startRowIndex = processorIndex * sectionRowThickness + imageStr->rowExtend;
		printf("INFO:(%d): SubDivide: Processor: %d : startRowIndex: %d\n", mpiRank, processorIndex, startRowIndex);
		fromRowIndex = startRowIndex - imageStr->rowExtend;
		toRowIndex = startRowIndex + sectionRowThickness + imageStr->rowExtend;
		//toRowIndex = fromRowIndex + sectionRowThicknessWithExtend - 1;
		printf("INFO:(%d): SubDivide: Including Extend: From %d to %d\n", mpiRank, fromRowIndex, toRowIndex);

		startElement = fromRowIndex * imageStr->width;
		toElement = (toRowIndex) * imageStr->width - 1;
		printf("INFO:(%d): SubDivide: From Element: %d to %d\n", mpiRank, startElement, toElement);
	}

	startRowIndex = processorIndex * sectionRowThickness + imageStr->rowExtend;
	printf("INFO:(%d): SubDivide: Processors: %d : startRowIndex: %d\n", mpiRank, processorIndex, startRowIndex);
	fromRowIndex = startRowIndex - imageStr->rowExtend;
	toRowIndex = startRowIndex + finalSectionRowThickness + imageStr->rowExtend;
	//toRowIndex = fromRowIndex + finalSectionRowThicknessWithExtend - 1;
	printf("INFO:(%d): SubDivide: Including Extend: From %d to %d\n", mpiRank, fromRowIndex, toRowIndex);

	startElement = fromRowIndex * imageStr->width;
	toElement = (toRowIndex) * imageStr->width - 1;
	printf("INFO:(%d): SubDivide: From Element: %d to %d\n", mpiRank, startElement, toElement);

	/*
	for (rowIndex=imageStr->rowExtend ; rowIndex<rowEndIndex ; rowIndex++)
	{
		for (columnIndex=imageStr->columnExtend ; columnIndex<columnEndIndex ; columnIndex++)
		{
			filterElementPtr = spatialFilter->filter;
			componentSum = 0.0f;

			for (filterRowIndex=-filterRange ; filterRowIndex<=filterRange ; filterRowIndex++)
			{
				for (filterColumnIndex=-filterRange ; filterColumnIndex<=filterRange ; filterColumnIndex++)
				{
					dataArrayIndex = (rowIndex + filterRowIndex) * imageStr->width + (columnIndex + filterColumnIndex);
					// Spatial Filter element: (filterColumnIndex, filterRowIndex)
					elementValue = *(arrayPtr + dataArrayIndex);
					componentSum += (*filterElementPtr) * elementValue;
					filterElementPtr++;
				}
			}

			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			*(processedArrayPtr + dataArrayIndex) = spatialFilter->scalar * componentSum;
		}
	}
	*/

	ImageStr* subImageStr = (ImageStr*) malloc(sizeof(ImageStr));

	return subImageStr;
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

