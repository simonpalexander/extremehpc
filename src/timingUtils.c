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
	printf("TimeTracker: Added label: %s\n", timeTracker->trackerPointLabel[timeTracker->lastIndex]);
	gettimeofday(&(timeTracker->trackerPointTiming[timeTracker->lastIndex]), 0);
	timeTracker->lastIndex++;
}

//-----------------------------------------------------------------------------

void calculateInterals(TimeTracker* timeTracker)
{
	//printf("TimeTracker: Calculating Intervals: %s:\n", timeTracker->trackerName);
	int index;

	timeTracker->interval[0] = 0.0f;
	for (index=1 ; index<timeTracker->lastIndex ; index++)
	{
		timeTracker->interval[index] =
				((timeTracker->trackerPointTiming[index]).tv_sec - (timeTracker->trackerPointTiming[index-1]).tv_sec)
				+ 1.0e-6 * ((timeTracker->trackerPointTiming[index]).tv_usec - (timeTracker->trackerPointTiming[index-1]).tv_usec);
	}
	//printf("TimeTracker: Intervals calculated.\n");
}

//-----------------------------------------------------------------------------

void calculateInteral(TimeTracker* timeTracker, int startIndex, int endIndex)
{
	//printf("TimeTracker: Calculating Interval: Between %d and %d:\n", startIndex, endIndex);

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
	printf("TimeTracker: %s\n", timeTracker->trackerName);
	for (index=0 ; index<timeTracker->lastIndex ; index++)
	{
		printf("TimeTracker: %d): %s: %f\n", index, timeTracker->trackerPointLabel[index], timeTracker->interval[index]);
	}
}

