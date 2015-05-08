# Make file
#

all: serialExtremeHPC ompExtremeHPC mpiExtremeHPC  

serialExtremeHPC: src/mainExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c
	gcc -O3 -o serialExtremeHPC src/mainExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c -I.

ompExtremeHPC: src/ompExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c
	gcc -O3 -fopenmp -o ompExtremeHPC src/ompExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c -I.

mpiExtremeHPC: src/mpiExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c src/mpiUtils.c
	mpicc -O3 -fopenmp -o mpiExtremeHPC src/mpiExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c  src/mpiUtils.c -I.

clean:
	rm serialExtremeHPC.o serialExtremeHPC.exe
	rm ompExtremeHPC.o ompExtremeHPC.exe
	rm mpiExtremeHPC.o mpiExtremeHPC.exe
	 	 