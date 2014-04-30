#include <iostream>
#include <set>

using namespace std;

struct Parameters {
  int cal_bits = 10;
  int shift_bits = 9;
  double ratio_threshold = 0.5;
  int ecal_threshold = 50;
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
  os << "  parameter SCALED_RATIO_THRESHOLD = CAL_BITS'd" << scaled_ratio_threshold << ";" << endl;
  os << "  parameter RATIO_THRESHOLD_SHIFT_BITS = CAL_BITS - 1;" << endl;
  os << "  parameter ECAL_THRESHOLD = CAL_BITS'd" << ecal_threshold << ";" << endl;
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
  os << "  parameter SCALED_RATIO_THRESHOLD = CAL_BITS'd" << scaled_ratio_threshold << ";" << endl;
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
  os << "  parameter ECAL_THRESHOLD = CAL_BITS'd" << ecal_threshold << ";" << endl;
  os << "  input [CAL_BITS-1:0] ecal, hcal;" << endl;
  os << "  output reg energy_pass;" << endl;
  os << endl;
  os << "  always@(*) begin" << endl;
  os << "    energy_pass = ecal > ECAL_THRESHOLD;" << endl;
  os << "  end" << endl;
  os << "endmodule" << endl;
}

void print_epim_veto(ostream& os, const Parameters& parameters, const string& unique_name_suffix,
                     const set<int>& ecal_vetoes, const set<int>& hcal_vetoes) {
  int cal_bits = parameters.cal_bits;

  os << "module epim_veto" << unique_name_suffix << "(ecal, hcal, energy_pass);" << endl;
  os << "  parameter CAL_BITS = " << cal_bits << ";" << endl;
  os << "  input [CAL_BITS-1:0] ecal, hcal;" << endl;
  os << "  output reg veto_pass;" << endl;
  os << endl;
  os << "  always@(*) begin" << endl;
  os << "    veto_pass = 1'b1;" << endl;
  if (!ecal_vetoes.empty()) {
    os << "    case (ecal) begin" << endl;
    for (int val : ecal_vetoes) {
      os << "      " << cal_bits << "'d" << val << ": veto_pass = 1'b0;" << endl;
    }
    os << "    endcase" << endl;
  }
  if (!ecal_vetoes.empty()) {
    os << "    case (hcal) begin" << endl;
    for (int val : hcal_vetoes) {
      os << "      " << cal_bits << "'d" << val << ": veto_pass = 1'b0;" << endl;
    }
    os << "    endcase" << endl;
  }
  os << "  end" << endl;
  os << "endmodule" << endl;
}

int main(int argc, char* argv[]) {

  ostream& os = cout;
  Parameters parameters;

  print_epim(os, parameters, "");
  os << endl;
  print_epim_ratio(os, parameters, "");
  os << endl;
  print_epim_energy(os, parameters, "");
  os << endl;

  set<int> ecal_vetoes = {
    20, 50, 120
  };
  set<int> hcal_vetoes = {
    30, 60, 90
  };
  print_epim_veto(os, parameters, "", ecal_vetoes, hcal_vetoes);
  os << endl;

  return 0;
}
