/*
 * imageProcessingUtils.c
 *
 *  Created on: Apr 26, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>

#include "globalDefines.h"
#include "imageProcessingUtils.h"

ImageStr* createImageStrFromTgaImageForSpatialFilter(TgaImage* tgaImage, SpatialFilter* spatialFilter)
{
	printf("Create image structure from TGA image.\n");
	printf("Row edges extended by %d top and bottom.\n", spatialFilter->requiredImageEdgeExtend);
	printf("Column Edges extended by %d left and right.\n", spatialFilter->requiredImageEdgeExtend);

	ImageStr* imageStr = (ImageStr*) malloc(sizeof(ImageStr));

	imageStr->columnExtend = spatialFilter->requiredImageEdgeExtend;
	imageStr->rowExtend = spatialFilter->requiredImageEdgeExtend;

	imageStr->width = tgaImage->width + (2 * imageStr->columnExtend);
	imageStr->height = tgaImage->height + (2 * imageStr->rowExtend);

	imageStr->redDataArray = (float*) malloc(imageStr->width * imageStr->height * sizeof(float));
	imageStr->greenDataArray = (float*) malloc(imageStr->width * imageStr->height * sizeof(float));
	imageStr->blueDataArray = (float*) malloc(imageStr->width * imageStr->height * sizeof(float));

	int columnIndex, rowIndex;
	int dataArrayIndex;
	int dataArrayValue;

	// Perform in stages.

	// Set Top Left Corner.
	for (rowIndex=0 ; rowIndex<imageStr->rowExtend ; rowIndex++)
	{
		for (columnIndex=0 ; columnIndex<imageStr->columnExtend ; columnIndex++)
		{
			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			dataArrayValue = getTgaImageElementValue(tgaImage, 0, 0);
			setComponentValues(imageStr, dataArrayIndex, dataArrayValue);
		}
	}

	// Set Top Right Corner.
	for (rowIndex=0 ; rowIndex<imageStr->rowExtend ; rowIndex++)
	{
		for (columnIndex=(tgaImage->width + imageStr->columnExtend) ; columnIndex<imageStr->width ; columnIndex++)
		{
			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			dataArrayValue = getTgaImageElementValue(tgaImage, tgaImage->width-1, 0);
			setComponentValues(imageStr, dataArrayIndex, dataArrayValue);
		}
	}

	// Set Bottom Left Corner.
	for (rowIndex=(tgaImage->height + imageStr->rowExtend) ; rowIndex<imageStr->height; rowIndex++)
	{
		for (columnIndex=0 ; columnIndex<imageStr->columnExtend ; columnIndex++)
		{
			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			dataArrayValue = getTgaImageElementValue(tgaImage, 0, tgaImage->height-1);
			setComponentValues(imageStr, dataArrayIndex, dataArrayValue);
		}
	}

	// Set Bottom Right Corner.
	for (rowIndex=(tgaImage->height + imageStr->rowExtend) ; rowIndex<imageStr->height; rowIndex++)
	{
		for (columnIndex=(tgaImage->width + imageStr->columnExtend) ; columnIndex<imageStr->width ; columnIndex++)
		{
			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			dataArrayValue = getTgaImageElementValue(tgaImage, tgaImage->width-1, tgaImage->height-1);
			setComponentValues(imageStr, dataArrayIndex, dataArrayValue);
		}
	}

	// Set Top Edge.
	for (rowIndex=0 ; rowIndex<imageStr->rowExtend ; rowIndex++)
	{
		for (columnIndex=imageStr->columnExtend ; columnIndex<(tgaImage->width + imageStr->columnExtend) ; columnIndex++)
		{
			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			dataArrayValue = getTgaImageElementValue(tgaImage, columnIndex - imageStr->columnExtend, 0);
			setComponentValues(imageStr, dataArrayIndex, dataArrayValue);
		}
	}

	// Set Bottom Edge.
	for (rowIndex=(tgaImage->height + imageStr->rowExtend) ; rowIndex<imageStr->height; rowIndex++)
	{
		for (columnIndex=imageStr->columnExtend ; columnIndex<(tgaImage->width + imageStr->columnExtend) ; columnIndex++)
		{
			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			dataArrayValue = getTgaImageElementValue(tgaImage, columnIndex - imageStr->columnExtend, tgaImage->height - 1);
			setComponentValues(imageStr, dataArrayIndex, dataArrayValue);
		}
	}

	// Set Left Edge.
	for (rowIndex=imageStr->rowExtend ; rowIndex<(tgaImage->height + imageStr->rowExtend) ; rowIndex++)
	{
		for (columnIndex=0 ; columnIndex<imageStr->columnExtend ; columnIndex++)
		{
			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			dataArrayValue = getTgaImageElementValue(tgaImage, 0, rowIndex - imageStr->rowExtend);
			setComponentValues(imageStr, dataArrayIndex, dataArrayValue);
		}
	}

	// Set Right Edge.
	for (rowIndex=imageStr->rowExtend ; rowIndex<(tgaImage->height + imageStr->rowExtend) ; rowIndex++)
	{
		for (columnIndex=(tgaImage->width + imageStr->columnExtend) ; columnIndex<imageStr->width ; columnIndex++)
		{
			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			dataArrayValue = getTgaImageElementValue(tgaImage, tgaImage->width - 1, rowIndex - imageStr->rowExtend);
			setComponentValues(imageStr, dataArrayIndex, dataArrayValue);
		}
	}

	// Copy original tga image data.
	for (rowIndex=imageStr->rowExtend ; rowIndex<(tgaImage->height + imageStr->rowExtend) ; rowIndex++)
	{
		for (columnIndex=imageStr->columnExtend ; columnIndex<(tgaImage->width + imageStr->columnExtend) ; columnIndex++)
		{
			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			dataArrayValue = getTgaImageElementValue(tgaImage, columnIndex - imageStr->columnExtend, rowIndex - imageStr->rowExtend);
			setComponentValues(imageStr, dataArrayIndex, dataArrayValue);
		}
	}

	printf("Created image structure.\n");

	return imageStr;
}

//-----------------------------------------------------------------------------

void setComponentValues(ImageStr* imageStr, int dataArrayIndex, int dataArrayValue)
{
	float redValue = (float)(dataArrayValue & 0xFF) / 255.0f;
	float greenValue = (float)((dataArrayValue >> 8) & 0xFF) / 255.0f;
	float blueValue = (float)((dataArrayValue >> 16) & 0xFF) / 255.0f;

	*(imageStr->redDataArray + dataArrayIndex) = redValue;
	*(imageStr->greenDataArray + dataArrayIndex) = greenValue;
	*(imageStr->blueDataArray + dataArrayIndex) = blueValue;
}

//-----------------------------------------------------------------------------

ImageStr* createImageStr(ImageStr* imageStr)
{
	printf("Create image structure from image structure.\n");

	ImageStr* newImageStr = (ImageStr*) malloc(sizeof(ImageStr));

	newImageStr->width = imageStr->width;
	newImageStr->height = imageStr->height;

	newImageStr->columnExtend = imageStr->columnExtend;
	newImageStr->rowExtend = imageStr->rowExtend;

	newImageStr->redDataArray = (float*) malloc(newImageStr->width * newImageStr->height * sizeof(float));
	newImageStr->greenDataArray = (float*) malloc(newImageStr->width * newImageStr->height * sizeof(float));
	newImageStr->blueDataArray = (float*) malloc(newImageStr->width * newImageStr->height * sizeof(float));

	return newImageStr;
}

//-----------------------------------------------------------------------------

void printImageStrDataValues(ImageStr* imageStr)
{
	printf("Print image array data values.\n");
	int columnIndex, rowIndex;
	float* redIndexPtr = imageStr->redDataArray;
	float* greenIndexPtr = imageStr->greenDataArray;
	float* blueIndexPtr = imageStr->blueDataArray;

	for (rowIndex = 0 ; rowIndex < imageStr->height ; rowIndex++)
	{
		for (columnIndex = 0 ; columnIndex < imageStr->width ; columnIndex++)
		{
			printf("(%f, %f, %f) ", *redIndexPtr, *greenIndexPtr, *blueIndexPtr);
			redIndexPtr++;
			greenIndexPtr++;
			blueIndexPtr++;
		}
		printf("\n");
	}
}

//-----------------------------------------------------------------------------

int applySpatialFilterToImageStrComponentArray(SpatialFilter* spatialFilter, ImageStr* imageStr, ImageStr* processedImageStr, int component)
{
	printf("Applying spatial filter to image array. Component %d\n", component);

	int rowIndex, columnIndex;
	int dataArrayIndex;

	int rowEndIndex = imageStr->height - imageStr->rowExtend;
	int columnEndIndex = imageStr->width - imageStr->columnExtend;

	int filterRange = spatialFilter->requiredImageEdgeExtend;
	int filterColumnIndex, filterRowIndex;
	float* filterElementPtr;
	float elementValue;

	float* arrayPtr;
	float* processedArrayPtr;
	if (isRed(component))
	{
		arrayPtr = imageStr->redDataArray;
		processedArrayPtr = processedImageStr->redDataArray;
	}
	else if (isGreen(component))
	{
		arrayPtr = imageStr->greenDataArray;
		processedArrayPtr = processedImageStr->greenDataArray;
	}
	else if (isBlue(component))
	{
		arrayPtr = imageStr->blueDataArray;
		processedArrayPtr = processedImageStr->blueDataArray;
	}

	float componentSum = 0.0f;

	for (rowIndex=imageStr->rowExtend ; rowIndex<rowEndIndex ; rowIndex++)
	{
		for (columnIndex=imageStr->columnExtend ; columnIndex<columnEndIndex ; columnIndex++)
		{
			filterElementPtr = spatialFilter->filter;
			componentSum = 0.0f;

			for (filterRowIndex=-filterRange ; filterRowIndex<=filterRange ; filterRowIndex++)
			{
				for (filterColumnIndex=-filterRange ; filterColumnIndex<=filterRange ; filterColumnIndex++)
				{
					dataArrayIndex = (rowIndex + filterRowIndex) * imageStr->width + (columnIndex + filterColumnIndex);
					// Spatial Filter element: (filterColumnIndex, filterRowIndex)
					elementValue = *(arrayPtr + dataArrayIndex);
					componentSum += (*filterElementPtr) * elementValue;
					filterElementPtr++;
				}
			}

			dataArrayIndex = rowIndex * imageStr->width + columnIndex;
			*(processedArrayPtr + dataArrayIndex) = spatialFilter->scalar * componentSum;
		}
	}

	printf("Spatial filter applied.\n");
	return SUCCESS;
}

//-----------------------------------------------------------------------------

TgaImage* createTgaImageFromImageStr(ImageStr* imageStr, TgaImage* tgaImage)
{
	printf("Creating TgaImage from ImageStr\n");

	TgaImage* newTgaImage = createTgaImage(tgaImage);

	int rowIndex, columnIndex;
	int dataArrayIndex;

	int rowEndIndex = imageStr->height - imageStr->rowExtend;
	int columnEndIndex = imageStr->width - imageStr->columnExtend;

	int redComponent, greenComponent, blueComponent;
	float redValue, greenValue, blueValue;

	for (rowIndex=imageStr->rowExtend ; rowIndex<rowEndIndex ; rowIndex++)
	{
		for (columnIndex=imageStr->columnExtend ; columnIndex<columnEndIndex ; columnIndex++)
		{
			dataArrayIndex = (rowIndex * imageStr->width) + columnIndex;

			redValue = *(imageStr->redDataArray + dataArrayIndex);
			redComponent = (int) (redValue * 255);
			if (redComponent<0)
			{
				redComponent = 0;
			}
			if (redComponent>255)
			{
				redComponent = 255;
			}

			greenValue = *(imageStr->greenDataArray + dataArrayIndex);
			greenComponent = (int) (greenValue * 255);
			if (greenComponent<0)
			{
				greenComponent = 0;
			}
			if (greenComponent>255)
			{
				greenComponent = 255;
			}

			blueValue = *(imageStr->blueDataArray + dataArrayIndex);
			blueComponent = (int) (blueValue * 255);
			if (blueComponent<0)
			{
				blueComponent = 0;
			}
			if (blueComponent>255)
			{
				blueComponent = 255;
			}

			dataArrayIndex = ((rowIndex - imageStr->rowExtend) * tgaImage->width) + (columnIndex - imageStr->columnExtend);
			*(newTgaImage->imageData + dataArrayIndex) = ((int) redComponent) + ((int)(greenComponent) << 8) + ((int)(blueComponent) << 16);
		}
	}

	printf("Created TgaImage.\n");

	return newTgaImage;
}

//-----------------------------------------------------------------------------

