LIB=alglib_cpp/lib/alglib.bc
SRC=alglib_cpp/src
LIBSRC=$(wildcard $(SRC)/*.cpp)
APP=$(wildcard src/*.cpp)

all: $(LIB) webfit

$(LIB): $(LIBSRC) alglib_cpp/lib
	emcc -O2 -I$(SRC) $(LIBSRC) -o $(LIB)

alglib_cpp/lib:
	mkdir -p alglib_cpp/lib

webfit: src/webfit.cpp apps/webfit
	emcc -O2 --bind -I$(SRC) $(LIB) src/webfit.cpp -o apps/webfit/app.js

apps/webfit:
	mkdir -p apps
	mkdir -p apps/webfit

lsfit: src/lsfit.cpp apps/lsfit
	emcc -O2 --bind -I$(SRC) $(LIB) src/lsfit.cpp -o apps/lsfit/app.js

apps/lsfit:
	mkdir -p apps
	mkdir -p apps/lsfit
