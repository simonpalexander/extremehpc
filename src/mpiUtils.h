/*
 * mpiUtilities.h
 *
 *  Created on: May 5, 2015
 *      Author: simon
 */

#ifndef SRC_MPIUTILS_H_
#define SRC_MPIUTILS_H_

#include "spatialFilterUtils.h"

void defineMpiDataTypes();
void freeMpiDataTypes();

int mpiSendIsLog(int isLog, int srcNode);
int mpiReceiveIsLog(int srcNode, int mpiRank);
int mpiSendSpatialFilter(SpatialFilter* spatialFilter, int srcNode);
SpatialFilter* mpiReceiveSpatialFilter(int srcNode, int mpiRank, int isLog);

void mpiPrintSpatialFilter(SpatialFilter* spatialFilter, int mpiRank);


#endif /* SRC_MPIUTILS_H_ */
