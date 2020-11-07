PLATFORM = SDL

BINDIR = bin
OBJDIR = obj

INCDIRS = inc example/inc
LIBDIRS = $(OBJDIR)
LIBS    = GL dl stdc++fs

COMMONFLAGS = -Wall -Werror -Wfatal-errors -DLAK_USE_$(PLATFORM) $(foreach D,$(INCDIRS),-I$D ) $(foreach D,$(LIBDIRS),-L$D ) $(foreach L,$(LIBS),-l$L )

CXX = g++
CC  = gcc

CXXFLAGS = -g3 -O0 -mfpmath=387 -mtune=generic -no-pie -mavx -pthread
CCFLAGS  = -g3 -O0 -mfpmath=387 -mtune=generic -no-pie -mavx -pthread

CXXFLAGS += -std=c++17 $(COMMONFLAGS)
CCFLAGS  += -std=c99 $(COMMONFLAGS)

ifeq ($(PLATFORM),SDL)
CXXFLAGS += `sdl2-config --cflags`
LIBS += SDL2
else ifeq ($(PLATFORM),WIN32)
else ifeq ($(PLATFORM),XCB)
else ifeq ($(PLATFORM),XLIB)
else
$(error Invalid platform "$(PLATFORM)")
endif

array_example: $(BINDIR)/array_example.elf

tokeniser_example: $(BINDIR)/tokeniser_example.elf

trie_example: $(BINDIR)/trie_example.elf

type_pack_example: $(BINDIR)/type_pack_example.elf

SOURCES = \
	src/opengl/gl3w.c \
	src/opengl/mesh.cpp \
	src/opengl/shader.cpp \
	src/opengl/texture.cpp \
	src/debug.cpp \
	src/file.cpp \
	src/intrin.cpp \
	src/memmanip.cpp \
	src/memory.cpp \
	src/profile.cpp \
	src/strconv.cpp \
	src/tinflate.cpp \
	src/tokeniser.cpp \
	src/unicode.cpp
	# src/platform.cpp
	# src/events.cpp
	# src/window.cpp
HEADERS = \
	inc/GL/gl3w.h \
	inc/GL/glcorearb.h \
	inc/lak/opengl/mesh.hpp \
	inc/lak/opengl/shader.hpp \
	inc/lak/opengl/state.hpp \
	inc/lak/opengl/texture.hpp \
	inc/lak/array.hpp \
	inc/lak/array.inl \
	inc/lak/bank_ptr.hpp \
	inc/lak/bank_ptr.inl \
	inc/lak/buffer.hpp \
	inc/lak/char.hpp \
	inc/lak/compiler.hpp \
	inc/lak/debug.hpp \
	inc/lak/debug.inl \
	inc/lak/defer.hpp \
	inc/lak/endian.hpp \
	inc/lak/events.hpp \
	inc/lak/file.hpp \
	inc/lak/image.hpp \
	inc/lak/intrin.hpp \
	inc/lak/macro_utils.hpp \
	inc/lak/memmanip.hpp \
	inc/lak/memory.hpp \
	inc/lak/optional.hpp \
	inc/lak/os.hpp \
	inc/lak/platform.hpp \
	inc/lak/profile.hpp \
	inc/lak/span.hpp \
	inc/lak/span.inl \
	inc/lak/stdint.hpp \
	inc/lak/strcast.hpp \
	inc/lak/strcast.inl \
	inc/lak/strconv.hpp \
	inc/lak/strconv.inl \
	inc/lak/string.hpp \
	inc/lak/tinflate.hpp \
	inc/lak/tokeniser.hpp \
	inc/lak/tokeniser.inl \
	inc/lak/trie.hpp \
	inc/lak/trie.inl \
	inc/lak/type_pack.hpp \
	inc/lak/type_utils.hpp \
	inc/lak/unicode.hpp \
	inc/lak/unicode.inl \
	inc/lak/vec_intrin.hpp \
	inc/lak/vec.hpp \
	inc/lak/window.hpp \
	Makefile

OBJECTS = $(SOURCES:src/%=obj/%.$(PLATFORM).o)
.PRECIOUS: $(OBJECTS)

# directories that need to be created in order for compiliation to work.
BLDDIRS = $(BINDIR) $(OBJDIR) $(OBJDIR)/opengl $(OBJDIR)/sdl $(OBJDIR)/win32 $(OBJDIR)/xcb $(OBJDIR)/xlib

$(BINDIR)/%.elf: example/%.cpp $(OBJECTS) | $(BLDDIRS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(OBJDIR)/%.cpp.$(PLATFORM).o: src/%.cpp $(HEADERS) | $(BLDDIRS)
	$(CXX) -o $@ -c $< $(CXXFLAGS)

$(OBJDIR)/%.c.$(PLATFORM).o: src/%.c $(HEADERS) | $(BLDDIRS)
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	rm -rf $(BLDDIRS)

$(BLDDIRS):
	mkdir $@
