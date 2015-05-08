/*
 * tgaUtils.c
 *
 *  Created on: Apr 24, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>

#include "globalDefines.h"
#include "tgaUtils.h"

extern int isLog;

void tgaImageProperties(TgaImage* tgaImage)
{
	printf("- - - - - - - - - - - - - - - - - - -\n");
	printf("TGA Image Properties:\n");
	printf("Header:\n");
	printf("Number of Characters in Identification Field: %d\n", tgaImage->header[0]);
	printf("Colour Map Type: %d\n", tgaImage->header[1]);
	printf("Image Type Code: %d\n", tgaImage->header[2]);

	printf("Colour Map Specification:\n");
	int tmp = (int) ((tgaImage->header[4] << 8) + tgaImage->header[3]);
	printf("\tColour Map Origin: %d\n", tmp);
	tmp = (int) ((tgaImage->header[6] << 8) + tgaImage->header[5]);
	printf("\tColour Map Length: %d\n", tmp);
	printf("\tColour Map Entry Size: %d\n", tgaImage->header[7]);

	printf("Image Specification:\n");
	tmp = (int) ((tgaImage->header[9] << 8) + tgaImage->header[8]);
	printf("\tX Origin of Image: %d\n", tmp);
	tmp = (int) ((tgaImage->header[11] << 8) + tgaImage->header[10]);
	printf("\tY Origin of Image: %d\n", tmp);
	tmp = (int) ((tgaImage->header[13] << 8) + tgaImage->header[12]);
	printf("\tWidth of Image: %d : (lo-hi) %d %d\n", tmp, tgaImage->header[12],tgaImage->header[13]);
	tmp = (int) ((tgaImage->header[15] << 8) + tgaImage->header[14]);
	printf("\tHeight of Image: %d : (lo-hi) %d %d\n", tmp, tgaImage->header[14],tgaImage->header[15]);

	printf("Image Pixel Size: %d\n", tgaImage->header[16]);
	printf("Image Descriptor Byte: %d\n", tgaImage->header[17]);
	//printf("Image Identification Field: %d\n", tgaImage->header[18]);

	printf("General:\n");
	printf("\tWidth: %d\n", tgaImage->width);
	printf("\tHeight: %d:\n", tgaImage->height);
	printf("\tNum of Pixels: %d:\n", tgaImage->numOfPixels);
	printf("\tBytes per Pixel: %d\n", tgaImage->pixelSize);
	printf("- - - - - - - - - - - - - - - - - - -\n");
}

//-----------------------------------------------------------------------------

TgaImage* readTGAFile(char* filename)
{
	if (isLog) { printf("\nOpening TGA image file: %s\n", filename); }

	TgaImage* tgaImage = (TgaImage*) malloc(sizeof(TgaImage));

	FILE *filePtr = fopen(filename, "rb");
	if (!filePtr)
	{
		printf("ERROR: Problem opening TGA image file to read: %s\n", filename);
		return NULL;
	}

	/*
	 * Read the first 18 bytes of the file.
	 * This contains the header information.
	 */
	fread(tgaImage->header, 18, 1, filePtr);

	tgaImage->width = (int) ((tgaImage->header[13] << 8) + tgaImage->header[12]);
	tgaImage->height = (int) ((tgaImage->header[15] << 8) + tgaImage->header[14]);
	tgaImage->numOfPixels = tgaImage->width * tgaImage->height;
	tgaImage->pixelSize = tgaImage->header[16]/8;

	tgaImageProperties(tgaImage);

	int totalNumberOfBytes = tgaImage->pixelSize * tgaImage->numOfPixels;
	if (isLog) { printf("Total number of Bytes: %d\n", totalNumberOfBytes); }

	if (isLog) { printf("Reading TGA image data.\n"); }
	unsigned char* imageDataBuffer = (unsigned char*) malloc(totalNumberOfBytes * sizeof(unsigned char));
	fread(imageDataBuffer, totalNumberOfBytes, 1, filePtr);
	if (isLog) { printf("TGA image data read.\n"); }

	fclose(filePtr);

	if (isLog) { printf("Unpacking Image Data.\n"); }

	tgaImage->imageData = (int*) malloc(tgaImage->numOfPixels * sizeof(int));

	unsigned char* imageDataBufferPtr = imageDataBuffer;
	int* imageDataPixelPtr = tgaImage->imageData;;

	unsigned char redComponent, greenComponent, blueComponent;

	int pixelIndex;
	for (pixelIndex = 0 ; pixelIndex < tgaImage->numOfPixels ; pixelIndex++) {
		// Remember TGA pixels pack colour components in the format BGR.
		blueComponent = *imageDataBufferPtr;
		imageDataBufferPtr++;
		greenComponent = *imageDataBufferPtr;
		imageDataBufferPtr++;
		redComponent = *imageDataBufferPtr;
		imageDataBufferPtr++;

		// Pack in format RGBA
		*imageDataPixelPtr = ((int) redComponent) + ((int)(greenComponent) << 8) + ((int)(blueComponent) << 16);
		imageDataPixelPtr++;
	}
	if (isLog) { printf("Unpacked Image Data.\n"); }

	free(imageDataBuffer);

	if (isLog) { printf("TFA image file read.\n"); }
	return(tgaImage);
}

//-----------------------------------------------------------------------------

int saveTGAImage(char* filename, TgaImage* tgaImage)
{
	printf("Saving TGA image file: %s\n", filename);

	printf("General:\n");
	printf("\tWidth: %d\n", tgaImage->width);
	printf("\tHeight: %d:\n", tgaImage->height);
	printf("\tNum of Pixels: %d:\n", tgaImage->numOfPixels);
	printf("\tBytes per Pixel: %d\n", tgaImage->pixelSize);

	if (isLog) { printf("Defining header info:\n"); }

	//- - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Assuming TGA Image DATA TYPE 2: Unmapped RGB images.

	// Number of Characters in Identification Field.
	// A value of 0 means that no Image.
	tgaImage->header[0] = 0;

	// Color Map Type.
	// 0 means no color map  is included.
	tgaImage->header[1] = 0;
	tgaImage->header[2] = 2;

	tgaImage->header[12] = tgaImage->width;
	tgaImage->header[13] = tgaImage->width >> 8;
	tgaImage->header[14] = tgaImage->height;
	tgaImage->header[15] = tgaImage->height >> 8;

	// Image Pixel Size.
	// Number of bits in a pixel. This is 24 for Targa 24.
	tgaImage->header[16] = tgaImage->pixelSize * 8;

	if (isLog)
	{
		printf("Number of Characters in Identification Field: %d\n", tgaImage->header[0]);
		printf("Colour Map Type: %d\n", tgaImage->header[1]);
		printf("Image Type Code: %d\n", tgaImage->header[2]);
		printf("Image Specification:\n");
		printf("\tWidth of Image: (lo-hi) %d %d\n", tgaImage->header[12],tgaImage->header[13]);
		printf("\tHeight of Image: (lo-hi) %d %d\n", tgaImage->header[14],tgaImage->header[15]);
		printf("Image Pixel Size: %d\n", tgaImage->header[16]);
	}

	if (isLog) { printf("Packing Image Data.\n"); }

	int totalNumberOfBytes = tgaImage->pixelSize * tgaImage->numOfPixels;
	unsigned char* imageDataBuffer = (unsigned char*) malloc(totalNumberOfBytes * sizeof(unsigned char));

	unsigned char* imageDataBufferPtr = imageDataBuffer;
	int* imageDataPixelPtr = tgaImage->imageData;;

	int pixelIndex;
	for (pixelIndex = 0 ; pixelIndex < tgaImage->numOfPixels ; pixelIndex++)
	{
		// Blue Component
		*imageDataBufferPtr = (unsigned char) (*imageDataPixelPtr >> 16);
		imageDataBufferPtr++;

		// Green Component
		*imageDataBufferPtr = (unsigned char) (*imageDataPixelPtr >> 8);
		imageDataBufferPtr++;

		// Red Component
		*imageDataBufferPtr = (unsigned char) (*imageDataPixelPtr);
		imageDataBufferPtr++;

		imageDataPixelPtr++;
	}

	if (isLog) { printf("Packed Image Data.\n"); }

	FILE *filePtr = fopen(filename, "wb");
	if (!filePtr)
	{
		printf("ERROR: Problem opening TGA image file to write: %s\n", filename);
		return FAIL;
	}

	fwrite(tgaImage->header, 18, 1, filePtr);
	fwrite(imageDataBuffer, totalNumberOfBytes, 1, filePtr);
	fclose(filePtr);

	free(imageDataBuffer);

	printf("TGA image written.\n");
	return SUCCESS;
}

//-----------------------------------------------------------------------------

void cleanUpTgaImage(TgaImage* tgaImage)
{
	free(tgaImage->imageData);
	free(tgaImage);
}

//-----------------------------------------------------------------------------

// Remember index will be from:
// Column: 0 to (column-1)
// Row: 0 to (width-1)
int getTgaImageElementValue(TgaImage* tgaImage, int columnIndex, int rowIndex)
{
	int elementIndex = rowIndex * tgaImage->width + columnIndex;

	return *(tgaImage->imageData + elementIndex);
}

//-----------------------------------------------------------------------------

void printTgaImageDataValues(TgaImage* tgaImage)
{
	printf("Print tga image data values.\n");

	int columnIndex, rowIndex;
	int* elementIndexPtr = tgaImage->imageData;
	for (rowIndex = 0 ; rowIndex < tgaImage->height ; rowIndex++)
	{
		for (columnIndex = 0 ; columnIndex < tgaImage->width ; columnIndex++)
		{
			printf("%d  ", *elementIndexPtr);
			elementIndexPtr++;
		}
		printf("\n");
	}
}

//-----------------------------------------------------------------------------

TgaImage* createTgaImage(int width, int height, int pixelSize)
{
	TgaImage* tgaImage = (TgaImage*) malloc(sizeof(TgaImage));

	tgaImage->width = width;
	tgaImage->height = height;
	tgaImage->numOfPixels = width * height;
	tgaImage->pixelSize = pixelSize;

	int index;
	for (index=0 ; index<18 ; index++)
	{
		tgaImage->header[index] = 0;
	}

	tgaImage->imageData = (int*) malloc(tgaImage->numOfPixels * sizeof(int));

	return tgaImage;
}
