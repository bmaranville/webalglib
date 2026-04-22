
SRC=alglib_cpp/src
LIBSRC=$(wildcard $(SRC)/*.cpp)
APP=$(wildcard src/*.cpp)

# Object and static library paths
ALGLIB_OBJS=$(patsubst $(SRC)/%.cpp,alglib_cpp/obj/%.o,$(LIBSRC))
ALGLIB_STATIC=alglib_cpp/lib/libalglib.a

WEBFIT=lib/webfit.js
REFL=lib/refl.js
MAGREFL=lib/magrefl.js
REFLFIT=lib/reflfit.js


# Common flags for Emscripten ES6 module output
FLAGS=--bind -sSINGLE_FILE -sEXPORT_ES6=1 -sMODULARIZE=1
EMCC=emcc -O3

all: $(ALGLIB_STATIC) $(WEBFIT) $(REFL) $(MAGREFL) $(REFLFIT)

clean:
	rm -f $(wildcard $(ALGLIB_STATIC)* $(ALGLIB_OBJS)* $(WEBFIT)* $(REFL)* $(MAGREFL)* $(REFLFIT)*)
	rm -rf alglib_cpp/obj/*.o alglib_cpp/lib/libalglib.a


# Compile each ALGLIB source to object file
alglib_cpp/obj/%.o: $(SRC)/%.cpp
	mkdir -p alglib_cpp/obj
	emcc -O3 -I$(SRC) -c $< -o $@

# Archive object files into a static library
$(ALGLIB_STATIC): $(ALGLIB_OBJS)
	mkdir -p alglib_cpp/lib
	emar rcs $@ $^

$(WEBFIT): $(ALGLIB_STATIC) src/webfit.cpp
	mkdir -p lib
	emcc -O3 $(FLAGS) -I$(SRC) $^ -o lib/webfit.js

$(REFL): $(ALGLIB_STATIC) src/reflectivity.cc src/refl_wrap.cc src/reflcalc.h
	mkdir -p lib
	$(EMCC) $(FLAGS) -I$(SRC) $(ALGLIB_STATIC) src/reflectivity.cc src/refl_wrap.cc -o lib/refl.js

$(MAGREFL): $(ALGLIB_STATIC) src/magnetic.cc src/mag_wrap.cc src/reflcalc.h
	mkdir -p lib
	$(EMCC) $(FLAGS) -I$(SRC) $(ALGLIB_STATIC) src/magnetic.cc src/mag_wrap.cc -o lib/magrefl.js

$(REFLFIT): $(ALGLIB_STATIC) src/reflectivity.cc src/magnetic.cc src/reflfit.cpp src/reflcalc.h
	mkdir -p lib
	$(EMCC) $(FLAGS) -I$(SRC) $(ALGLIB_STATIC) src/reflectivity.cc src/magnetic.cc src/reflfit.cpp -o lib/reflfit.js

