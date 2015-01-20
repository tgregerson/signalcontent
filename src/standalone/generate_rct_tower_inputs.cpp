/*
 * generate_rct_tower_inputs.cpp
 *
 *  Created on: Jul 22, 2014
 *      Author: gregerso
 */

#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <array>
#include <iomanip>
#include <fstream>
#include <random>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

  assert(argc == 4);

  // Must be multiple of 4.
  int X_DIM = atoi(argv[2]);
  int Y_DIM = atoi(argv[3]);
  assert(X_DIM % 4 == 0);

  const int VALS_PER_CAL_INPUT_FILE = 1330;
  const int VALS_PER_FG_INPUT_FILE = 540;

  // Only argument is the path to E/HCAL1, 2, 3, and 4.txt
  const string ecal1_filename = string(argv[1]) + "/ECAL1.txt";
  const string ecal2_filename = string(argv[1]) + "/ECAL2.txt";
  const string ecal3_filename = string(argv[1]) + "/ECAL3.txt";
  const string ecal4_filename = string(argv[1]) + "/ECAL4.txt";
  const string hcal1_filename = string(argv[1]) + "/HCAL1.txt";
  const string hcal2_filename = string(argv[1]) + "/HCAL2.txt";
  const string hcal3_filename = string(argv[1]) + "/HCAL3.txt";
  const string hcal4_filename = string(argv[1]) + "/HCAL4.txt";
  const string fg1_filename = string(argv[1]) + "/FG1.txt";
  const string fg2_filename = string(argv[1]) + "/FG2.txt";
  const string fg3_filename = string(argv[1]) + "/FG3.txt";
  const string fg4_filename = string(argv[1]) + "/FG4.txt";

  ifstream ecal1_file(ecal1_filename);
  ifstream ecal2_file(ecal2_filename);
  ifstream ecal3_file(ecal3_filename);
  ifstream ecal4_file(ecal4_filename);
  ifstream hcal1_file(hcal1_filename);
  ifstream hcal2_file(hcal2_filename);
  ifstream hcal3_file(hcal3_filename);
  ifstream hcal4_file(hcal4_filename);
  ifstream fg1_file(fg1_filename);
  ifstream fg2_file(fg2_filename);
  ifstream fg3_file(fg3_filename);
  ifstream fg4_file(fg4_filename);

  assert(ecal1_file.is_open());
  assert(ecal2_file.is_open());
  assert(ecal3_file.is_open());
  assert(ecal4_file.is_open());
  assert(hcal1_file.is_open());
  assert(hcal2_file.is_open());
  assert(hcal3_file.is_open());
  assert(hcal4_file.is_open());
  assert(fg1_file.is_open());
  assert(fg2_file.is_open());
  assert(fg3_file.is_open());
  assert(fg4_file.is_open());

  stringstream out_filename;
  out_filename << argv[1] << "/towers_" << X_DIM << "x" << Y_DIM << ".txt";
  /*
  stringstream hcal_out_filename =
      string(argv[1]) << "/hcal_towers_" << X_DIM << "x" << Y_DIM << ".txt";
      */
  ofstream out_file(out_filename.str().c_str());
  //ofstream hcal_out_file(hcal_out_filename.str().c_str());
  assert(out_file.is_open());
  //assert(hcal_out_file.is_open());

  vector<vector<vector<int>>> ecal_towers;
  vector<vector<vector<int>>> hcal_towers;
  vector<vector<vector<int>>> fg_towers;

  for (int i = 0; i < X_DIM; i++) {
    vector<vector<int>> row(Y_DIM);
    ecal_towers.push_back(row);
    hcal_towers.push_back(row);
    fg_towers.push_back(row);
  }

  string line;

  // Skip 'int' line in ecal/hcal input files.
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
  vector<int> fg_tower1;
  vector<int> fg_tower2;
  vector<int> fg_tower3;
  vector<int> fg_tower4;

  int ecal;
  int hcal;
  for (int i = 0; i < VALS_PER_CAL_INPUT_FILE; ++i) {
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

  int fg;
  for (int i = 0; i < VALS_PER_FG_INPUT_FILE; ++i) {
    fg1_file >> fg;
    assert(fg <= 1);
    fg_tower1.push_back(fg);
    fg2_file >> fg;
    assert(fg <= 1);
    fg_tower2.push_back(fg);
    fg3_file >> fg;
    assert(fg <= 1);
    fg_tower3.push_back(fg);
    fg4_file >> fg;
    assert(fg <= 1);
    fg_tower4.push_back(fg);
  }
  for (int i = VALS_PER_FG_INPUT_FILE; i < VALS_PER_CAL_INPUT_FILE; ++i) {
    int index = rand() % VALS_PER_FG_INPUT_FILE;
    fg_tower1.push_back(fg_tower1.at(index));
    fg_tower2.push_back(fg_tower2.at(index));
    fg_tower3.push_back(fg_tower3.at(index));
    fg_tower4.push_back(fg_tower4.at(index));
  }

  assert(fg_tower1.size() == ecal_tower1.size() &&
         hcal_tower1.size() == ecal_tower1.size());
  for (int i = 0; i < X_DIM; i += 4) {
    for (int j = 0; j < Y_DIM; ++j) {
      std::shuffle(ecal_tower1.begin(), ecal_tower1.end(), generator);
      std::shuffle(ecal_tower2.begin(), ecal_tower2.end(), generator);
      std::shuffle(ecal_tower3.begin(), ecal_tower3.end(), generator);
      std::shuffle(ecal_tower4.begin(), ecal_tower4.end(), generator);
      std::shuffle(hcal_tower1.begin(), hcal_tower1.end(), generator);
      std::shuffle(hcal_tower2.begin(), hcal_tower2.end(), generator);
      std::shuffle(hcal_tower3.begin(), hcal_tower3.end(), generator);
      std::shuffle(hcal_tower4.begin(), hcal_tower4.end(), generator);
      std::shuffle(fg_tower1.begin(), fg_tower1.end(), generator);
      std::shuffle(fg_tower2.begin(), fg_tower2.end(), generator);
      std::shuffle(fg_tower3.begin(), fg_tower3.end(), generator);
      std::shuffle(fg_tower4.begin(), fg_tower4.end(), generator);
      for (size_t k = 0; k < ecal_tower1.size(); ++k) {
        double modifier = dist(generator);
        if (modifier < 0) {
          modifier = 0;
        }
        int val = modifier * ecal_tower1.at(k);
        if (val > 255) val = 255;
        ecal_towers.at(i + 0).at(j).push_back(val);
        val = modifier * ecal_tower2.at(k);
        if (val > 255) val = 255;
        ecal_towers.at(i + 1).at(j).push_back(val);
        val = modifier * ecal_tower3.at(k);
        if (val > 255) val = 255;
        ecal_towers.at(i + 2).at(j).push_back(val);
        val = modifier * ecal_tower4.at(k);
        if (val > 255) val = 255;
        ecal_towers.at(i + 3).at(j).push_back(val);
        val = modifier * hcal_tower1.at(k);
        if (val > 255) val = 255;
        hcal_towers.at(i + 0).at(j).push_back(val);
        val = modifier * hcal_tower2.at(k);
        if (val > 255) val = 255;
        hcal_towers.at(i + 1).at(j).push_back(val);
        val = modifier * hcal_tower3.at(k);
        if (val > 255) val = 255;
        hcal_towers.at(i + 2).at(j).push_back(val);
        val = modifier * hcal_tower4.at(k);
        if (val > 255) val = 255;
        hcal_towers.at(i + 3).at(j).push_back(val);
        assert(fg_tower1.at(k) <= 1);
        fg_towers.at(i + 0).at(j).push_back(fg_tower1.at(k));
        assert(fg_tower2.at(k) <= 1);
        fg_towers.at(i + 1).at(j).push_back(fg_tower2.at(k));
        assert(fg_tower3.at(k) <= 1);
        fg_towers.at(i + 2).at(j).push_back(fg_tower3.at(k));
        assert(fg_tower4.at(k) <= 1);
        fg_towers.at(i + 3).at(j).push_back(fg_tower4.at(k));
      }
    }
  }

  out_file << X_DIM << endl;
  out_file << Y_DIM << endl;

  for (int i = 0; i < X_DIM; ++i) {
    for (int j = 0; j < Y_DIM; ++j) {
      for (size_t k = 0; k < ecal_tower1.size(); ++k) {
        out_file << std::hex;
        assert(fg_towers.at(i).at(j).at(k) <= 1);
        assert(ecal_towers.at(i).at(j).at(k) <= 255);
        assert(hcal_towers.at(i).at(j).at(k) <= 255);
        out_file << setw(1) << setfill('X') << fg_towers.at(i).at(j).at(k);
        out_file << setw(2) << setfill('0') << ecal_towers.at(i).at(j).at(k);
        out_file << setw(2) << setfill('0') << hcal_towers.at(i).at(j).at(k);
        out_file << "\n";
      }
    }
  }

  return 0;
}
