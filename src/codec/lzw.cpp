/*
 * lzw.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: gregerso
 */

#include "lzw.h"

#include <cassert>

#include <stdexcept>

#include "../base/macros.h"

using namespace std;

namespace signal_content {
using base::QueueFv;

namespace codec {

// Currently only works for 256-ary nodes.
void LzwCodec::PopulateDictionary(base::QueueFv* queue_fv) {
  PopulateInitialMappings();
  Node256Ary* n256 = code_tree_root_.get();
  vector<char> symbol_string;
  while (!queue_fv->empty() && next_codeword_slot_ < 4096) {
    char symbol = Get8Bits(queue_fv);
    symbol_string.push_back(symbol);
    Node256Ary* next = n256->children.at(symbol).get();
    if (next != nullptr) {
      n256 = next;
    } else {
      // Add new mapping between symbol string and codeword.
      codeword_to_symbol_.at(next_codeword_slot_) = symbol_string;
      symbol_string.clear();
      n256->children.at(symbol).reset(new Node256Ary(next_codeword_slot_++));
      n256 = code_tree_root_.get();
    }
  }
}

void LzwCodec::PopulateInitialMappings() {
  // Add all 8-bit symbols
  assert(next_codeword_slot_ == 0);
  for (int symbol = 0; symbol < 256; ++symbol) {
    codeword_to_symbol_.at(symbol).push_back(short(symbol));
    // Will add to symbol codeword tree, since not yet present.
    QueueFv q = base::QueueFvFromBits<int, 8>(symbol);

    code_tree_root_->children.at(symbol).reset(
        new Node256Ary(next_codeword_slot_));
    code_tree_root_binary_->children.at(symbol).reset(
        new NodeBinary(next_codeword_slot_));
    next_codeword_slot_++;
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
    NodeBinary* next_node = node->children[index].get();
    if (next_node == nullptr) {
      return node->codeword;
    } else {
      bit_queue->pop();
      node = next_node;
    }
  }
  return -1;  // Should not be reached.
}

int LzwCodec::GetCodeword256(QueueFv* bit_queue) {
  if (CHECK_NOTNULL(bit_queue)->size() < 8) {
    throw std::runtime_error("Binary stream contained less than minimum "
                             "number of symbol bits");
  }
  Node256Ary* node = CHECK_NOTNULL(code_tree_root_.get());
  while (true) {
    char symbol = Get8Bits(bit_queue);
    Node256Ary* next_node = node->children.at(symbol).get();
    if (next_node == nullptr) {
      return node->codeword;
    } else {
      node = next_node;
    }
  }
  return -1;  // Should not be reached.
}

char LzwCodec::Get8Bits(QueueFv* bit_queue) {
  char ret = 0;
  for (int i = 0; i < 8 && !bit_queue->empty(); ++i) {
    base::FourValueLogic fvl_bit = bit_queue->front();
    bit_queue->pop();
    bool bit = fvl_bit == base::FourValueLogic::ONE ? 1 : 0;
    ret = (ret << 1) & bit;
  }
  return ret;
}

}  // namespace codec
}  // namespace signal_content



