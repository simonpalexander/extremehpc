/*
 * mpiExtremeHPC.c
 *
 *  Created on: May 2, 2015
 *      Author: simon
 */


#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"
#include <omp.h>

#include "globalDefines.h"
#include "tgaUtils.h"

#include "proprtiesFileUtils.h"
#include "spatialFilterUtils.h"
#include "imageProcessingUtils.h"
#include "timingUtils.h"

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
//Done Add timing utilities.
//Done: Consider strategy where only data array relating to component being processed is malloc-ed minimizing amount of active memory use. My machine only has 8G, other machines will have more, consider if it becomes a problem.
//Done: Write results to a text file.
//Done: Basic OMP functionality added to create a thread for each data array.
//ToDo: Can further OMP optimization be added. On blue waters, each processor has 8/16 cores.
//ToDo: Add MPI functionality.
//ToDo: Consider how data array will be divided up with respect to the number of nodes. Image is already extended w.r.t. spatial filter.
//ToDo: Consider how when sections of images are sent cross the network, they are 'pre-processed' with the 'extend halo' already accounted for.
//ToDo: Create MPI structure to send spatial filter and other 'extend' image related details.
//ToDo: Create MPI structure to send data array across netwrok.
//Done: Setup cygwin openmpi environment.

/*
 * Compiling on Windows 7 Platform:
 * Open CYGWIN console.
 * Navigate to: /cygdrive/c/Development/workspace/ExtremeHPC
 * Run make mpiExtremeHPC
 */

#define MPI_ROOT_NODE 0

char* inputFilename = "resources/input.tga";
char* outputFilename = "resources/output.tga";
char* spatialFilterFilename = "resources/spatialFilter.txt";

// MPI: Number of processes
int mpiNumOfProcessors;

const int mpiTag = 2;

MPI_Datatype mpiSpatialFilterType;

int processComandLineArguments(int argc, char **argv);

int main(int argc, char **argv)
{
	// Initialize MPI
	// MPI: Rank of process.
	int mpiRank;
	int mpiProvided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &mpiProvided);
	// MPI_THREAD_MULTIPLE, MPI_THREAD_SINGLE

	MPI_Comm_size(MPI_COMM_WORLD, &mpiNumOfProcessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);

	omp_set_num_threads(3);

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


	if (mpiRank == MPI_ROOT_NODE)
	{
		printf("INFO:(%d): Starting MPI Extreme HPC Project Program:\n", mpiRank);

		char numThreadsStr[4];
		int numOfThreads;

		omp_set_num_threads(3);
		#pragma omp parallel
		{
			#pragma omp master
			{
				int threadId = omp_get_thread_num();
				numOfThreads = omp_get_num_threads();
				printf( "Thread %d: OMP Settings: Num of Threads: %d\n", threadId, numOfThreads);
				sprintf(numThreadsStr, "%d", numOfThreads);
			}
		}

		TimeTracker* timeTracker = createTimeTracker("TT1");
		addTrackingPoint(timeTracker, "AppStart");

		char filenameBase[256];
		time_t now = time(NULL);
		struct tm *t = localtime(&now);
		strftime(filenameBase, sizeof(filenameBase)-1, "extremehpc-mpi-results-%Y%m%d-%H%M", t);
		char resultsFilename[256];
		snprintf(resultsFilename, sizeof(resultsFilename), "%s.txt", filenameBase);
		printf("INFO:(%d): Results filename: %s\n", mpiRank, resultsFilename);

		if (!successful(processComandLineArguments(argc, argv))) {
			printf("INFO:(%d): ERROR: Exiting program.\n", mpiRank);
			return FAIL;
		}

		TgaImage* tgaImage = readTGAFile(inputFilename);
		if (!tgaImage) {
			printf("INFO:(%d): ERROR: Exiting program.\n", mpiRank);
			return FAIL;
		}

		SpatialFilter* spatialFilter = readSpatialFilterProprtyFile(spatialFilterFilename);
		if (!spatialFilter) {
			printf("INFO:(%d): ERROR: Exiting program.\n", mpiRank);
			return FAIL;
		}

		ImageStr* imageStr = createImageStrFromTgaImageForSpatialFilter(tgaImage, spatialFilter);
		if (!imageStr) {
			printf("INFO:(%d): ERROR: Exiting program.\n", mpiRank);
			return FAIL;
		}
		cleanUpTgaImage(tgaImage);

		printf("INFO:(%d): size: %d\n", mpiRank, spatialFilter->size);
		printf("INFO:(%d): requiredImageEdgeExtend: %d\n", mpiRank, spatialFilter->requiredImageEdgeExtend);
		printf("INFO:(%d): scalar: %f\n", mpiRank, spatialFilter->scalar);

		int processorIndex;
		for (processorIndex=1 ; processorIndex<mpiNumOfProcessors ; processorIndex++) {
			MPI_Send(spatialFilter, 1, mpiSpatialFilterType, processorIndex, mpiTag, MPI_COMM_WORLD);
		}

		/*
		ImageStr* processedImageStr = createImageStr(imageStr);
		if (!processedImageStr) {
			printf("INFO:(%d): ERROR: Exiting program.\n", mpiRank);
			return FAIL;
		}

		addTrackingPoint(timeTracker, "AppInit");

		addTrackingPoint(timeTracker, "AppProcessStart");

		//#pragma omp parallel
		//{
		//	int componentID = omp_get_thread_num();
		//	applySpatialFilterToImageStrComponentArray(spatialFilter, imageStr, processedImageStr, componentID);
		//}

		addTrackingPoint(timeTracker, "AppProcessEnd");

		cleanUpImageStr(imageStr);

		TgaImage* processedTgaImage = createTgaImageFromImageStr(processedImageStr);
		if (!successful(saveTGAImage(outputFilename, processedTgaImage))) {
			printf("INFO:(%d): ERROR: Exiting program.\n"mpiRank, );
			return FAIL;
		}

		cleanUpImageStr(processedImageStr);

		addTrackingPoint(timeTracker, "AppEnd");
		printTimeTracker(timeTracker);

		FILE *file = fopen(resultsFilename, "wt");
		if (file == NULL)
		{
			printf("INFO:(%d): ERROR: Error opening results file: %s\n", mpiRank, resultsFilename);
			return FAIL;
		}

		fprintf(file, "OMP: Num of Threads: %d\n", numOfThreads);
		fprintf(file, "Input: %s\n", inputFilename);
		fprintf(file, "Output: %s\n", outputFilename);
		fprintf(file, "Image,%d,%d,%d\n", processedTgaImage->width, processedTgaImage->height,processedTgaImage->numOfPixels);

		writeTimeTrackerHeader(file);
		writeTimeTrackerFile(timeTracker, file);

		fprintf(file, "SpatialFIlter: %s\n", spatialFilterFilename);
		writeSpatialFilterToFile(spatialFilter, file);
		fclose(file);
		*/
		//cleanUpTgaImage(processedTgaImage);
		cleanUpSpatialFilter(spatialFilter);
		free(timeTracker);
	}
	else
	{
		MPI_Status status;
		const int src = MPI_ROOT_NODE;

		SpatialFilter* spatialFilter = (SpatialFilter*) malloc(sizeof(SpatialFilter));
		MPI_Recv(spatialFilter, 1, mpiSpatialFilterType, src, mpiTag, MPI_COMM_WORLD, &status);

		printf("INFO:(%d): size: %d\n", mpiRank, spatialFilter->size);
		printf("INFO:(%d): requiredImageEdgeExtend: %d\n", mpiRank, spatialFilter->requiredImageEdgeExtend);
		printf("INFO:(%d): scalar: %f\n", mpiRank, spatialFilter->scalar);

		free(spatialFilter);
	}

	// Barrier at the point where the spatial filter information
	// has been sent to the processors.
	MPI_Barrier(MPI_COMM_WORLD);

	if (mpiRank == MPI_ROOT_NODE){
		printf("INFO:(%d): Exiting Program: %d\n", mpiRank, SUCCESS);
	}

	// Free derived MPI data type.
	MPI_Type_free(&mpiSpatialFilterType);
	// Shutdown MPI
	MPI_Finalize();

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
