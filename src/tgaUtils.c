/*
 * tgaUtils.c
 *
 *  Created on: Apr 24, 2015
 *      Author: simon
 */

#include <stdio.h>
#include <stdlib.h>

#include "tgaUtils.h"

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
	printf("Image Identification Field: %d\n", tgaImage->header[18]);

	printf("General:\n");
	printf("\tWidth: %d\n", tgaImage->width);
	printf("\tHeight: %d:\n", tgaImage->height);
	printf("\tNum of Pixels: %d:\n", tgaImage->numOfPixels);
	printf("\tBytes per Pixel: %d\n", tgaImage->pixelSize);
	printf("- - - - - - - - - - - - - - - - - - -\n");
}

/*---------------------------------------------------------------------------*/

TgaImage* readTGAFile(char* filename)
{
	printf("\nOpening TGA image file: %s\n", filename);

	TgaImage* tgaImage = (TgaImage*) malloc(sizeof(TgaImage));

	FILE *data = fopen(filename, "rb");

	/*
	 * Read the first 18 bytes of the file.
	 * This contains the header information.
	 */
	fread(tgaImage->header, 18, 1, data);

	tgaImage->width = (int) ((tgaImage->header[13] << 8) + tgaImage->header[12]);
	tgaImage->height = (int) ((tgaImage->header[15] << 8) + tgaImage->header[14]);
	tgaImage->numOfPixels = tgaImage->width * tgaImage->height;
	tgaImage->pixelSize = tgaImage->header[16]/8;

	tgaImageProperties(tgaImage);

	int totalNumberOfBytes = tgaImage->pixelSize * tgaImage->numOfPixels;
	printf("Total number of Bytes: %d\n", totalNumberOfBytes);

	tgaImage->image = (unsigned char *) malloc(totalNumberOfBytes * sizeof(unsigned char));
	fread(tgaImage->image, totalNumberOfBytes, 1, data);

	fclose(data);
	printf("\nTGA image read.\n");
	return(tgaImage);
}

/*---------------------------------------------------------------------------*/

void saveTGAImage(char* filename, TgaImage* tgaImage)
{
	printf("\nSaving TGA image file: %s\n", filename);

	FILE *data;

	data = fopen(filename, "wb");
	fwrite(tgaImage->header, 18, 1, data);

	int totalNumberOfBytes = tgaImage->pixelSize * tgaImage->numOfPixels;
	fwrite(tgaImage->image, totalNumberOfBytes, 1, data);

	fclose(data);
	printf("TGA image written.\n");
}

/*---------------------------------------------------------------------------*/

void unpackImageData(TgaImage* tgaImage)
{
	printf("\nUnpacking Image Data.\n");

	tgaImage->unpackedImage = (int*) malloc(tgaImage->numOfPixels * sizeof(int));

	unsigned char* imagePixelPtr;
	int* unpackedImagePixelPtr;

	unsigned char redComponent, greenComponent, blueComponent;
	int unpackedPixel;

	imagePixelPtr = tgaImage->image;
	unpackedImagePixelPtr = tgaImage->unpackedImage;

	int pixelIdx;
	for (pixelIdx = 0 ; pixelIdx < tgaImage->numOfPixels ; pixelIdx++) {

		// Remember TGA pixels pack colour components
		// in the format BGR.
		blueComponent = *imagePixelPtr;
		imagePixelPtr++;
		greenComponent = *imagePixelPtr;
		imagePixelPtr++;
		redComponent = *imagePixelPtr;
		imagePixelPtr++;

		// Pack in format RGBA
		unpackedPixel = ((int) redComponent) + ((int)(greenComponent) << 8) + ((int)(blueComponent) << 16);
		*unpackedImagePixelPtr = unpackedPixel;

		unpackedImagePixelPtr++;
	}

	free(tgaImage->image);

	printf("\nUnpacked Image Data.\n");
}

/*---------------------------------------------------------------------------*/

void packImageData(TgaImage* tgaImage)
{
	printf("\nPacking Image Data.\n");

	tgaImage->image = (unsigned char*) malloc(tgaImage->pixelSize * tgaImage->numOfPixels * sizeof(unsigned char));

	unsigned char* imagePixelPtr;
	int* unpackedImagePixelPtr;

	unsigned char redComponent, greenComponent, blueComponent;

	imagePixelPtr = tgaImage->image;
	unpackedImagePixelPtr = tgaImage->unpackedImage;

	int pixelIdx;
	for (pixelIdx = 0 ; pixelIdx < tgaImage->numOfPixels ; pixelIdx++) {

		blueComponent = (unsigned char) (*unpackedImagePixelPtr >> 16);
		*imagePixelPtr = blueComponent;
		imagePixelPtr++;

		greenComponent = (unsigned char) (*unpackedImagePixelPtr >> 8);
		*imagePixelPtr = greenComponent;
		imagePixelPtr++;

		redComponent = (unsigned char) (*unpackedImagePixelPtr);
		*imagePixelPtr = redComponent;
		imagePixelPtr++;

		unpackedImagePixelPtr++;
	}

	free(tgaImage->unpackedImage);

	printf("\nPacked Image Data.\n");
}



