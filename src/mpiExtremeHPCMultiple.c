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
#include "mpiUtils.h"

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
//Done: Add MPI functionality.
//Done: Consider how data array will be divided up with respect to the number of nodes. Image is already extended w.r.t. spatial filter.
//Done: Consider how when sections of images are sent cross the network, they are 'pre-processed' with the 'extend halo' already accounted for.
//Done: Create MPI structure to send spatial filter and other 'extend' image related details.
//Done: Create MPI structure to send data array across network.
//Done: Setup cygwin openmpi environment.
//Done: Added assumption that image has more rows than number of processors being used.

/*
 * Compiling on Windows 7 Platform:
 * Open CYGWIN console.
 * Navigate to: /cygdrive/c/Development/workspace/ExtremeHPC
 * Run make mpiExtremeHPC
 */

#define MPI_ROOT_NODE 0

int isLog;
char* inputFilename = "resources/input.tga";
char* outputFilename = "resources/output.tga";
char* spatialFilterFilename = "resources/spatialFilter.txt";
char* resultsFilename = "mpi-extremehpc-results.txt";

// MPI: Number of processes
int mpiNumOfProcessors;
// OMP: Number of threads
int numOfThreads;

FILE *file;

int processComandLineArguments(int argc, char **argv, int mpiRank);

int main(int argc, char **argv)
{
	char* inputFilenames[] = {
			"resources/Orion_Nebula_1000.tga",
			"resources/Orion_Nebula_2000.tga",
			"resources/Orion_Nebula_4000.tga",
			"resources/Orion_Nebula_6000.tga",
			"resources/Orion_Nebula_8000.tga",
			"resources/Orion_Nebula_10000.tga",
			"resources/Orion_Nebula_12000.tga",
			"resources/Orion_Nebula_14000.tga",
			"resources/Orion_Nebula_16000.tga",
			"resources/Orion_Nebula_18000.tga",
			"resources/Orion_Nebula_20000.tga" };

	char* spatialFilterFilenames[] = {
			"resources/spatialFilter3by3.txt",
			"resources/spatialFilter5by5.txt",
			"resources/spatialFilter7by7.txt",
			"resources/spatialFilter9by9.txt",
			"resources/spatialFilter11by11txt",
			"resources/spatialFilter13by13txt",
			"resources/spatialFilter15by15txt",
			"resources/spatialFilter17by17txt",
			"resources/spatialFilter19by19txt",
			"resources/spatialFilter21by21txt" };

	// Initialize MPI
	// MPI: Rank of process.
	int mpiRank;
	int mpiProvided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &mpiProvided);

	MPI_Comm_size(MPI_COMM_WORLD, &mpiNumOfProcessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);

	omp_set_num_threads(3);

	mpiDefineMpiDataTypes();

	if (mpiRank == MPI_ROOT_NODE)
	{
		printf("INFO:(%d): Starting MPI Extreme HPC Project Program: NumOfProcessors: %d\n", mpiRank, mpiNumOfProcessors);

		if (!successful(processComandLineArguments(argc, argv, mpiRank))) {
			printf("ERROR:(%d): Exiting program.\n", mpiRank);
			return FAIL;
		}

		char numThreadsStr[4];

		omp_set_num_threads(3);
		#pragma omp parallel
		{
			#pragma omp master
			{
				int threadId = omp_get_thread_num();
				numOfThreads = omp_get_num_threads();
				printf( "INFO:(%d): Thread(%d): OMP Settings: Num of Threads: %d\n", mpiRank, threadId, numOfThreads);
				sprintf(numThreadsStr, "%d", numOfThreads);
			}
		}

		file = fopen(resultsFilename, "wt");
		if (file == NULL)
		{
			printf("ERROR:(%d): Error opening results file: %s\n", mpiRank, resultsFilename);
			return FAIL;
		}
	}

	int numOfInputFiles = 11;
	int numOfSpatialFilterFiles = 10;
	printf("INFO: Starting Extreme HPC Project Program:\n");
	printf("INFO: Results filename: %s\n", resultsFilename);

	int inputIndex, spatialFilterIndex;

	for (inputIndex = 0 ; inputIndex < numOfInputFiles ; inputIndex++)
	{
		inputFilename = inputFilenames[inputIndex];
		for (spatialFilterIndex = 0 ; spatialFilterIndex < numOfSpatialFilterFiles ; spatialFilterIndex++)
		{
			spatialFilterFilename = spatialFilterFilenames[spatialFilterIndex];

			if (mpiRank == MPI_ROOT_NODE)
			{
				printf("INFO: - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf("INFO: Input file: %s\n", inputFilename);
				printf("INFO: SpatialFilter: %s\n", spatialFilterFilename);

				TimeTracker* timeTracker = createTimeTracker("TimeTracker");
				addTrackingPoint(timeTracker, "Started");

				mpiSendIsLog(isLog, MPI_ROOT_NODE);

				TgaImage* tgaImage = readTGAFile(inputFilename);
				if (!tgaImage) {
					printf("ERROR:(%d): Exiting program.\n", mpiRank);
					return FAIL;
				}

				SpatialFilter* spatialFilter = readSpatialFilterProprtyFile(spatialFilterFilename);
				if (!spatialFilter) {
					printf("ERROR:(%d): Exiting program.\n", mpiRank);
					return FAIL;
				}
				if (isLog) {
					mpiPrintSpatialFilter(spatialFilter, mpiRank);
				}

				ImageStr* imageStr = createImageStrFromTgaImageForSpatialFilter(tgaImage, spatialFilter);
				if (!imageStr) {
					printf("ERROR:(%d): Exiting program.\n", mpiRank);
					return FAIL;
				}
				cleanUpTgaImage(tgaImage);

				mpiSendSpatialFilter(spatialFilter, MPI_ROOT_NODE);

				ImageStr* processedImageStr = createImageStr(imageStr);
				if (!processedImageStr) {
					printf("ERROR:(%d): Exiting program.\n", mpiRank);
					return FAIL;
				}

				printf("INFO(%d): Starting Image Processing.\n", mpiRank);
				addTrackingPoint(timeTracker, "Initialized");

				TimeTracker* timeTracker2 = createTimeTracker("TimeTracker2");
				addTrackingPoint(timeTracker2, "Start Processing");

				// Sub-divide imageStr and send sub data arrays to processors.
				ImageStr* subImageStr = mpiSubDivideAndSendImageStr(imageStr, mpiNumOfProcessors, MPI_ROOT_NODE, mpiRank, isLog);

				printf("INFO(%d): Data Sent.\n", mpiRank);
				addTrackingPoint(timeTracker, "Sent");

				#pragma omp parallel
				{
					int componentID = omp_get_thread_num();
					applySpatialFilterToSubImageStrComponentArray(spatialFilter, imageStr, processedImageStr, subImageStr, componentID);
				}

				addTrackingPoint(timeTracker, "Processed");

				mpiReceiveAllSubDividedImageStr(processedImageStr, mpiNumOfProcessors, MPI_ROOT_NODE, mpiRank, isLog);

				addTrackingPoint(timeTracker, "Received");
				printf("INFO(%d): Data Received.\n", mpiRank);

				addTrackingPoint(timeTracker2, "Finished Processing");
				printf("INFO(%d): Finished Image Processing.\n", mpiRank);

				cleanUpImageStr(imageStr);

				TgaImage* processedTgaImage = createTgaImageFromImageStr(processedImageStr);
				if (!successful(saveTGAImage(outputFilename, processedTgaImage))) {
					printf("ERROR:(%d): Exiting program.\n", mpiRank);
					return FAIL;
				}

				cleanUpImageStr(processedImageStr);

				addTrackingPoint(timeTracker, "Finished");
				printTimeTracker(timeTracker);
				printTimeTracker(timeTracker2);

				fprintf(file, "- - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
				fprintf(file, "MPI: Num of Processors: %d\n", mpiNumOfProcessors);
				fprintf(file, "OMP: Num of Threads: %d\n", numOfThreads);
				//fprintf(file, "isLog: %d\n", isLog);
				fprintf(file, "Input: %s\n", inputFilename);
				fprintf(file, "Output: %s\n", outputFilename);
				fprintf(file, "Image,%d,%d,%d\n", processedTgaImage->width, processedTgaImage->height,processedTgaImage->numOfPixels);

				fprintf(file, "SpatialFilter: %s\n", spatialFilterFilename);
				//writeSpatialFilterToFile(spatialFilter, file);

				//writeTimeTrackerHeader(file);
				writeTimeTrackerFile(timeTracker, file);
				writeTimeTrackerFile(timeTracker2, file);

				cleanUpTgaImage(processedTgaImage);
				cleanUpSpatialFilter(spatialFilter);
				free(timeTracker);
				free(timeTracker2);
			}
			else
			{
				isLog = mpiReceiveIsLog(MPI_ROOT_NODE, mpiRank);

				SpatialFilter* spatialFilter = mpiReceiveSpatialFilter(MPI_ROOT_NODE, mpiRank, isLog);

				ImageStr* subImageStr = mpiReceiveSubDividedImageStr(MPI_ROOT_NODE, mpiRank, isLog);

				ImageStr* processedSubImageStr = createImageStr(subImageStr);
				if (!processedSubImageStr) {
					printf("ERROR:(%d): Exiting program.\n", mpiRank);
					return FAIL;
				}

				#pragma omp parallel
				{
					int componentID = omp_get_thread_num();
					applySpatialFilterToImageStrComponentArray(spatialFilter, subImageStr, processedSubImageStr, componentID);
				}

				mpiSendSubDividedImageStr(processedSubImageStr, MPI_ROOT_NODE, mpiRank, isLog);

				free(spatialFilter);
				free(subImageStr);
				free(processedSubImageStr);
			}

			if (mpiRank == MPI_ROOT_NODE){
				printf("INFO:(%d): Exiting Program: %d\n", mpiRank, SUCCESS);
			}


		}
	}

	if (mpiRank == MPI_ROOT_NODE)
	{
		fclose(file);
	}

	// Free derived MPI data type.
	mpiDefineMpiDataTypes();
	// Shutdown MPI
	MPI_Finalize();

	return SUCCESS;
}

//-----------------------------------------------------------------------------

int processComandLineArguments(int argc, char **argv, int mpiRank)
{
	if (argc == 2) {
		resultsFilename = argv[1];
	}
	else if (argc > 2) {
		printf("ERROR:(%d): Problem parsing command line arguments:\n", mpiRank);
		printf("ERROR:(%d): 1) results filename\n", mpiRank);
		return FAIL;
	}
	else {
		printf("INFO:(%d): The command line had no other arguments. Using defaults.\n", mpiRank);
	}

	return SUCCESS;
}
