/*
 * ImageProcessingUtils.h
 *
 *  Created on: Apr 26, 2015
 *      Author: simon
 */

#ifndef SRC_IMAGEPROCESSINGUTILS_H_
#define SRC_IMAGEPROCESSINGUTILS_H_

#include "tgaUtils.h"
#include "spatialFilterUtils.h"

#define isRed(component) (component == red)? (true) : (false)
#define isGreen(component) (component == green)? (true) : (false)
#define isBlue(component) (component == blue)? (true) : (false)

// Types of edge handling.
enum { crop, pad, extend, wrap };

enum {red, green, blue, alpha };

typedef struct image_structure	{
	int width;
	int height;
	int columnExtend;
	int rowExtend;
	int startRowIndex;
	float *redDataArray;
	float *greenDataArray;
	float *blueDataArray; } ImageStr;

ImageStr* createImageStrFromTgaImageForSpatialFilter(TgaImage* tgaImage, SpatialFilter* spatialFilter);
ImageStr* createImageStr(ImageStr* imageStr);
TgaImage* createTgaImageFromImageStr(ImageStr* imageStr);
void printImageStrDataValues(ImageStr* imageStr);
int applySpatialFilterToImageStrComponentArray(SpatialFilter* spatialFilter, ImageStr* imageStr, ImageStr* processedImageStr, int component);
int applySpatialFilterToSubImageStrComponentArray(SpatialFilter* spatialFilter, ImageStr* imageStr, ImageStr* processedImageStr, ImageStr* subImageStr, int component);
void setComponentValues(ImageStr* imageStr, int dataArrayIndex, int dataArrayValue);
void cleanUpImageStr(ImageStr* imageStr);

#endif /* SRC_IMAGEPROCESSINGUTILS_H_ */
