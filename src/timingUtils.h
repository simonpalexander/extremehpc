/*
 * timingUtils.h
 *
 *  Created on: Apr 28, 2015
 *      Author: simon
 */

#ifndef SRC_TIMINGUTILS_H_
#define SRC_TIMINGUTILS_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>

typedef struct time_tracker_structure	{
	char trackerName[64];
	char trackerPointLabel[16][64];
	struct timeval trackerPointTiming[16];
	float interval[16];
	int lastIndex; } TimeTracker;

TimeTracker* createTimeTracker(char* trackerName);
void addTrackingPoint(TimeTracker* timeTracker, char* label);
void calculateInterals(TimeTracker* timeTracker);
void calculateInteral(TimeTracker* timeTracker, int startIndex, int endIndex);
float getInterval(TimeTracker* timeTracker, int intervalIndex);
void printTimeTracker(TimeTracker* timeTracker);

void writeTimeTrackerFile(TimeTracker* timeTracker, FILE* file);
void writeTimeTrackerHeader(FILE* file);

#endif /* SRC_TIMINGUTILS_H_ */
