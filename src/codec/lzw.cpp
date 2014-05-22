/*
 * lzw.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: gregerso
 */

#include "lzw.h"

#include <cassert>

#include <stdexcept>

#include "../base/frame_fv.h"
#include "../base/macros.h"
#include "../base/queue_fv.h"

using namespace std;

namespace signal_content {
using base::QueueFv;
using base::VFrameDeque;

namespace codec {

vector<int> LzwCodec::Encode(const QueueFv& bit_stream) {
  assert(next_codeword_slot_ > 255); // Check that dictionary has been populated.
  vector<int> encoded;
  QueueFv qfv = bit_stream;
  while (!qfv.empty()) {
    encoded.push_back(GetCodeword256(&qfv));
  }
  return encoded;
}

vector<bool> LzwCodec::Decode(const std::vector<int>& codewords) {
  vector<bool> decoded;
  vector<unsigned char> symbols;
  for (int codeword : codewords) {
    symbols.insert(symbols.end(),
                   codeword_to_symbol_.at(codeword).begin(),
                   codeword_to_symbol_.at(codeword).end());
  }
  for (unsigned char symbol : symbols) {
    for (int mask = 0x0800; mask != 0; mask = mask >> 1) {
      decoded.push_back(symbol & mask);
    }
  }
  return decoded;
}

// Currently only works for 256-ary nodes.
void LzwCodec::PopulateDictionary(const base::QueueFv& queue_fv) {
  PopulateInitialMappings();
  Node256Ary* n256 = code_tree_root_.get();
  vector<unsigned char> symbol_string;
  base::QueueFv qfv_copy = queue_fv;
  while (!qfv_copy.empty() && next_codeword_slot_ < 4096) {
    unsigned char symbol = Get8Bits(&qfv_copy);
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
    next_codeword_slot_++;
  }
}

int LzwCodec::GetCodeword256(QueueFv* bit_queue) {
  if (CHECK_NOTNULL(bit_queue)->size() < 8) {
    throw std::runtime_error("Binary stream contained less than minimum "
                             "number of symbol bits");
  }
  Node256Ary* node = CHECK_NOTNULL(code_tree_root_.get());
  while (!bit_queue->empty()) {
    unsigned char symbol = Get8Bits(bit_queue);
    Node256Ary* next_node = node->children.at(symbol).get();
    if (next_node == nullptr) {
      return node->codeword;
    } else {
      node = next_node;
    }
  }
  return node->codeword;
}

unsigned char LzwCodec::Get8Bits(QueueFv* bit_queue) {
  unsigned char ret = 0;
  for (int i = 0; i < 8 && !bit_queue->empty(); ++i) {
    base::FourValueLogic fvl_bit = bit_queue->front();
    bit_queue->pop();
    bool bit = fvl_bit == base::FourValueLogic::ONE ? 1 : 0;
    ret = (ret << 1) | bit;
  }
  return ret;
}

}  // namespace codec
}  // namespace signal_content



