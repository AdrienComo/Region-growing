B = bin
O = obj
S = src
FLAGS  = -c -Wall
LIBS   = `pkg-config --libs opencv4`
CFLAGS = `pkg-config --cflags opencv4`

all: $(O) $(B) $(B)/main

$(B)/main: $(O)/Image.o
	g++ $(CFLAGS) $(LIBS) $(S)/main.cpp -o $(B)/main $(O)/*.o -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -Ofast
	
$(O)/Image.o:
	g++ $(FLAGS) $(CFLAGS) $(LIBS) $(S)/Image.cpp -o $(O)/Image.o 
	
$(O):
	mkdir $(O)

$(B):
	mkdir $(B)

clean: $(O) $(B)
	rm -rf $(O)
	rm -rf $(B)
	

