ALGLIB=alglib_cpp/lib/alglib.bc
SRC=alglib_cpp/src
LIBSRC=$(wildcard $(SRC)/*.cpp)
APP=$(wildcard src/*.cpp)

WEBFIT=lib/webfit.js
REFL=lib/refl.js
MAGREFL=lib/magrefl.js
REFLFIT=lib/reflfit.js

EMCC=emcc -O3

all: $(ALGLIB) $(WEBFIT) $(REFL) $(MAGREFL) $(REFLFIT)

clean:
	rm -f $(wildcard $(ALGLIB)* $(WEBFIT)* $(REFL)* $(MAGREFL)* $(REFLFIT)*)

$(ALGLIB): $(LIBSRC)
	mkdir -p alglib_cpp/lib
	$(EMCC) -I$(SRC) $(LIBSRC) -o $(ALGLIB)

$(WEBFIT): src/webfit.cpp
	mkdir -p lib
	emcc -O3 --bind -I$(SRC) $(ALGLIB) src/webfit.cpp -o lib/webfit.js

$(REFL): src/reflectivity.cc src/refl_wrap.cc src/reflcalc.h
	mkdir -p lib
	$(EMCC) --bind -I$(SRC) $(ALGLIB) src/reflectivity.cc src/refl_wrap.cc -o lib/refl.js

$(MAGREFL): src/magnetic.cc src/mag_wrap.cc src/reflcalc.h
	mkdir -p lib
	$(EMCC) --bind -I$(SRC) $(ALGLIB) src/magnetic.cc src/mag_wrap.cc -o lib/magrefl.js

$(REFLFIT): src/reflectivity.cc src/magnetic.cc src/reflfit.cpp src/reflcalc.h
	mkdir -p lib
	$(EMCC) --bind -I$(SRC) $(ALGLIB) src/magnetic.cc src/reflectivity.cc src/refl_fit.cpp -o lib/reflfit.js

