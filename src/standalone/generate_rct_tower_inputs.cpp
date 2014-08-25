/*
 * generate_rct_tower_inputs.cpp
 *
 *  Created on: Jul 22, 2014
 *      Author: gregerso
 */

#include <cassert>

#include <algorithm>
#include <array>
#include <fstream>
#include <random>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

  assert(argc == 2);

  // Only argument is the path to E/HCAL1, 2, 3, and 4.txt
  const string ecal1_filename = string(argv[1]) + "/ECAL1.txt";
  const string ecal2_filename = string(argv[1]) + "/ECAL2.txt";
  const string ecal3_filename = string(argv[1]) + "/ECAL3.txt";
  const string ecal4_filename = string(argv[1]) + "/ECAL4.txt";
  const string hcal1_filename = string(argv[1]) + "/HCAL1.txt";
  const string hcal2_filename = string(argv[1]) + "/HCAL2.txt";
  const string hcal3_filename = string(argv[1]) + "/HCAL3.txt";
  const string hcal4_filename = string(argv[1]) + "/HCAL4.txt";

  ifstream ecal1_file(ecal1_filename);
  ifstream ecal2_file(ecal2_filename);
  ifstream ecal3_file(ecal3_filename);
  ifstream ecal4_file(ecal4_filename);
  ifstream hcal1_file(hcal1_filename);
  ifstream hcal2_file(hcal2_filename);
  ifstream hcal3_file(hcal3_filename);
  ifstream hcal4_file(hcal4_filename);

  assert(ecal1_file.is_open());
  assert(ecal2_file.is_open());
  assert(ecal3_file.is_open());
  assert(ecal4_file.is_open());
  assert(hcal1_file.is_open());
  assert(hcal2_file.is_open());
  assert(hcal3_file.is_open());
  assert(hcal4_file.is_open());

  const string ecal_out_filename = string(argv[1]) + "/ecal_towers_72x56.txt";
  const string hcal_out_filename = string(argv[1]) + "/hcal_towers_72x56.txt";
  ofstream ecal_out_file(ecal_out_filename);
  ofstream hcal_out_file(hcal_out_filename);
  assert(ecal_out_file.is_open());
  assert(hcal_out_file.is_open());

  vector<vector<vector<int>>> ecal_towers;
  vector<vector<vector<int>>> hcal_towers;

  // Preallocate 72x56 2D vector of vectors.
  for (int i = 0; i < 72; i++) {
    vector<vector<int>> row(56);
    ecal_towers.push_back(row);
    hcal_towers.push_back(row);
  }

  string line;

  // Skip 'int' line in each input file.
  std::getline(ecal1_file, line);
  std::getline(ecal2_file, line);
  std::getline(ecal3_file, line);
  std::getline(ecal4_file, line);
  std::getline(hcal1_file, line);
  std::getline(hcal2_file, line);
  std::getline(hcal3_file, line);
  std::getline(hcal4_file, line);

  default_random_engine generator(0);
  normal_distribution<double> dist(1.0, 0.2);

  vector<int> ecal_tower1;
  vector<int> ecal_tower2;
  vector<int> ecal_tower3;
  vector<int> ecal_tower4;
  vector<int> hcal_tower1;
  vector<int> hcal_tower2;
  vector<int> hcal_tower3;
  vector<int> hcal_tower4;

  int ecal;
  int hcal;
  for (int i = 0; i < 1330; ++i) {
    ecal1_file >> ecal;
    ecal_tower1.push_back(ecal);
    ecal2_file >> ecal;
    ecal_tower2.push_back(ecal);
    ecal3_file >> ecal;
    ecal_tower3.push_back(ecal);
    ecal4_file >> ecal;
    ecal_tower4.push_back(ecal);
    hcal1_file >> hcal;
    hcal_tower1.push_back(hcal);
    hcal2_file >> hcal;
    hcal_tower2.push_back(hcal);
    hcal3_file >> hcal;
    hcal_tower3.push_back(hcal);
    hcal4_file >> hcal;
    hcal_tower4.push_back(hcal);
  }

  for (int i = 0; i < 72; i += 4) {
    for (int j = 0; j < 56; ++j) {
      std::shuffle(ecal_tower1.begin(), ecal_tower1.end(), generator);
      std::shuffle(ecal_tower2.begin(), ecal_tower2.end(), generator);
      std::shuffle(ecal_tower3.begin(), ecal_tower3.end(), generator);
      std::shuffle(ecal_tower4.begin(), ecal_tower4.end(), generator);
      std::shuffle(hcal_tower1.begin(), hcal_tower1.end(), generator);
      std::shuffle(hcal_tower2.begin(), hcal_tower2.end(), generator);
      std::shuffle(hcal_tower3.begin(), hcal_tower3.end(), generator);
      std::shuffle(hcal_tower4.begin(), hcal_tower4.end(), generator);
      for (int k = 0; k < ecal_tower1.size(); ++k) {
        double modifier = dist(generator);
        if (modifier < 0) {
          modifier = 0;
        }
        ecal_towers[i + 0][j].push_back(modifier * ecal_tower1[k]);
        ecal_towers[i + 1][j].push_back(modifier * ecal_tower2[k]);
        ecal_towers[i + 2][j].push_back(modifier * ecal_tower3[k]);
        ecal_towers[i + 3][j].push_back(modifier * ecal_tower4[k]);
        hcal_towers[i + 0][j].push_back(modifier * hcal_tower1[k]);
        hcal_towers[i + 1][j].push_back(modifier * hcal_tower2[k]);
        hcal_towers[i + 2][j].push_back(modifier * hcal_tower3[k]);
        hcal_towers[i + 3][j].push_back(modifier * hcal_tower4[k]);
      }
    }
  }

  for (int i = 0; i < 72; ++i) {
    for (int j = 0; j < 56; ++j) {
      for (int k = 0; k < ecal_tower1.size(); ++k) {
        ecal_out_file << ecal_towers[i][j][k] << "\n";
        hcal_out_file << hcal_towers[i][j][k] << "\n";
      }
    }
  }

  return 0;
}
