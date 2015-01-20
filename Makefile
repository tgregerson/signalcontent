OBJDIR = build_files

VPATH = src/base:src/codec:src/parser:src/standalone

CXXFLAGS_OPT = -O3 -g -Wall -fmessage-length=0 -std=c++0x -flto
CXXFLAGS_DEBUG = -O0 -g -Wall -std=c++0x
CXXFLAGS = $(CXXFLAGS_DEBUG)
CXX = g++

LDFLAGS = -flto
LDFLAGS_CV = $(LDFLAGS) `pkg-config --libs opencv` -lboost_program_options

ALL_OBJS = $(addprefix $(OBJDIR)/,generate_epims.o huffman.o lzw.o)

CODEC_O = $(addprefix $(OBJDIR)/,huffman.o lzw.o)

GE_BIN_O = $(CODEC_O) $(OBJDIR)/generate_epims.o

GR_BIN_O = $(CODEC_O) $(OBJDIR)/generate_rct_tower_inputs.o

SBM_BIN_O = $(addprefix $(OBJDIR)/,dlsc_stereobm_models_program.o dlsc_stereobm_models.o)

all: src/standalone/generate_epims src/standalone/generate_rct_tower_inputs src/standalone/dlsc_stereobm_models_program

src/standalone/generate_epims: $(GE_BIN_O)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

src/standalone/generate_rct_tower_inputs: $(GR_BIN_O)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	
src/standalone/dlsc_stereobm_models_program: $(SBM_BIN_O)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS_CV)

clean:
	rm -f $(ALL_OBJS) $(TARGET)

BASE_H = four_value_logic.h frame_fv.h macros.h queue_fv.h
CODEC_H = fixed_frame_huffman.h huffman.h lzw.h
PARSER_H = parser_interface.h

$(OBJDIR)/dlsc_stereobm_models.o: dlsc_stereobm_models_program.cpp dlsc_stereobm_models.h
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(OBJDIR)/dlsc_stereobm_models_program.o: dlsc_stereobm_models.cpp dlsc_stereobm_models.h
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(OBJDIR)/generate_epims.o: generate_epims.cpp $(BASE_H) $(CODEC_H)
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(OBJDIR)/generate_rct_tower_inputs.o: generate_rct_tower_inputs.cpp $(BASE_H) $(CODEC_H)
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(OBJDIR)/huffman.o: huffman.cpp $(CODEC_H) $(BASE_H)
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(OBJDIR)/lzw.o: lzw.cpp $(CODEC_H) $(BASE_H)
	$(CXX) -c $< $(CXXFLAGS) -o $@