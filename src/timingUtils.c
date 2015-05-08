/*
 * timingUtils.c
 *
 *  Created on: Apr 28, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timingUtils.h"

extern int isLog;

TimeTracker* createTimeTracker(char* trackerName)
{
	TimeTracker* timeTracker = (TimeTracker*) malloc(sizeof(TimeTracker));
	strcpy (timeTracker->trackerName, trackerName);
	timeTracker->lastIndex = 0;
	return(timeTracker);
}

//-----------------------------------------------------------------------------

void addTrackingPoint(TimeTracker* timeTracker, char* label)
{
	strcpy (timeTracker->trackerPointLabel[timeTracker->lastIndex], label);
	if (isLog) { printf("Info: TimeTracker: Added label: %s\n", timeTracker->trackerPointLabel[timeTracker->lastIndex]); }
	gettimeofday(&(timeTracker->trackerPointTiming[timeTracker->lastIndex]), 0);
	timeTracker->lastIndex++;
}

//-----------------------------------------------------------------------------

void calculateInterals(TimeTracker* timeTracker)
{
	int index;

	timeTracker->interval[0] = 0.0f;
	for (index=1 ; index<timeTracker->lastIndex ; index++)
	{
		timeTracker->interval[index] =
				((timeTracker->trackerPointTiming[index]).tv_sec - (timeTracker->trackerPointTiming[index-1]).tv_sec)
				+ 1.0e-6 * ((timeTracker->trackerPointTiming[index]).tv_usec - (timeTracker->trackerPointTiming[index-1]).tv_usec);
	}
}

//-----------------------------------------------------------------------------

void calculateInteral(TimeTracker* timeTracker, int startIndex, int endIndex)
{
	float interval =
			((timeTracker->trackerPointTiming[endIndex]).tv_sec - (timeTracker->trackerPointTiming[startIndex]).tv_sec)
			+ 1.0e-6 * ((timeTracker->trackerPointTiming[endIndex]).tv_usec - (timeTracker->trackerPointTiming[startIndex]).tv_usec);

	printf("TimeTracker: Interval between %d to %d: %f\n", startIndex, endIndex, interval);
}

//-----------------------------------------------------------------------------

float getInterval(TimeTracker* timeTracker, int intervalIndex)
{
	calculateInterals(timeTracker);

	if (intervalIndex<=timeTracker->lastIndex)
	{
		return timeTracker->interval[intervalIndex];
	}

	return -1.0;
}

//-----------------------------------------------------------------------------

void printTimeTracker(TimeTracker* timeTracker)
{
	calculateInterals(timeTracker);

	int index;
	printf("INFO: TimeTracker: %s\n", timeTracker->trackerName);
	for (index=0 ; index<timeTracker->lastIndex ; index++)
	{
		printf("%d): %s: %f\n", index, timeTracker->trackerPointLabel[index], timeTracker->interval[index]);
	}
}

//-----------------------------------------------------------------------------

void writeTimeTrackerHeader(FILE* file)
{
	fprintf(file,"Tracker,Label,Sec,Usec,Interval\n");
}

//-----------------------------------------------------------------------------

void writeTimeTrackerFile(TimeTracker* timeTracker, FILE* file)
{
	calculateInterals(timeTracker);

	int index;
	for (index=0 ; index<timeTracker->lastIndex ; index++)
	{
		fprintf(file, "%s,%s,%d,%d,%f\n", timeTracker->trackerName, timeTracker->trackerPointLabel[index], (int)(timeTracker->trackerPointTiming[index]).tv_sec, (int)(timeTracker->trackerPointTiming[index]).tv_usec, timeTracker->interval[index]);
	}
}
