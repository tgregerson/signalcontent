module epim(ecal, hcal, egamma);
  parameter CAL_BITS = 10;
  parameter SCALED_RATIO_THRESHOLD = 10'd256;
  parameter RATIO_THRESHOLD_SHIFT_BITS = CAL_BITS - 1;
  parameter ECAL_THRESHOLD = 10'd50;
  input [CAL_BITS-1:0] ecal, hcal;
  output egamma;

  wire ratio_pass, energy_pass, veto_pass;

  assign egamma = (ratio_pass | energy_pass) & veto_pass;

  epim_ratio
      #(
        .CAL_BITS(CAL_BITS),
        .SCALED_RATIO_THRESHOLD(SCALED_RATIO_THRESHOLD),
        .RATIO_THRESHOLD_SHIFT_BITS(RATIO_THRESHOLD_SHIFT_BITS)
      )
      er_inst(
        .ecal(ecal),
        .hcal(hcal),
        .ratio_pass(ratio_pass)
      );

  epim_energy
      #(
        .CAL_BITS(CAL_BITS),
        .ECAL_THRESHOLD(ECAL_THRESHOLD)
      )
      ee_inst(
        .ecal(ecal),
        .hcal(hcal),
        .energy_pass(energy_pass)
      );

  epim_veto
      #(
        .CAL_BITS(CAL_BITS)
      )
      ev_inst(
        .ecal(ecal),
        .hcal(hcal),
        .veto_pass(veto_pass)
      );

endmodule

module epim_ratio(ecal, hcal, ratio_pass);
  parameter CAL_BITS = 10;
  parameter SCALED_RATIO_THRESHOLD = 10'd256;
  parameter RATIO_THRESHOLD_SHIFT_BITS = CAL_BITS - 1;
  input [CAL_BITS-1:0] ecal, hcal;
  output reg ratio_pass;

  reg [CAL_BITS*2-1:0] scaled_sum, scaled_ecal;

  always@(*) begin
    scaled_sum = (ecal + hcal) * SCALED_RATIO_THRESHOLD;
    scaled_ecal = ecal << RATIO_THRESHOLD_SHIFT_BITS;
    ratio_pass = scaled_sum < scaled_ecal;
  end
endmodule

module epim_energy(ecal, hcal, energy_pass);
  parameter CAL_BITS = 10;
  parameter ECAL_THRESHOLD = 10'd50;
  input [CAL_BITS-1:0] ecal, hcal;
  output reg energy_pass;

  always@(*) begin
    energy_pass = ecal > ECAL_THRESHOLD;
  end
endmodule

module epim_veto(ecal, hcal, veto_pass);
  parameter CAL_BITS = 10;
  input [CAL_BITS-1:0] ecal, hcal;
  output reg veto_pass;

  always@(*) begin
    veto_pass = 1'b1;
    case (ecal)
      10'd20: veto_pass = 1'b0;
      10'd50: veto_pass = 1'b0;
      10'd120: veto_pass = 1'b0;
    endcase
    case (hcal)
      10'd30: veto_pass = 1'b0;
      10'd60: veto_pass = 1'b0;
      10'd90: veto_pass = 1'b0;
    endcase
  end
endmodule

