cc:=gcc
src:=decode.c
ccflags:=-I./ -O3 -w
exe:=decode_exe
# jpeg_file_name:=./testorig.jpg
# jpeg_file_name:=./images.jpeg
jpeg_file_name:=./Seq41_Ts922699240.jpg

build:
		$(cc) $(src) $(ccflags) -o $(exe)
clean:
		rm -rf $(exe)
run:
		./$(exe) $(jpeg_file_name)