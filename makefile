# Make file
#

all: mainExtremeHPC ompExtremeHPC mpiExtremeHPC  
# read_24bit_tga_file
	 
#read_24bit_tga_file: src/read_24bit_tga_file.c src/tgaUtils.c
#	gcc -o read_24bit_tga_file src/read_24bit_tga_file.c src/tgaUtils.c -I.

mainExtremeHPC: src/mainExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c
	gcc -O3 -o mainExtremeHPC src/mainExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c -I.

ompExtremeHPC: src/ompExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c
	gcc -O3 -fopenmp -o ompExtremeHPC src/ompExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c -I.

mpiExtremeHPC: src/mpiExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c
	mpicc -O3 -fopenmp -o mpiExtremeHPC src/mpiExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c -I.

clean:
	rm mainExtremeHPC.o mainExtremeHPC.exe
	rm ompExtremeHPC.o ompExtremeHPC.exe
	rm mpiExtremeHPC.o mpiExtremeHPC.exe
#	rm read_24bit_tga_file.o read_24bit_tga_file.exe
	 
	 
	 