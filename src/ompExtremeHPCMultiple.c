/*
 * ompExtremeHPC.c
 *
 *  Created on: Apr 29, 2015
 *      Author: simon
 */


/*
 * mainExtremeHPC.c
 *
 *  Created on: Apr 25, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>

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

int isLog;
char* inputFilename = "resources/input.tga";
char* outputFilename = "resources/output.tga";
char* spatialFilterFilename = "resources/spatialFilter.txt";
char* resultsFilename = "omp-extremehpc-results.txt";

int processComandLineArguments(int argc, char **argv);

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
			"resources/spatialFilter11by11.txt",
			"resources/spatialFilter13by13.txt",
			"resources/spatialFilter15by15.txt",
			"resources/spatialFilter17by17.txt",
			"resources/spatialFilter19by19.txt",
			"resources/spatialFilter21by21.txt" };

	//if (!successful(processComandLineArguments(argc, argv))) {
	//	printf("ERROR: Exiting program.\n");
	//	return FAIL;
	//}

	int numOfInputFiles = 3;
	int numOfSpatialFilterFiles = 10;
	printf("INFO: Starting Extreme HPC Project Program:\n");
	printf("INFO: Results filename: %s\n", resultsFilename);

	char numThreadsStr[4];
	int numOfThreads;
	omp_set_num_threads(3);
	#pragma omp parallel
	{
		#pragma omp master
		{
			int threadId = omp_get_thread_num();
			numOfThreads = omp_get_num_threads();
			printf("INFO: Thread %d: OMP Settings: Num of Threads: %d\n", threadId, numOfThreads);
			sprintf(numThreadsStr, "%d", numOfThreads);
		}
	}

	FILE *file = fopen(resultsFilename, "wt");
	if (file == NULL)
	{
		printf("ERROR: Error opening results file: %s\n", resultsFilename);
		return FAIL;
	}

	int inputIndex, spatialFilterIndex;
	for (inputIndex = 0 ; inputIndex < numOfInputFiles ; inputIndex++)
	{
		inputFilename = inputFilenames[inputIndex];
		for (spatialFilterIndex = 0 ; spatialFilterIndex < numOfSpatialFilterFiles ; spatialFilterIndex++)
		{
			spatialFilterFilename = spatialFilterFilenames[spatialFilterIndex];

			printf("INFO: - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
			printf("INFO: Image filename: %s\n", inputFilename);
			printf("INFO: SpatialFilter: %s\n", spatialFilterFilename);

			TimeTracker* timeTracker = createTimeTracker("TimeTracker");
			addTrackingPoint(timeTracker, "Started");

			TgaImage* tgaImage = readTGAFile(inputFilename);
			if (!tgaImage) {
				printf("ERROR: Exiting program.\n");
				return FAIL;
			}

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

			printf("INFO: Starting Image Processing.\n");
			addTrackingPoint(timeTracker, "Initialized");

			#pragma omp parallel
			{
				int componentID = omp_get_thread_num();
				applySpatialFilterToImageStrComponentArray(spatialFilter, imageStr, processedImageStr, componentID);
			}

			addTrackingPoint(timeTracker, "Processed");
			printf("INFO: Finished Image Processing.\n");

			cleanUpImageStr(imageStr);

			TgaImage* processedTgaImage = createTgaImageFromImageStr(processedImageStr);
			if (!successful(saveTGAImage(outputFilename, processedTgaImage))) {
				printf("ERROR: Exiting program.\n");
				return FAIL;
			}

			cleanUpImageStr(processedImageStr);

			addTrackingPoint(timeTracker, "Finished");
			printTimeTracker(timeTracker);

			//fprintf(file, "- - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
			//fprintf(file, "OMP: Num of Threads: %d\n", numOfThreads);
			//fprintf(file, "isLog: %d\n", isLog);
			//fprintf(file, "Input: %s\n", inputFilename);
			//fprintf(file, "Output: %s\n", outputFilename);
			//fprintf(file, "Image,%d,%d,%d\n", processedTgaImage->width, processedTgaImage->height,processedTgaImage->numOfPixels);

			//fprintf(file, "SpatialFilter: %s\n", spatialFilterFilename);
			//writeSpatialFilterToFile(spatialFilter, file);

			//writeTimeTrackerHeader(file);
			//writeTimeTrackerFile(timeTracker, file);
			writeTimeTrackerDetailsFile(timeTracker, file, 1, processedTgaImage->height, processedTgaImage->width, spatialFilter->size, 0);

			cleanUpTgaImage(processedTgaImage);
			cleanUpSpatialFilter(spatialFilter);
			free(timeTracker);
		}
	}

	fclose(file);

	printf("INFO: Exiting Program.\n");
	return SUCCESS;
}

//-----------------------------------------------------------------------------

int processComandLineArguments(int argc, char **argv)
{
	if (argc == 2) {
		isLog = atoi(argv[1]);
	}
	else if (argc == 3) {
		isLog = atoi(argv[1]);
		inputFilename = argv[2];
	}
	else if (argc == 4) {
		isLog = atoi(argv[1]);
		inputFilename = argv[2];
		spatialFilterFilename = argv[3];
	}
	else if (argc == 5) {
		isLog = atoi(argv[1]);
		inputFilename = argv[2];
		spatialFilterFilename = argv[3];
		outputFilename = argv[4];
	}
	else if (argc>1 && argc > 5) {
		printf("ERROR: Problem parsing command line arguments:\n");
		printf("1) isLog: 0 false : 1 true\n");
		printf("2) input filename : Default: resources/input.tga:\n");
		printf("3) spatial filter filename : Default: resources/spatialFilter.txt:\n");
		printf("4) output filename : Default: resources/output.tga:\n");
		return FAIL;
	}
	else {
		printf("INFO: The command line had no other arguments. Using defaults.\n");
	}

	printf("INFO: Command Line Arguments:\n");
	printf("\tisLog: %d\n", isLog);
	printf("\tInput: %s\n", inputFilename);
	printf("\tSpatial Filter: %s\n", spatialFilterFilename);
	printf("\tOutput: %s\n", outputFilename);

	return SUCCESS;
}
