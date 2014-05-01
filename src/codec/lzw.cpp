/*
 * lzw.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: gregerso
 */

#include "lzw.h"

#include <stdexcept>

namespace signal_content {
using base::QueueFv;

namespace codec {

void LzwCodec::PopulateInitialMappings() {
  // Add all 8-bit symbols
  for (int symbol = 0; symbol < 256; ++symbol) {
    codeword_to_symbol_[symbol].push_back(short(symbol));
    // Will add to symbol codeword tree, since not yet present.
    QueueFv q = base::QueueFvFromBits<int, 8>(symbol);

    // TODO populate codeword tree
  }
}

int LzwCodec::GetCodewordBinary(QueueFv* bit_queue) {
  if (CHECK_NOTNULL(bit_queue)->size() < 8) {
    throw std::runtime_error("Binary stream contained less than minimum "
                             "number of symbol bits");
  }
  NodeBinary* node = CHECK_NOTNULL(code_tree_root_binary_.get());
  while (true) {
    if (bit_queue->empty()) {
      return node->codeword;
    }
    // Treats X and Z as 0.
    int index = bit_queue->front() == base::FourValueLogic::ONE ? 1 : 0;
    NodeBinary* next_node = node->children[index];
    if (next_node == nullptr) {
      return node->codeword;
    } else {
      bit_queue->pop();
      node = next_node;
    }
  }
  return -1;  // Should not be reached.
}

}  // namespace codec
}  // namespace signal_content



