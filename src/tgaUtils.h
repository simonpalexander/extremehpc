/*
 * tgaUtils.h
 *
 *  Created on: Apr 24, 2015
 *      Author: simon
 */

#ifndef SRC_TGAUTILS_H_
#define SRC_TGAUTILS_H_

typedef struct tga_image_structure	{
	int width;
	int height;
	int numOfPixels;
	int pixelSize;
	unsigned char header[18];
	int* imageData; } TgaImage;

void tgaImageProperties(TgaImage* tgaImage);
TgaImage* readTGAFile(char* filename);
int saveTGAImage(char* filename, TgaImage* tgaImage);
void cleanUpTgaImage(TgaImage* tgaImage);
int getTgaImageElementValue(TgaImage* tgaImage, int columnIndex, int rowIndex);

void printTgaImageDataValues(TgaImage* tgaImage);

TgaImage* createTgaImage(TgaImage *tgaImage);

#endif /* SRC_TGAUTILS_H_ */
