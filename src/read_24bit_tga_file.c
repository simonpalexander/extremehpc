/*
 * read_24bit_tga_file.c
 *
 *  Created on: 24 Apr 2015
 *      Author: simon
 */


/*
 *	Program to read in a 24 bit TGA image file,
 *	and then display it in an appropriately sized 2D OpenGL window.
 *	We know in advance that the image is 128 by 128 pixels and that there
 *	are 8bits for each of the red, green and blue components with no alpha information.
 *	The program confirms, by accessing the header information, the width, height
 *	and bits per pixel.
 *	by
 *	Simon Alexander
 *	January 2000
 */

#include <stdio.h>
#include <stdlib.h>

#include "tgaUtils.h"

void main(int argc, char **argv)
{
	// n6822_big.tga, wood.tga
	char* inputFilename = "resources/n6822_big.tga";
	TgaImage* tgaImage = readTGAFile(inputFilename);

	char* outputFilename = "resources/output.tga";
	saveTGAImage(outputFilename, tgaImage);

	free(tgaImage->image);
	free(tgaImage);
}


