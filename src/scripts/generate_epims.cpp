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
  int ecal_threshold = parameters.ecal_threshold;

  os << "module epim_energy" << unique_name_suffix << "(ecal, hcal, energy_pass);" << endl;
  os << "  parameter CAL_BITS = " << cal_bits << ";" << endl;
  os << "  parameter ECAL_THRESHOLD = " << cal_bits << "'d" << ecal_threshold << ";" << endl;
  os << "  input [CAL_BITS-1:0] ecal, hcal;" << endl;
  os << "  output reg energy_pass;" << endl;
  os << endl;
  os << "  always@(*) begin" << endl;
  os << "    energy_pass = ecal > ECAL_THRESHOLD;" << endl;
  os << "  end" << endl;
  os << "endmodule" << endl;
}

void print_epim_veto(ostream& os, const Parameters& parameters,
                     const string& unique_name_suffix) {
  int cal_bits = parameters.cal_bits;
  const set<int>& ecal_vetoes = parameters.ecal_vetoes;
  const set<int>& hcal_vetoes = parameters.hcal_vetoes;

  os << "module epim_veto" << unique_name_suffix << "(ecal, hcal, veto_pass);" << endl;
  os << "  parameter CAL_BITS = " << cal_bits << ";" << endl;
  os << "  input [CAL_BITS-1:0] ecal, hcal;" << endl;
  os << "  output reg veto_pass;" << endl;
  os << endl;
  os << "  always@(*) begin" << endl;
  os << "    veto_pass = 1'b1;" << endl;
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
  os << "  end" << endl;
  os << "endmodule" << endl;
}

void print_vivado_script_preamble(
    ostream& os, const string& project_dir, const string& part_num,
    const string& hdl_dir) {
  os << "create_project epim_test " << project_dir << " -part " << part_num
     << endl;
  os << "import_files " << hdl_dir << endl << endl;
}

void print_vivado_script_entry(
    ostream& os, const string& name_suffix, const string& output_path) {
  os << "set_property top epim" << name_suffix << " [current_fileset]" << endl;
  os << "update_compile_order -fileset sources_1" << endl;
  os << "synth_design -rtl -name -rtl_1" << endl;
  os << "reset_run synth_1" << endl;
  os << "launch_runs synth_1" << endl;
  os << "refresh_design" << endl;
  os << "open_run synth_1 -name netlist_1" << endl;
  os << "report_utilization >> " << output_path << "/epim_" << name_suffix
     << "_utilization.txt" << endl;
  os << "report_timing >> " << output_path << "/epim_" << name_suffix
     << "_timing.txt" << endl << endl;
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

void compress_memory_image(QueueFv& image) {
  QueueFv dead_soon = image;
  VFrameDeque memory_vfd = ConvertToFrameDeque(std::move(dead_soon), 64);
  HuffmanCodec codec(memory_vfd, 64, 32);
  vector<bool> encoded = codec.Encode(memory_vfd);
  cout << "Encoded bits to size: " << encoded.size() << endl;
}

int main(int argc, char* argv[]) {

  Parameters parameters;

  const string output_file_prefix = "out/epim_";

  const int initial_seed = 0;
  const int start_num_vetoes = 0;
  const int increment_vetoes = 10;
  const int end_num_vetoes = 200;
  const int veto_energy_min = 0;
  const int veto_energy_max = 1023;

  // We continue using previous veto values, so successive sets of vetoes are
  // supersets of the previous ones.
  default_random_engine generator(initial_seed);
  uniform_int_distribution<int> distribution(
      veto_energy_min, veto_energy_max);

  for (int num_vetoes = start_num_vetoes; num_vetoes < end_num_vetoes;
       num_vetoes += increment_vetoes) {
    ofstream output_file;
    stringstream output_file_name;
    output_file_name << output_file_prefix << num_vetoes << ".v";
    output_file.open(output_file_name.str(), ofstream::out | ofstream::trunc);
    assert(output_file.is_open());

    ostream& os = output_file;

    while (parameters.ecal_vetoes.size() < num_vetoes) {
      parameters.ecal_vetoes.insert(distribution(generator));
    }
    while (parameters.hcal_vetoes.size() < num_vetoes) {
      parameters.hcal_vetoes.insert(distribution(generator));
    }

    stringstream ss;
    ss << "_" << num_vetoes;
    print_epim(os, parameters, ss.str());
    os << endl;
    print_epim_ratio(os, parameters, ss.str());
    os << endl;
    print_epim_energy(os, parameters, ss.str());
    os << endl;

    print_epim_veto(os, parameters, ss.str());
    os << endl;

    //QueueFv memory = get_memory_image(parameters);
    //compress_memory_image(memory);
  }

  return 0;
}
