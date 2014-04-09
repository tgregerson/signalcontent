/*
 * huffman.cpp
namespace signal_content {
namespace codec {
 *
 *  Created on: Apr 7, 2014
 *      Author: gregerso
 */

#include "huffman.h"

#include "../base/macros.h"

using namespace std;

namespace signal_content {
using base::FourValueLogic;
using base::VFrameDeque;
using base::VFrameFv;
namespace codec {

HuffmanCodec::HuffmanCodec(
    const VFrameDeque& frame_deque, size_t frame_size,
    size_t symbol_bits) : frame_size_(frame_size), symbol_bits_(symbol_bits) {
  if (symbol_bits > 32) {
    throw runtime_error("Symbol size cannot exceed 32 bits.");
  }

  // Build frequency table.
  unordered_map<int, size_t> symbol_to_freq;
  for (size_t frame_num = 0; frame_num < frame_deque.size(); ++frame_num) {
    const VFrameFv& frame = frame_deque.at(frame_num);
    vector<int> symbols = FrameToSymbols(frame);
    for (int symbol : symbols) {
      auto it = symbol_to_freq.find(symbol);
      if (it == symbol_to_freq.end()) {
        symbol_to_freq.insert(std::make_pair(symbol, 1));
      } else {
        it->second = it->second + 1;
      }
    }
  }

  // Build the decoding tree. Start by creating all leaf nodes, then build the
  // tree from the bottom up.
  std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>,
                      HuffmanNodePointerGreater> freq_sorted_nodes;
  for (std::pair<int, size_t> p : symbol_to_freq) {
    // Add all symbols as leaf nodes.
    freq_sorted_nodes.push(
        new HuffmanNode(true, p.second, p.first, nullptr, nullptr, nullptr));
  }

  // Build parent nodes from leaves.
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

  // Ownership of all nodes in the tree is transferred to the root. Nodes will
  // be deleted when root is destroyed.
  code_tree_root_ = freq_sorted_nodes.top();
  freq_sorted_nodes.pop();

  // Build the code table.
  std::vector<bool> empty;
  BuildCodeTableRecursive(code_tree_root_, &empty, &symbol_to_codeword_);
}

HuffmanCodec::~HuffmanCodec() {
  if (code_tree_root_ != nullptr) {
    delete code_tree_root_;
  }
}

vector<bool> HuffmanCodec::Encode(const VFrameDeque& frame_deque) {
  vector<bool> encoded;
  for (size_t frame_num = 0; frame_num < frame_deque.size(); ++frame_num) {
    const VFrameFv& frame = frame_deque.at(frame_num);
    CHECK_EQ(frame_size_, frame.size()) << "Frame size mismatch: "
                                        << frame.size() << " " << frame_size_;
    vector<int> symbols = FrameToSymbols(frame);
    for (int symbol : symbols) {
      const vector<bool>& codeword = symbol_to_codeword_.at(symbol);
      encoded.insert(encoded.end(), codeword.begin(), codeword.end());
    }
  }
  return encoded;
}

vector<int> HuffmanCodec::Decode(const vector<bool>& bits) {
  vector<int> decoded;
  HuffmanNode* current_node = CHECK_NOTNULL(code_tree_root_);
  for (bool bit : bits) {
    current_node = bit ? current_node->right : current_node->left;
    if (current_node->is_leaf_node) {
      decoded.push_back(current_node->symbol);
      current_node = code_tree_root_;
    }
  }
  CHECK_EQ(code_tree_root_, current_node) << "Bits did not end on leaf.";
  return decoded;
}

int HuffmanCodec::FourValueBitsToSymbol(
    const FourValueLogic* fv_array, size_t num_bits) const {
  int encoded = 0;
  for (size_t bit = 0; bit < num_bits; ++bit) {
    encoded = encoded << 1;
    // Treat X, Z as zeroes.
    if (fv_array[bit] == FourValueLogic::ONE) {
      encoded |= 1;
    }
  }
  return encoded;
}

vector<int> HuffmanCodec::FrameToSymbols(const base::VFrameFv& frame) {
  vector<int> symbols;
  for (size_t bit = 0; bit < frame_size_; bit += symbol_bits_) {
    size_t bits_left_in_frame = frame_size_ - bit;
    symbols.push_back(
        (bits_left_in_frame < symbol_bits_) ?
        FourValueBitsToSymbol(&(frame[bit]), bits_left_in_frame) :
        FourValueBitsToSymbol(&(frame[bit]), symbol_bits_));
  }
  return symbols;
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
