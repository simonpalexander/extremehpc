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
	int requiredImageEdgeExtend;
	float scalar;
	float* filter; } SpatialFilter;

SpatialFilter* createSpatialFilter(int size, float scalar, float* numArray);
void cleanUpSpatialFilter(SpatialFilter* spatialFilter);
int setFilterElement(SpatialFilter* spatialFilter, int row, int col, float value);
void printSpatialFilter(SpatialFilter* spatialFilter);

void writeSpatialFilterToFile(SpatialFilter* spatialFilter, FILE* file);

#endif /* SRC_SPATIALFILTERUTILS_H_ */
