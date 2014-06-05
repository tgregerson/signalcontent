#include <cassert>

#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <sstream>

#include "../base/four_value_logic.h"
#include "../base/frame_fv.h"
#include "../base/queue_fv.h"
#include "../codec/huffman.h"
#include "../codec/lzw.h"

using namespace std;
using namespace signal_content::base;
using namespace signal_content::codec;

struct Parameters {
  int cal_bits = 10;
  int shift_bits = 9;
  double ratio_threshold = 0.5;
  int ecal_threshold = 50;
  set<int> ecal_vetoes;
  set<int> hcal_vetoes;
  set<int> ecalhcal_vetoes;
  set<int> segment_end_points;
};

void print_epim(ostream& os, const Parameters& parameters, const string& unique_name_suffix) {
  int cal_bits = parameters.cal_bits;
  int shift_bits = parameters.shift_bits;
  double ratio_threshold = parameters.ratio_threshold;
  int ecal_threshold = parameters.ecal_threshold;

  for (int i = 0; i < shift_bits; ++i) {
    ratio_threshold *= 2;
  }
  int scaled_ratio_threshold = (int)ratio_threshold;

  os << "module epim" << unique_name_suffix << "(ecal, hcal, egamma);" << endl;
  os << "  parameter CAL_BITS = " << cal_bits << ";" << endl;
  os << "  parameter SCALED_RATIO_THRESHOLD = " << cal_bits << "'d" << scaled_ratio_threshold << ";" << endl;
  os << "  parameter RATIO_THRESHOLD_SHIFT_BITS = CAL_BITS - 1;" << endl;
  os << "  parameter ECAL_THRESHOLD = " << cal_bits << "'d" << ecal_threshold << ";" << endl;
  os << "  input [CAL_BITS-1:0] ecal, hcal;" << endl;
  os << "  output egamma;" << endl;
  os << endl;
  os << "  wire ratio_pass, energy_pass, veto_pass;" << endl;
  os << endl;
  os << "  assign egamma = (ratio_pass | energy_pass) & veto_pass;" << endl;
  os << endl;
  os << "  epim_ratio" << unique_name_suffix << endl;
  os << "      #(" << endl;
  os << "        .CAL_BITS(CAL_BITS)," << endl;
  os << "        .SCALED_RATIO_THRESHOLD(SCALED_RATIO_THRESHOLD)," << endl;
  os << "        .RATIO_THRESHOLD_SHIFT_BITS(RATIO_THRESHOLD_SHIFT_BITS)" << endl;
  os << "      )" << endl;
  os << "      er_inst(" << endl;
  os << "        .ecal(ecal)," << endl;
  os << "        .hcal(hcal)," << endl;
  os << "        .ratio_pass(ratio_pass)" << endl;
  os << "      );" << endl;
  os << endl;
  os << "  epim_energy" << unique_name_suffix << endl;
  os << "      #(" << endl;
  os << "        .CAL_BITS(CAL_BITS)," << endl;
  os << "        .ECAL_THRESHOLD(ECAL_THRESHOLD)" << endl;
  os << "      )" << endl;
  os << "      ee_inst(" << endl;
  os << "        .ecal(ecal)," << endl;
  os << "        .hcal(hcal)," << endl;
  os << "        .energy_pass(energy_pass)" << endl;
  os << "      );" << endl;
  os << endl;
  os << "  epim_veto" << unique_name_suffix << endl;
  os << "      #(" << endl;
  os << "        .CAL_BITS(CAL_BITS)" << endl;
  os << "      )" << endl;
  os << "      ev_inst(" << endl;
  os << "        .ecal(ecal)," << endl;
  os << "        .hcal(hcal)," << endl;
  os << "        .veto_pass(veto_pass)" << endl;
  os << "      );" << endl;
  os << endl;
  os << "endmodule" << endl;
}

void print_epim_ratio(ostream& os, const Parameters& parameters, const string& unique_name_suffix) {
  int cal_bits = parameters.cal_bits;
  int shift_bits = parameters.shift_bits;
  double ratio_threshold = parameters.ratio_threshold;

  for (int i = 0; i < shift_bits; ++i) {
    ratio_threshold *= 2;
  }
  int scaled_ratio_threshold = (int)ratio_threshold;

  os << "module epim_ratio" << unique_name_suffix << "(ecal, hcal, ratio_pass);" << endl;
  os << "  parameter CAL_BITS = " << cal_bits << ";" << endl;
  os << "  parameter SCALED_RATIO_THRESHOLD = " << cal_bits <<"'d" << scaled_ratio_threshold << ";" << endl;
  os << "  parameter RATIO_THRESHOLD_SHIFT_BITS = CAL_BITS - 1;" << endl;
  os << "  input [CAL_BITS-1:0] ecal, hcal;" << endl;
  os << "  output reg ratio_pass;" << endl;
  os << endl;
  os << "  reg [CAL_BITS*2-1:0] scaled_sum, scaled_ecal;" << endl;
  os << endl;
  os << "  always@(*) begin" << endl;
  os << "    scaled_sum = (ecal + hcal) * SCALED_RATIO_THRESHOLD;" << endl;
  os << "    scaled_ecal = ecal << RATIO_THRESHOLD_SHIFT_BITS;" << endl;
  os << "    ratio_pass = scaled_sum < scaled_ecal;" << endl;
  os << "  end" << endl;
  os << "endmodule" << endl;
}

void print_epim_energy(ostream& os, const Parameters& parameters, const string& unique_name_suffix) {
  int cal_bits = parameters.cal_bits;
  int concatenated_cal_bits = 2 * parameters.cal_bits;
  int ecal_threshold = parameters.ecal_threshold;

  os << "module epim_energy" << unique_name_suffix << "(ecal, hcal, energy_pass);" << endl;
  os << "  parameter CAL_BITS = " << cal_bits << ";" << endl;
  os << "  parameter ECAL_THRESHOLD = " << cal_bits << "'d" << ecal_threshold << ";" << endl;
  os << "  input [CAL_BITS-1:0] ecal, hcal;" << endl;
  os << "  output reg energy_pass;" << endl;
  os << endl;
  os << "  reg [2*CAL_BITS-1:0] ecalhcal;" << endl;
  os << endl;
  os << "  always@(*) begin" << endl;
  if (parameters.segment_end_points.empty()) {
    os << "    energy_pass = ecal > ECAL_THRESHOLD;" << endl;
  } else {
    os << "    energy_pass = 1'b0;" << endl;
    os << "    ecalhcal = {ecal, hcal};" << endl;
    auto it = parameters.segment_end_points.begin();
      os << "    if (ecalhcal < " << concatenated_cal_bits << "'d"
         << *it << ") begin energy_pass = 1'b1; end" << endl;
    it++;
    int next_pass_val = 0;
    for (; it != parameters.segment_end_points.end(); it++) {
      int end_point = *it;
      os << "    else if (ecalhcal < " << concatenated_cal_bits << "'d"
         << end_point << ") begin energy_pass = 1'b" << next_pass_val
         << "; end" << endl;
      if (next_pass_val == 0) {
        next_pass_val = 1;
      } else {
        next_pass_val = 0;
      }
    }
  }
  os << "  end" << endl;
  os << "endmodule" << endl;
}

void print_epim_veto(ostream& os, const Parameters& parameters,
                     const string& unique_name_suffix) {
  int cal_bits = parameters.cal_bits;
  int combined_cal_bits = cal_bits * 2;
  const set<int>& ecal_vetoes = parameters.ecal_vetoes;
  const set<int>& hcal_vetoes = parameters.hcal_vetoes;
  const set<int>& ecalhcal_vetoes = parameters.ecalhcal_vetoes;

  os << "module epim_veto" << unique_name_suffix << "(ecal, hcal, veto_pass);" << endl;
  os << "  parameter CAL_BITS = " << cal_bits << ";" << endl;
  os << "  input [CAL_BITS-1:0] ecal, hcal;" << endl;
  os << "  output reg veto_pass;" << endl;
  os << endl;
  os << "  reg [2*CAL_BITS-1:0] ecalhcal;" << endl;
  os << "  always@(*) begin" << endl;
  os << "    veto_pass = 1'b1;" << endl;
  os << "    ecalhcal = {ecal, hcal};" << endl;
  if (!ecal_vetoes.empty()) {
    os << "    case (ecal)" << endl;
    for (int val : ecal_vetoes) {
      os << "      " << cal_bits << "'d" << val << ": veto_pass = 1'b0;" << endl;
    }
    os << "    endcase" << endl;
  }
  if (!ecal_vetoes.empty()) {
    os << "    case (hcal)" << endl;
    for (int val : hcal_vetoes) {
      os << "      " << cal_bits << "'d" << val << ": veto_pass = 1'b0;" << endl;
    }
    os << "    endcase" << endl;
  }
  if (!ecalhcal_vetoes.empty()) {
    os << "    case (ecalhcal)" << endl;
    for (int val : ecalhcal_vetoes) {
      os << "      " << combined_cal_bits << "'d" << val << ": veto_pass = 1'b0;" << endl;
    }
    os << "    endcase" << endl;
  }
  os << "  end" << endl;
  os << "endmodule" << endl;
}

void print_epim_verilog(const string& output_dir,
                        const string& unique_name_suffix,
                        const Parameters& parameters) {
  ofstream output_file;
  stringstream output_file_name;
  output_file_name << output_dir << "/epim" << unique_name_suffix << ".v";
  output_file.open(output_file_name.str(), ofstream::out | ofstream::trunc);
  assert(output_file.is_open());

  ostream& os = output_file;

  print_epim(os, parameters, unique_name_suffix);
  os << endl;
  print_epim_ratio(os, parameters, unique_name_suffix);
  os << endl;
  print_epim_energy(os, parameters, unique_name_suffix);
  os << endl;

  print_epim_veto(os, parameters, unique_name_suffix);
  os << endl;
}

void print_vivado_script_preamble(
    ostream& os, const string& project_dir, const string& part_num,
    const string& hdl_dir) {
  os << "create_project epim_test -force " << project_dir << "/epim_test -part "
     << part_num << endl;
  os << "import_files " << hdl_dir << endl << endl;
}

void print_vivado_script_entry(
    ostream& os, const string& name_suffix, const string& output_path) {
  os << "set_property top epim" << name_suffix << " [current_fileset]" << endl;
  os << "set_property strategy no_bram [get_runs synth_1]" << endl;
  os << "update_compile_order -fileset sources_1" << endl;
  os << "synth_design" << endl;
  os << "report_utilization > " << output_path << "/epim" << name_suffix
     << "_utilization.txt" << endl;
  os << "report_timing > " << output_path << "/epim" << name_suffix
     << "_timing.txt" << endl << endl;
}

void print_vivado_script_post(ostream& os) {
  os << "exit" << endl;
}

QueueFv get_memory_image(const Parameters& parameters) {
  QueueFv memory;
  const int num_addr_bits = parameters.cal_bits * 2;
  assert (num_addr_bits > 0);
  const int num_contents_bits = 1 << (num_addr_bits - 1);

  const int addr_mask = ((1 << num_addr_bits) - 1);
  const int ecal_mask = (1 << (parameters.cal_bits)) - 1;
  const int hcal_mask = addr_mask & ~ecal_mask;

  for (int i = 0; i < num_contents_bits; ++i) {
    int ecal = i & ecal_mask;
    int hcal = i & hcal_mask;
    double ratio = ((double)ecal) / ((double)ecal + (double)hcal);
    bool ratio_pass = ratio > parameters.ratio_threshold;
    bool veto = (parameters.ecal_vetoes.find(ecal) != parameters.ecal_vetoes.end()) |
                (parameters.hcal_vetoes.find(hcal) != parameters.hcal_vetoes.end());
    memory.push(
        signal_content::base::FourValueLogicFromBool(
            ((ecal > parameters.ecal_threshold) | ratio_pass) & ~veto));
  }
  return memory;
}

void compress_memory_image(ofstream& os, QueueFv& image, Parameters& parameters) {
  int segments = parameters.segment_end_points.size();
  int vetoes = parameters.ecal_vetoes.size() + parameters.ecalhcal_vetoes.size();

  os << segments << ", " << vetoes << ", ";

  // Compress with LZW
  LzwCodec lzw_codec;
  lzw_codec.PopulateDictionary(image);
  vector<int> lzw_encoded = lzw_codec.Encode(image);
  os << (lzw_encoded.size() * 12) << ", ";

  QueueFv dead_soon = image;
  VFrameDeque memory_vfd = ConvertToFrameDeque(std::move(dead_soon), 64);
  HuffmanCodec huffman_codec(memory_vfd, 64, 16);
  vector<bool> huffman_encoded = huffman_codec.Encode(memory_vfd);
  os << huffman_encoded.size() << endl;
}

int main(int argc, char* argv[]) {

  Parameters parameters;

  bool make_verilog = false;
  bool make_scripts = false;
  bool compress_memory = true;
  bool use_concatenated = true;

  const int initial_seed = 0;

  const int start_num_vetoes = 0;
  const int increment_vetoes = 10;
  const int end_num_vetoes = 500;
  const int veto_energy_min = 0;
  const int veto_energy_max = 1023;
  const int ecalhcal_min = 0;
  const int ecalhcal_max = 0x0FFFFF;  // 20 bits

  const int num_segments = 20;
  const int avg_segment_length = ecalhcal_max / num_segments;
  const int min_segment_length = ceil(0.5 * avg_segment_length);
  const int max_segment_length = 1.5 * avg_segment_length;

  const string xilinx_workspace = "/localhome/gregerso/tools/Xilinx/workspace";
  const string xilinx_device = "xc7a200tlffg1156-2L";
  const string hdl_dir = "/localhome/gregerso/git/signalcontent/src/scripts/out";
  const string script_dir = "/localhome/gregerso/git/signalcontent/src/scripts/out";
  const string memory_compression_dir = "/localhome/gregerso/git/signalcontent/src/scripts/out";

  // We continue using previous veto values, so successive sets of vetoes are
  // supersets of the previous ones.
  default_random_engine generator(initial_seed);
  uniform_int_distribution<int> distribution(
      veto_energy_min, veto_energy_max);
  uniform_int_distribution<int> ecalhcal_distribution(
      ecalhcal_min, ecalhcal_max);
  uniform_int_distribution<int> segment_distribution(
      min_segment_length, max_segment_length);

  ofstream script_file;
  if (make_scripts) {
    stringstream ss;
    ss << script_dir << "/epim_";
    ss << "0-" << num_segments << "_";
    ss << start_num_vetoes << "-" << end_num_vetoes << ".tcl";
    script_file.open(ss.str(), ofstream::out | ofstream::trunc);
    assert(script_file.is_open());
    print_vivado_script_preamble(script_file,
                                 xilinx_workspace,
                                 xilinx_device,
                                 hdl_dir);
  }

  ofstream memory_compression_file;
  if (compress_memory) {
    stringstream ss;
    ss << memory_compression_dir << "/memory_epim_";
    ss << "0-" << num_segments << "_";
    ss << start_num_vetoes << "-" << end_num_vetoes << ".tcl";
    memory_compression_file.open(ss.str(), ofstream::out | ofstream::trunc);
    assert(memory_compression_file.is_open());
  }

  vector<Parameters> segment_parameter_sets;
  for (int segment = 1; segment < num_segments; ++segment) {
    Parameters parameters;
    if (!segment_parameter_sets.empty()) {
      parameters = segment_parameter_sets.back();
    }
    int last_end_point = 0;
    while (parameters.segment_end_points.size() < segment &&
           last_end_point < ecalhcal_max) {
      int new_end_point = last_end_point + segment_distribution(generator);
      if (new_end_point < ecalhcal_max) {
        parameters.segment_end_points.insert(new_end_point);
      }
      last_end_point = new_end_point;
    }
    segment_parameter_sets.push_back(parameters);
  }

  vector<Parameters> parameter_sets;
  for (Parameters parameters : segment_parameter_sets) {
    for (int num_vetoes = start_num_vetoes; num_vetoes < end_num_vetoes;
        num_vetoes += increment_vetoes) {
      if (!parameter_sets.empty() &&
          (parameter_sets.back().ecal_vetoes.size() +
           parameter_sets.back().ecalhcal_vetoes.size() < num_vetoes)) {
        // Make sure that larger sets are supersets of smaller ones.
        parameters.ecal_vetoes = parameter_sets.back().ecal_vetoes;
        parameters.hcal_vetoes = parameter_sets.back().hcal_vetoes;
        parameters.ecalhcal_vetoes = parameter_sets.back().ecalhcal_vetoes;
      }

      if (use_concatenated) {
        while (parameters.ecalhcal_vetoes.size() < num_vetoes) {
          parameters.ecalhcal_vetoes.insert(ecalhcal_distribution(generator));
        }
      } else {
        while (parameters.ecal_vetoes.size() < num_vetoes) {
          parameters.ecal_vetoes.insert(distribution(generator));
        }
        while (parameters.hcal_vetoes.size() < num_vetoes) {
          parameters.hcal_vetoes.insert(distribution(generator));
        }
      }
      parameter_sets.push_back(parameters);
    }
  }

  for (Parameters parameters : parameter_sets) {
    stringstream ss;
    int num_vetoes = parameters.ecal_vetoes.size() +
                     parameters.hcal_vetoes.size() +
                     parameters.ecalhcal_vetoes.size();
    int num_segments = parameters.segment_end_points.size();
    ss << "_" << num_segments << "_" << num_vetoes;

    if (make_verilog) {
      print_epim_verilog(hdl_dir, ss.str(), parameters);
    }
    if (make_scripts) {
      print_vivado_script_entry(
          script_file, ss.str(), "/localhome/gregerso/temp");
    }
    if (compress_memory) {
      cout << "Compressing " << num_segments << "_" << num_vetoes << endl;
      QueueFv memory = get_memory_image(parameters);
      compress_memory_image(memory_compression_file, memory, parameters);
    }
  }

  if (make_scripts) {
    print_vivado_script_post(script_file);
  }

  return 0;
}
