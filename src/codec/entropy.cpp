/*
 * entropy.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: gregerso
 */

#include "entropy.h"

#include "../base/macros.h"

using namespace std;
using signal_content::base::FourValueLogic;

namespace signal_content {
namespace codec {

  void ShannonEntropyAccumulator::AddSample(WordType word) {
    // TODO: Handle X's and Z's.
    int key = base::FVLtoUInt(word);
    ++word_counts_[key];
    ++total_words_;
  }

  double ShannonEntropyAccumulator::GetEntropy(WordType word) {
    int key = base::FVLtoUInt(word);
    int count = word_counts_[key];
    double probability = ((double)count)/((double)total_words_);
    return ComputeShannonEntropy(probability);
  }

}  // namespace codec
}  // namespace signal_content




