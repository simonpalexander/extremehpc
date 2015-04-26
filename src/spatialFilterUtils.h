/*
 * spatialFilterUtils.h
 *
 *  Created on: Apr 25, 2015
 *      Author: simon
 */

#ifndef SRC_SPATIALFILTERUTILS_H_
#define SRC_SPATIALFILTERUTILS_H_

// Assume a square filter;
typedef struct spatial_filter_structure	{
	int size;
	int* filter; } SpatialFilter;

SpatialFilter* createSpatialFilter(int size);
void cleanUpSpatialFilter(SpatialFilter* spatialFilter);
int setFilterElement(SpatialFilter* spatialFilter, int row, int col, int value);
void printSpatialFilter(SpatialFilter* spatialFilter);

#endif /* SRC_SPATIALFILTERUTILS_H_ */
