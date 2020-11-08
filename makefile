IDIR = ./include
ODIR = ./source/obj
LDIR = ./lib
SDIR = ./source

CC = g++
CFLAGS = -g -O0 -std=c++11 -I$(IDIR) -L$(LDIR)

LIBS = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

_DEPS = Game.h TerrainGenerator.h ResourceManager.h Shader.h World.h Chunk.h Chunkmesh.h Face.h Camera.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o Game.o TerrainGenerator.o ResourceManager.o Shader.o World.o Chunk.o Chunkmesh.o Face.o Camera.o glad.o stb_image.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

nickcraft: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
