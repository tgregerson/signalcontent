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

#include <map>
#include <utility>
#include <vector>

#include "../base/four_value_logic.h"

namespace signal_content {
namespace codec {

template <typename WordType>
class ShannonEntropyAccumulator {
 public:

  ShannonEntropyAccumulator() {}
  ~ShannonEntropyAccumulator() {}

  void AddSample(WordType word) {
    auto it = word_counts_.find(word);
    if (it == word_counts_.end()) {
      word_counts_.insert(make_pair(word, 1));
    } else {
      it->second = it->second + 1;
    }
    ++total_words_;
  }

  double GetEntropy() {
    double entropy = 0.0;
    for (auto word_freq_pair : word_counts_) {
      entropy += GetWordEntropy(word_freq_pair.first);
    }
    return entropy;
  }

  double GetWordEntropy(WordType word) {
    if (total_words_ <= 0) {
      return 0.0;
    } else {
      int count = word_counts_[word];
      double probability = ((double)count)/((double)total_words_);
      return ComputeShannonWordEntropy(probability);
    }
  }

 private:
  double ComputeShannonWordEntropy(double probability) {
    return -probability * log2(probability);
  }

  int total_words_{0};
  std::map<WordType, int> word_counts_;
};
}
}





#endif /* ENTROPY_H_ */
