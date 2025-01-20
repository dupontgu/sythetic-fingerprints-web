CXX = emcc
CXXFLAGS = -O3 \
	-s WASM=1 \
	-s EXPORTED_RUNTIME_METHODS=ccall,cwrap \
	-s EXPORTED_FUNCTIONS=_malloc,_free \
	-s ALLOW_MEMORY_GROWTH=0 \
	-s NO_EXIT_RUNTIME=1 \
	-s "EXPORT_NAME='createFingerprint'" \
	-s ENVIRONMENT=web \
	-s MODULARIZE=1 \
	-s EXPORT_ES6=1 \
	-s USE_ES6_IMPORT_META=0 \
	-s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']" \
	-lembind \
	--bind \
	--no-entry \
	-std=c++11

SOURCES = matrix.cpp imgproc.cpp finge.cpp wasm_wrap.cpp
OBJECTS = $(SOURCES:.cpp=.o)
DIST_DIR = dist
TARGET = $(DIST_DIR)/fingerprint.js

all: $(DIST_DIR) $(TARGET)

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(DIST_DIR) $(OBJECTS)

.PHONY: clean all