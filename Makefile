ALGLIB_URL="https://www.alglib.net/translator/re/alglib-3.17.0.cpp.gpl.zip"
ALGLIB=alglib/cpp/lib/alglib.bc
SRC=alglib/cpp/src
APP=$(wildcard src/*.cpp)

WEBFIT=lib/webfit.*
REFL=lib/refl.*
MAGREFL=lib/magrefl.*
REFLFIT=lib/reflfit.*

EMCC=emcc -O3

all: $(ALGLIB) $(WEBFIT) $(REFL) $(MAGREFL) $(REFLFIT)

clean:
	rm -f $(wildcard $(ALGLIB)* $(WEBFIT)* $(REFL)* $(MAGREFL)* $(REFLFIT)*)

$(ALGLIB): $(LIBSRC)
	curl $(ALGLIB_URL) > alglib.zip
	unzip -o alglib.zip -d alglib
	mkdir -p alglib_cpp/lib
	$(EMCC) -I$(SRC)  $(SRC)/*.cpp -r -o $(ALGLIB)

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
	$(EMCC) --bind -I$(SRC) $(ALGLIB) src/magnetic.cc src/reflectivity.cc src/reflfit.cpp -o lib/reflfit.js

