# Make file
#

all: serialExtremeHPC ompExtremeHPC mpiExtremeHPC serialExtremeHPCMultiple ompExtremeHPCMultiple mpiExtremeHPCMultiple

serialExtremeHPC: src/serialExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c
	gcc -O3 -o serialExtremeHPC src/serialExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c -I.

ompExtremeHPC: src/ompExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c
	gcc -O3 -fopenmp -o ompExtremeHPC src/ompExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c -I.

mpiExtremeHPC: src/mpiExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c src/mpiUtils.c
	mpicc -O3 -fopenmp -o mpiExtremeHPC src/mpiExtremeHPC.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c  src/mpiUtils.c -I.


serialExtremeHPCMultiple: src/serialExtremeHPCMultiple.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c
	gcc -O3 -o serialExtremeHPCMultiple src/serialExtremeHPCMultiple.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c -I.

ompExtremeHPCMultiple: src/ompExtremeHPCMultiple.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c
	gcc -O3 -fopenmp -o ompExtremeHPCMultiple src/ompExtremeHPCMultiple.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c -I.

mpiExtremeHPCMultiple: src/mpiExtremeHPCMultiple.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c src/imageProcessingUtils.c src/timingUtils.c src/mpiUtils.c
	mpicc -O3 -fopenmp -o mpiExtremeHPCMultiple src/mpiExtremeHPCMultiple.c src/tgaUtils.c src/spatialFilterUtils.c src/proprtiesFileUtils.c  src/imageProcessingUtils.c src/timingUtils.c  src/mpiUtils.c -I.

clean:
	rm serialExtremeHPC.o serialExtremeHPC.exe
	rm ompExtremeHPC.o ompExtremeHPC.exe
	rm mpiExtremeHPC.o mpiExtremeHPC.exe
	rm serialExtremeHPCMultiple.o serialExtremeHPCMultiple.exe
	rm ompExtremeHPCMultiple.o ompExtremeHPCMultiple.exe
	rm mpiExtremeHPCMultiple.o mpiExtremeHPCMultiple.exe
	
	 	 