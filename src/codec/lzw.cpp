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
    GetCodewordBinary(&q);
  }
}

int LzwCodec::GetCodewordBinary(QueueFv* bit_queue) {
  if (CHECK_NOTNULL(bit_queue)->size() < 8) {
    throw std::runtime_error("Binary stream contained less than minimum "
                             "number of symbol bits");
  }
  NodeBinary* node = CHECK_NOTNULL(code_tree_root_binary_.get());
  while (true) {
    // TODO Complete
  }
  return 0;
}

}  // namespace codec
}  // namespace signal_content



