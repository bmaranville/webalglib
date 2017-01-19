LIB=alglib_cpp/lib/alglib.bc
SRC=alglib_cpp/src
LIBSRC=$(wildcard $(SRC)/*.cpp)
APP=$(wildcard src/*.cpp)

all: $(LIB) webfit refl

$(LIB): $(LIBSRC) alglib_cpp/lib
	emcc -O3 -I$(SRC) $(LIBSRC) -o $(LIB)

alglib_cpp/lib:
	mkdir -p alglib_cpp/lib

webfit: src/webfit.cpp apps/webfit
	emcc -O3 --bind -I$(SRC) $(LIB) src/webfit.cpp -o apps/webfit/app.js

apps/webfit:
	mkdir -p apps
	mkdir -p apps/webfit

refl: src/reflectivity.cc src/refl_wrap.cc src/reflcalc.h apps/refl
	emcc -O3 --bind -I$(SRC) $(LIB) src/reflectivity.cc src/refl_wrap.cc -o apps/refl/refl.js

apps/refl:
	mkdir -p apps
	mkdir -p apps/refl

lsfit: src/lsfit.cpp apps/lsfit
	emcc -O3 --bind -I$(SRC) $(LIB) src/lsfit.cpp -o apps/lsfit/app.js

apps/lsfit:
	mkdir -p apps
	mkdir -p apps/lsfit
