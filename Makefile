MACHINE= $(shell uname -s)
SRC=src/geom/ball.cpp\
    src/geom/vec.cpp\
    src/geom/mat.cpp\
    src/math/random.cpp\
    src/common/char.cpp\
    src/exception/exception.cpp\
    src/common/string.cpp\
    src/common/file.cpp\
    src/draw/image.cpp\
    src/draw/tga.cpp\
    src/draw/sdlWindow.cpp\
    src/draw/gl/texture.cpp\
    src/draw/gl/framebuffer.cpp\
    src/draw/gl/program.cpp\
    src/draw/gl/mesh.cpp\
    src/draw/gl/mdl.cpp\
    src/draw/gl/drawDevice.cpp\
    src/draw/scene.cpp\
    src/entity/entity.cpp\
    src/ring_automaton.cpp\
    src/object.cpp\
	src/main.cpp

ifeq ($(MACHINE),Darwin)
CMD=clang++
CXXFLAGS=-Isrc -g -O0 --std=c++11 `sdl-config --cflags`
LDFLAGS=-framework OpenGL \
		-framework SDL2_mixer -framework SDL2_image -framework SDL2
else
CMD=g++
CXXFLAGS=-Isrc -O0 -fPIC --std=c++11 -Werror
LDFLAGS=-shared -lGL -lSDL -lGLEW -lSDL_mixer
endif
OUT=ggj2016
all:
	$(CMD) $(SRC) $(CXXFLAGS) $(LDFLAGS) 

web:
	alias python2="python2.7"
	mkdir -p bin/web/
	em++ $(SRC) -Isrc --std=c++11 -o bin/web/index.html \
		-s USE_SDL=2 -s USE_SDL_MIXER=2 \
		-s USE_OGG=1 \
		-s TOTAL_MEMORY=67108864 \
		-s WASM=1 \
		--preload-file res
