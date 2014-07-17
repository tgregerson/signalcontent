/*
 * entropy.h
 *
 *  Created on: Jul 17, 2014
 *      Author: gregerso
 */

// Functions for computing the entropy of strings of four-value logic.

#ifndef ENTROPY_H_
#define ENTROPY_H_

#include <cassert>
#include <cmath>
#include <vector>

#include "../base/four_value_logic.h"

namespace signal_content {
namespace codec {

class ShannonEntropyAccumulator {
 public:
  typedef std::vector<base::FourValueLogic> WordType;

  ShannonEntropyAccumulator(int word_size) : word_size_(word_size) {}
  ~ShannonEntropyAccumulator() {}

  void AddSample(WordType word);
  double GetEntropy(WordType word);

 private:
  double ComputeShannonEntropy(double probability) {
    return -probability * log2(probability) -
           (1 - probability) * log2(1 - probability);
  }

  int word_size_;
  int total_words_{0};
  std::vector<int> word_counts_;
};
}
}





#endif /* ENTROPY_H_ */
