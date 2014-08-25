/*
 * rct_input_entropy.cpp
 *
 *  Created on: Jul 22, 2014
 *      Author: gregerso
 */

#include <cassert>

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "../codec/entropy.h"

using namespace std;

int main(int argc, char* argv[]) {

  assert(argc == 2);

  const string ecal_in_filename = string(argv[1]) + "/ecal_towers_72x56.txt";
  const string hcal_in_filename = string(argv[1]) + "/hcal_towers_72x56.txt";
  ifstream ecal_in_file(ecal_in_filename);
  ifstream hcal_in_file(hcal_in_filename);
  assert(ecal_in_file.is_open());
  assert(hcal_in_file.is_open());

  vector<vector<vector<int>>> ecal_towers;
  vector<vector<vector<int>>> hcal_towers;

  // Preallocate 72x56 2D vector of vectors.
  for (int i = 0; i < 72; i++) {
    vector<vector<int>> row(56);
    ecal_towers.push_back(row);
    hcal_towers.push_back(row);
  }

  string line;
  int ecal, hcal;
  while (!ecal_in_file.eof() && !hcal_in_file.eof()) {
    for (int i = 0; i < 72; ++i) {
      for (int j = 0; j < 56; ++j) {
        ecal_in_file >> ecal;
        hcal_in_file >> hcal;
        ecal_towers[i][j].push_back(ecal);
        hcal_towers[i][j].push_back(hcal);
      }
    }
  }

  cout << "Found " << ecal_towers[0][0].size() << " cycles.\n";
  signal_content::codec::ShannonEntropyAccumulator<int> entropy_accumulator_e;
  signal_content::codec::ShannonEntropyAccumulator<int> entropy_accumulator_h;

  for (auto& v1 : ecal_towers) {
    for (auto& v2 : v1) {
      for (int e_val : v2) {
        entropy_accumulator_e.AddSample(e_val);
      }
    }
  }
  for (auto& v1 : hcal_towers) {
    for (auto& v2 : v1) {
      for (int e_val : v2) {
        entropy_accumulator_h.AddSample(e_val);
      }
    }
  }

  cout << "Entropy: " << entropy_accumulator_e.GetEntropy() << " bits.\n";
  cout << "Entropy: " << entropy_accumulator_h.GetEntropy() << " bits.\n";

  return 0;
}


