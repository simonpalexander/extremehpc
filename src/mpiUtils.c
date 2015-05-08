/*
 * mpiUtilities.c
 *
 *  Created on: May 5, 2015
 *      Author: simon
 */

#include <stdlib.h>
#include <math.h>

#include "globalDefines.h"

#include "mpiUtils.h"

#include "mpi.h"

const int mpiTagRed = 0;
const int mpiTagGreen = 1;
const int mpiTagBlue = 2;
const int mpiTagBase = 3;

MPI_Datatype mpiSpatialFilterType;
MPI_Datatype mpiImageStrType;

void mpiDefineMpiDataTypes()
{
	// Setup MPI Spatial Filter Type.
	const int numOfItemsSFT=3;
	int blocklengthsSFT[3] = {1, 1, 1};
	MPI_Datatype typesSFT[3] = {MPI_INT, MPI_INT, MPI_FLOAT};
	MPI_Aint offsetsSFT[3];

	offsetsSFT[0] = offsetof(SpatialFilter, size);
	offsetsSFT[1] = offsetof(SpatialFilter, requiredImageEdgeExtend);
	offsetsSFT[2] = offsetof(SpatialFilter, scalar);

	MPI_Type_create_struct(numOfItemsSFT, blocklengthsSFT, offsetsSFT, typesSFT, &mpiSpatialFilterType);
	MPI_Type_commit(&mpiSpatialFilterType);

	// Setup MPI Image Str Type.
	const int numOfItemsIST=5;
	int blocklengthsIST[5] = {1, 1, 1, 1, 1};
	MPI_Datatype typesIST[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
	MPI_Aint offsetsIST[5];

	offsetsIST[0] = offsetof(ImageStr, width);
	offsetsIST[1] = offsetof(ImageStr, height);
	offsetsIST[2] = offsetof(ImageStr, columnExtend);
	offsetsIST[3] = offsetof(ImageStr, rowExtend);
	offsetsIST[4] = offsetof(ImageStr, startRowIndex);

	MPI_Type_create_struct(numOfItemsIST, blocklengthsIST, offsetsIST, typesIST, &mpiImageStrType);
	MPI_Type_commit(&mpiImageStrType);
}

//-----------------------------------------------------------------------------

void mpiFreeMpiDataTypes()
{
	MPI_Type_free(&mpiSpatialFilterType);
	MPI_Type_free(&mpiImageStrType);
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

ImageStr* mpiSubDivideAndSendImageStr(ImageStr* imageStr, int numOfProcessors, int srcNode, int mpiRank, int isLog)
{
	// Currently have the assumption that the number of rows in the image
	// is equal or greater than the number of processors.
	if (isLog)
	{
		printf("INFO:(%d): SubDivide: %f\n", mpiRank, ((float)(imageStr->height - (2 * imageStr->rowExtend)) / ((float)numOfProcessors)));
	}

	startRowIndexArray = (int*) malloc(numOfProcessors * sizeof(int));
	startElementArray = (int*) malloc(numOfProcessors * sizeof(int));
	numOfElementsArray = (int*) malloc(numOfProcessors * sizeof(int));

	int sectionRowThickness = (int) ceil(((float)(imageStr->height - (2 * imageStr->rowExtend)) / ((float)numOfProcessors)));
	int finalSectionRowThickness = (imageStr->height - (2 * imageStr->rowExtend)) - (sectionRowThickness * (numOfProcessors - 1) );
	int sectionRowThicknessWithExtend = sectionRowThickness + (2 * imageStr->rowExtend);
	int finalSectionRowThicknessWithExtend = finalSectionRowThickness + (2 * imageStr->rowExtend);

	ImageStr* subImageStr = (ImageStr*) malloc(sizeof(ImageStr));
	subImageStr->width = imageStr->width;
	subImageStr->columnExtend = imageStr->columnExtend;
	subImageStr->rowExtend = imageStr->rowExtend;

	ImageStr* masterSubImageStr = (ImageStr*) malloc(sizeof(ImageStr));
	masterSubImageStr->width = imageStr->width;
	masterSubImageStr->columnExtend = imageStr->columnExtend;
	masterSubImageStr->rowExtend = imageStr->rowExtend;

	if (isLog)
	{
		printf("INFO:(%d): SubDivide: Num of Processors: %d\n", mpiRank, numOfProcessors);
		printf("INFO:(%d): SubDivide: Image Height: %d : Width %d\n", mpiRank, imageStr->height, imageStr->width);
		printf("INFO:(%d): SubDivide: Rows from 0 to %d\n", mpiRank, imageStr->height - 1);
		printf("INFO:(%d): SubDivide: Image Row Extend: %d\n", mpiRank, imageStr->rowExtend);
		printf("INFO:(%d): SubDivide: Number of Elements: %d\n", mpiRank, (imageStr->width * imageStr->height));
		printf("INFO:(%d): SubDivide: Section Row Thickness: %d : With Extend %d\n", mpiRank, sectionRowThickness, sectionRowThicknessWithExtend);
		printf("INFO:(%d): SubDivide: Final Section Row Thickness: %d : With Extend %d\n", mpiRank, finalSectionRowThickness, finalSectionRowThicknessWithExtend);
	}

	int startRowIndex, fromRowIndex, toRowIndex, startElement, toElement, numOfElements;
	int processorIndex;
	for (processorIndex = 0 ; processorIndex < (numOfProcessors - 1) ; processorIndex++)
	{
		startRowIndex = processorIndex * sectionRowThickness + imageStr->rowExtend;
		fromRowIndex = startRowIndex - imageStr->rowExtend;
		toRowIndex = startRowIndex + (sectionRowThickness - 1) + imageStr->rowExtend;
		startElement = fromRowIndex * imageStr->width;
		toElement = startElement + (sectionRowThicknessWithExtend * imageStr->width) - 1;
		numOfElements = sectionRowThicknessWithExtend * imageStr->width;

		startRowIndexArray[processorIndex] = startRowIndex;
		startElementArray[processorIndex] = startElement;
		numOfElementsArray[processorIndex] = numOfElements;

		subImageStr->height = sectionRowThickness + (2 * subImageStr->rowExtend);
		subImageStr->startRowIndex = startRowIndex;

		if (isLog)
		{
			printf("INFO:(%d): SubDivide: Processor: %d\n", mpiRank, processorIndex);
			printf("INFO:(%d): SubDivide: Apply to rows: startRowIndex: %d to (and including) %d\n", mpiRank, startRowIndex, (startRowIndex + sectionRowThickness - 1));
			printf("INFO:(%d): SubDivide: Extends over rows: From %d to (and including) %d\n", mpiRank, fromRowIndex, toRowIndex);
			printf("INFO:(%d): SubDivide: From Element: %d to %d (%d)\n", mpiRank, startElement, toElement, numOfElements);
		}

		if (processorIndex != srcNode)
		{
			printf("INFO:(%d): Sending %d elements to %d\n", mpiRank, numOfElements, processorIndex);
			MPI_Send(subImageStr, 1, mpiImageStrType, processorIndex, mpiTagBase, MPI_COMM_WORLD);
			MPI_Send(&(imageStr->redDataArray[startElement]), numOfElements, MPI_FLOAT, processorIndex, mpiTagRed, MPI_COMM_WORLD);
			MPI_Send(&(imageStr->greenDataArray[startElement]), numOfElements, MPI_FLOAT, processorIndex, mpiTagGreen, MPI_COMM_WORLD);
			MPI_Send(&(imageStr->blueDataArray[startElement]), numOfElements, MPI_FLOAT, processorIndex, mpiTagBlue, MPI_COMM_WORLD);
		}
		else
		{
			printf("INFO:(%d): Processing on master node.\n", mpiRank);
			masterSubImageStr->height = sectionRowThickness;
			masterSubImageStr->startRowIndex = startRowIndex;
		}
	}

	startRowIndex = processorIndex * sectionRowThickness + imageStr->rowExtend;
	fromRowIndex = startRowIndex - imageStr->rowExtend;
	toRowIndex = startRowIndex + (finalSectionRowThickness - 1) + imageStr->rowExtend;
	startElement = fromRowIndex * imageStr->width;
	toElement = startElement + (finalSectionRowThicknessWithExtend * imageStr->width) - 1;
	numOfElements = finalSectionRowThicknessWithExtend * imageStr->width;

	startRowIndexArray[processorIndex] = startRowIndex;
	startElementArray[processorIndex] = startElement;
	numOfElementsArray[processorIndex] = numOfElements;

	subImageStr->height = finalSectionRowThickness + (2 * subImageStr->rowExtend);
	subImageStr->startRowIndex = startRowIndex;

	if (isLog)
	{
		printf("INFO:(%d): SubDivide: Apply to rows: startRowIndex: %d to (and including) %d\n", mpiRank, startRowIndex, (startRowIndex + finalSectionRowThickness - 1));
		printf("INFO:(%d): SubDivide: Extends over rows: From %d to (and including) %d\n", mpiRank, fromRowIndex, toRowIndex);
		printf("INFO:(%d): SubDivide: From Element: %d to %d (%d)\n", mpiRank, startElement, toElement, numOfElements);
	}

	if (processorIndex != srcNode)
	{
		printf("INFO:(%d): Sending %d elements to %d\n", mpiRank, numOfElements, processorIndex);
		MPI_Send(subImageStr, 1, mpiImageStrType, processorIndex, mpiTagBase, MPI_COMM_WORLD);
		MPI_Send(&(imageStr->redDataArray[startElement]), numOfElements, MPI_FLOAT, processorIndex, mpiTagRed, MPI_COMM_WORLD);
		MPI_Send(&(imageStr->greenDataArray[startElement]), numOfElements, MPI_FLOAT, processorIndex, mpiTagGreen, MPI_COMM_WORLD);
		MPI_Send(&(imageStr->blueDataArray[startElement]), numOfElements, MPI_FLOAT, processorIndex, mpiTagBlue, MPI_COMM_WORLD);
	}
	else
	{
		printf("INFO:(%d): Processing on master node.\n", mpiRank);
		masterSubImageStr->height = sectionRowThickness;
		masterSubImageStr->startRowIndex = startRowIndex;
	}

	return masterSubImageStr;
}

//-----------------------------------------------------------------------------

ImageStr* mpiReceiveSubDividedImageStr(int srcNode, int mpiRank, int isLog)
{
	MPI_Status* status;
	ImageStr* subImageStr = (ImageStr*) malloc(sizeof(ImageStr));

	MPI_Recv(subImageStr, 1, mpiImageStrType, srcNode, mpiTagBase, MPI_COMM_WORLD, status);

	if (isLog)
	{
		printf("INFO:(%d): Width: %d\n", mpiRank, subImageStr->width);
		printf("INFO:(%d): Height: %d\n", mpiRank, subImageStr->height);
		printf("INFO:(%d): Column Extend: %d\n", mpiRank, subImageStr->columnExtend);
		printf("INFO:(%d): Row Extend: %d\n", mpiRank, subImageStr->rowExtend);
		printf("INFO:(%d): Start Row Index: %d\n", mpiRank, subImageStr->startRowIndex);
	}

	// Create data array buffers of size number of elements.
	int numOfElements = subImageStr->width * subImageStr->height;
	subImageStr->redDataArray = (float*) malloc(numOfElements * sizeof(float));
	subImageStr->greenDataArray = (float*) malloc(numOfElements * sizeof(float));
	subImageStr->blueDataArray = (float*) malloc(numOfElements * sizeof(float));

	if (isLog)
	{
		printf("INFO:(%d): Receiving %d elements\n", mpiRank, numOfElements);
	}
	// Receive the three data arrays.
	MPI_Recv(subImageStr->redDataArray, numOfElements, MPI_FLOAT, srcNode, mpiTagRed, MPI_COMM_WORLD, status);
	MPI_Recv(subImageStr->greenDataArray, numOfElements, MPI_FLOAT, srcNode, mpiTagGreen, MPI_COMM_WORLD, status);
	MPI_Recv(subImageStr->blueDataArray, numOfElements, MPI_FLOAT, srcNode, mpiTagBlue, MPI_COMM_WORLD, status);

	return subImageStr;
}

//-----------------------------------------------------------------------------

int mpiSendSubDividedImageStr(ImageStr* processedSubImageStr, int srcNode, int mpiRank, int isLog)
{
	if (isLog)
	{
		printf("INFO:(%d): Sending processed data back.\n", mpiRank);
	}

	int startRow = processedSubImageStr->rowExtend;
	int numOfElements = (processedSubImageStr->height - (2 * processedSubImageStr->rowExtend)) * processedSubImageStr->width;
	int startElement = startRow * processedSubImageStr->width;

	if (isLog)
	{
		printf("INFO:(%d): StartRow %d : StartElement %d: Sending %d elements to %d\n", mpiRank, startRow, startElement, numOfElements, srcNode);
	}

	MPI_Request request[3];
	MPI_Status status[3];

	// Send the three data arrays.
	MPI_Isend(&(processedSubImageStr->redDataArray[startElement]), numOfElements, MPI_FLOAT, srcNode, mpiTagRed, MPI_COMM_WORLD, &request[0]);
	MPI_Isend(&(processedSubImageStr->greenDataArray[startElement]), numOfElements, MPI_FLOAT, srcNode, mpiTagGreen, MPI_COMM_WORLD, &request[1]);
	MPI_Isend(&(processedSubImageStr->blueDataArray[startElement]), numOfElements, MPI_FLOAT, srcNode, mpiTagBlue, MPI_COMM_WORLD, &request[2]);

	MPI_Waitall(3, request, status);

	return SUCCESS;
}

//-----------------------------------------------------------------------------

int mpiReceiveAllSubDividedImageStr(ImageStr* imageStr, int numOfProcessors, int srcNode, int mpiRank, int isLog)
{
	int startRowIndex, startElement, numOfElements;

	MPI_Status* status = (MPI_Status*) malloc(3 * (numOfProcessors -1) * sizeof(MPI_Status));
	MPI_Request* request = (MPI_Request*) malloc(3 * (numOfProcessors -1) * sizeof(MPI_Request));
	int requestIndex = 0;

	int processorIndex;
	for (processorIndex = 0 ; processorIndex < numOfProcessors ; processorIndex++)
	{
		if (processorIndex != srcNode)
		{
			startRowIndex = startRowIndexArray[processorIndex];
			startElement = startElementArray[processorIndex];
			numOfElements = numOfElementsArray[processorIndex];

			if (isLog)
			{
				printf("INFO:(%d): ReceiveSubDivide: Processor: %d\n", mpiRank, processorIndex);
				printf("INFO:(%d): ReceiveSubDivide: startRowIndex: %d : startElement %d : numOfElements %d\n", mpiRank, startRowIndex, startElement, numOfElements);
				printf("INFO:(%d): Sending %d elements to %d\n", mpiRank, numOfElements, processorIndex);
			}

			MPI_Irecv(&(imageStr->redDataArray[startElement]), numOfElements, MPI_FLOAT, processorIndex, mpiTagRed, MPI_COMM_WORLD, &request[requestIndex]);
			requestIndex++;
			MPI_Irecv(&(imageStr->greenDataArray[startElement]), numOfElements, MPI_FLOAT, processorIndex, mpiTagGreen, MPI_COMM_WORLD, &request[requestIndex]);
			requestIndex++;
			MPI_Irecv(&(imageStr->blueDataArray[startElement]), numOfElements, MPI_FLOAT, processorIndex, mpiTagBlue, MPI_COMM_WORLD, &request[requestIndex]);
			requestIndex++;
		}
	}

	MPI_Waitall(3 * (numOfProcessors - 1), request, status);

	free(startRowIndexArray);
	free(startElementArray);
	free(numOfElementsArray);

	return SUCCESS;
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


