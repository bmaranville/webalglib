ALGLIB=alglib_cpp/lib/alglib.bc
SRC=alglib_cpp/src
LIBSRC=$(wildcard $(SRC)/*.cpp)
APP=$(wildcard src/*.cpp)

WEBFIT=apps/webfit/app.js
REFL=apps/refl/refl.js
MAGREFL=apps/refl/magrefl.js
REFLFIT=apps/refl/refl_fit.js

EMCC=emcc -O3

all: $(ALGLIB) $(WEBFIT) $(REFL) $(MAGREFL) $(REFLFIT)

clean:
	rm $(ALGLIB) $(WEBFIT) $(REFL) $(MAGREFL) $(REFLFIT)

$(ALGLIB): $(LIBSRC) alglib_cpp/lib
	$(EMCC) -I$(SRC) $(LIBSRC) -o $(ALGLIB)

alglib_cpp/lib:
	mkdir -p alglib_cpp/lib

$(WEBFIT): src/webfit.cpp apps/webfit
	emcc -O3 --bind -I$(SRC) $(ALGLIB) src/webfit.cpp -o apps/webfit/app.js

apps/webfit:
	mkdir -p apps
	mkdir -p apps/webfit

$(REFL): src/reflectivity.cc src/refl_wrap.cc src/reflcalc.h apps/refl/
	$(EMCC) --bind -I$(SRC) $(ALGLIB) src/reflectivity.cc src/refl_wrap.cc -o apps/refl/refl.js

$(MAGREFL): src/magnetic.cc src/mag_wrap.cc src/reflcalc.h apps/refl
	$(EMCC) --bind -I$(SRC) $(ALGLIB) src/magnetic.cc src/mag_wrap.cc -o build/refl/magrefl.js
	
$(REFLFIT): src/reflectivity.cc src/magnetic.cc src/refl_fit.cpp src/reflcalc.h apps/refl build/refl
	$(EMCC) --bind -I$(SRC) $(ALGLIB) src/magnetic.cc src/reflectivity.cc src/refl_fit.cpp -o apps/refl/refl_fit.js

apps/refl:
	mkdir -p apps
	mkdir -p apps/refl
	
build/refl:
	mkdir -p build
	mkdir -p build/refl

lsfit: src/lsfit.cpp apps/lsfit
	emcc -O3 --bind -I$(SRC) $(LIB) src/lsfit.cpp -o apps/lsfit/app.js

apps/lsfit:
	mkdir -p apps
	mkdir -p apps/lsfit
