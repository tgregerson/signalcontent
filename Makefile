OBJDIR = build_files

VPATH = src/base:src/codec:src/parser:src/scripts

CXXFLAGS = -O3 -g -Wall -fmessage-length=0 -std=c++0x -flto
CXX = clang++

LDFLAGS = -flto

ALL_OBJS = $(addprefix $(OBJDIR)/,generate_epims.o huffman.o lzw.o)

CODEC_O = $(addprefix $(OBJDIR)/,huffman.o lzw.o)

GE_BIN_O = $(CODEC_O) $(OBJDIR)/generate_epims.o

all: src/scripts/generate_epims

src/scripts/generate_epims: $(GE_BIN_O)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(ALL_OBJS) $(TARGET)

BASE_H = four_value_logic.h frame_fv.h macros.h queue_fv.h
CODEC_H = fixed_frame_huffman.h huffman.h lzw.h
PARSER_H = parser_interface.h

$(OBJDIR)/generate_epims.o: generate_epims.cpp $(BASE_H) $(CODEC_H)
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(OBJDIR)/huffman.o: huffman.cpp $(CODEC_H) $(BASE_H)
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(OBJDIR)/lzw.o: lzw.cpp $(CODEC_H) $(BASE_H)
	$(CXX) -c $< $(CXXFLAGS) -o $@