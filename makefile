# Make file
#

all: read_24bit_tga_file
	 
read_24bit_tga_file: src/read_24bit_tga_file.c src/tgaUtils.c
	gcc -o read_24bit_tga_file src/read_24bit_tga_file.c src/tgaUtils.c -I.
	 
#extremehpc-hw6: src/extremehpc-hw6.c src/dummyFunctions.c
#	gcc -fopenmp -o extremehpc-hw6 src/extremehpc-hw6.c src/dummyFunctions.c -I.
		 
clean:
	 rm read_24bit_tga_file.o read_24bit_tga_file.exe
	 
	 