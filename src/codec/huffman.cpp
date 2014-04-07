/*
 * huffman.cpp
namespace signal_content {
namespace codec {
 *
 *  Created on: Apr 7, 2014
 *      Author: gregerso
 */

#include "huffman.h"

namespace signal_content {
namespace codec {

HuffmanCodec::HuffmanCodec(
    const base::VFrameDeque& frame_deque, size_t frame_size,
    size_t symbol_bits) : frame_size_(frame_size) {
  if (symbol_bits > 32) {
    throw std::runtime_error("Symbol size cannot exceed 32 bits.");
  }

  // Build frequency table.
  std::unordered_map<int, size_t> symbol_to_freq;
  for (size_t frame_num = 0; frame_num < frame_deque.size(); ++frame_num) {
    const base::VFrameFv& frame = frame_deque.at(frame_num);
    for (size_t bit = 0; bit < frame_size_; bit += symbol_bits) {
      size_t bits_left_in_frame = frame_size_ - bit;
      int encoded = (bits_left_in_frame < symbol_bits) ?
          FourValueSymbolToInt(&frame[bit], bits_left_in_frame) :
          FourValueSymbolToInt(&frame[bit], symbol_bits);
      auto it = symbol_to_freq.find(encoded);
      if (it == symbol_to_freq.end()) {
        symbol_to_freq.insert(std::make_pair(encoded, 1));
      } else {
        it->second = it->second + 1;
      }
    }
  }

  // Build the decoding tree.
  std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>,
                      HuffmanNodePointerGreater> freq_sorted_nodes;
  for (std::pair<int, size_t> p : symbol_to_freq) {
    // Add all symbols as leaf nodes.
    freq_sorted_nodes.push(
        new HuffmanNode(true, p.second, p.first, nullptr, nullptr, nullptr));
  }

  while (freq_sorted_nodes.size() > 1) {
    HuffmanNode* n1 = freq_sorted_nodes.top();
    freq_sorted_nodes.pop();
    HuffmanNode* n2 = freq_sorted_nodes.top();
    freq_sorted_nodes.pop();
    HuffmanNode* parent = new HuffmanNode(
        false, n1->frequency + n2->frequency, 0, n1, n2, nullptr);
    n1->parent = parent;
    n2->parent = parent;
    freq_sorted_nodes.push(parent);
  }
  assert(freq_sorted_nodes.size() == 1);

  code_tree_root_ = freq_sorted_nodes.top();
  freq_sorted_nodes.pop();

  // Build the code table.
  std::vector<bool> empty;
  BuildCodeTableRecursive(code_tree_root_, &empty, &symbol_to_codeword_);
}

int HuffmanCodec::FourValueSymbolToInt(
    const base::FourValueLogic* fv_array, size_t num_bits) const {
  int encoded = 0;
  for (size_t bit = 0; bit < num_bits; ++bit) {
    encoded = encoded << 1;
    // Treat X, Z as zeroes.
    if (fv_array[bit] == base::FourValueLogic::ONE) {
      encoded |= 1;
    }
  }
  return encoded;
}

void HuffmanCodec::BuildCodeTableRecursive(
    const HuffmanNode* node,
    std::vector<bool>* code,
    std::unordered_map<int, std::vector<bool>>* table) {
  if (CHECK_NOTNULL(node)->is_leaf_node) {
    table->insert(std::make_pair(node->symbol, *code));
  } else {
    code->push_back(false);
    BuildCodeTableRecursive(node->left, code, table);
    code->pop_back();
    code->push_back(true);
    BuildCodeTableRecursive(node->right, code, table);
    code->pop_back();
  }
}

}  // namespace codec
}  // namespace signal_content
