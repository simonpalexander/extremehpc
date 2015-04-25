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
	int pixelSize;
	unsigned char header[18];
	unsigned char *image; } TgaImage;

void tgaImageProperties(TgaImage* tgaImage);
TgaImage* readTGAFile(char* filename);
void saveTGAImage(char* filename, TgaImage* tgaImage);

#endif /* SRC_TGAUTILS_H_ */
