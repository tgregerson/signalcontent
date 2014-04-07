/*
 * fixed_frame_huffman.h
 *
 *  Created on: Apr 3, 2014
 *      Author: gregerso
 */

#ifndef SIGNAL_CONTENT_CODEC_FIXED_FRAME_HUFFMAN_H_
#define SIGNAL_CONTENT_CODEC_FIXED_FRAME_HUFFMAN_H_

#include <queue>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "../base/four_value_logic.h"
#include "../base/frame_fv.h"
#include "../base/macros.h"

namespace signal_content {
namespace codec {

struct FixedFrameHuffmanNode {
  FixedFrameHuffmanNode(bool leaf, int freq, int sym, FixedFrameHuffmanNode* l,
                        FixedFrameHuffmanNode* r, HuffmanNode* p)
      : is_leaf_node(leaf), frequency(freq), symbol(sym), left(l), right(r),
        parent(p) {}
  bool is_leaf_node{true};
  int frequency{0};
  int symbol{0};
  FixedFrameHuffmanNode* left{nullptr};
  FixedFrameHuffmanNode* right{nullptr};
  FixedFrameHuffmanNode* parent{nullptr};
};

template <size_t FRAME_SIZE>
class FixedFrameHuffmanCodec {
 public:
  FixedFrameHuffmanCodec(const base::FrameDeque<FRAME_SIZE>& frame_deque,
                         size_t symbol_bits);
 private:
  // Encodes a four-value symbol into binary.
  int FourValueSymbolToInt(
      const base::FourValueLogic* fv_array, size_t num_bits) const;

  void BuildCodeTableRecursive(
      const FixedFrameHuffmanNode* node,
      std::vector<bool>* code,
      std::unordered_map<int, std::vector<bool>>* table);

  std::unordered_map<int, std::vector<bool>> symbol_to_codeword_;
  FixedFrameHuffmanNode* code_tree_root_{nullptr};
};

// Functor for priority queue.
struct FixedFrameHuffmanNodePointerGreater {
  bool operator() (const FixedFrameHuffmanNode* a,
                   const FixedFrameHuffmanNode* b) {
    return CHECK_NOTNULL(a)->frequency > CHECK_NOTNULL(b)->frequency;
  }
};

template <size_t FRAME_SIZE>
FixedFrameHuffmanCodec<FRAME_SIZE>::FixedFrameHuffmanCodec(
    const base::FrameDeque<FRAME_SIZE>& frame_deque, size_t symbol_bits) {
  if (symbol_bits > 32) {
    throw std::runtime_error("Symbol size cannot exceed 32 bits.");
  }

  // Build frequency table.
  std::unordered_map<int, size_t> symbol_to_freq;
  for (size_t frame_num = 0; frame_num < frame_deque.size(); ++frame_num) {
    const base::FrameFv<FRAME_SIZE>& frame = frame_deque.at(frame_num);
    for (size_t bit = 0; bit < FRAME_SIZE; bit += symbol_bits) {
      size_t bits_left_in_frame = FRAME_SIZE - bit;
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
  std::priority_queue<FixedFrameHuffmanNode*,
                      std::vector<FixedFrameHuffmanNode*>,
                      FixedFrameHuffmanNodePointerGreater> freq_sorted_nodes;
  for (std::pair<int, size_t> p : symbol_to_freq) {
    // Add all symbols as leaf nodes.
    freq_sorted_nodes.push(
        new FixedFrameHuffmanNode(true, p.second, p.first, nullptr, nullptr, nullptr));
  }

  while (freq_sorted_nodes.size() > 1) {
    FixedFrameHuffmanNode* n1 = freq_sorted_nodes.top();
    freq_sorted_nodes.pop();
    FixedFrameHuffmanNode* n2 = freq_sorted_nodes.top();
    freq_sorted_nodes.pop();
    FixedFrameHuffmanNode* parent = new FixedFrameHuffmanNode(
        false, n1->frequency + n2->frequency, 0, n1, n2, nullptr);
    n1->parent = parent;
    n2->parent = parent;
    freq_sorted_nodes.push(parent);
  }
  assert(freq_sorted_nodes.size() == 1);

  code_tree_root_ = freq_sorted_nodes.pop();

  // Build the code table.
  std::vector<bool> empty;
  BuildCodeTableRecursive(code_tree_root_, &empty, &symbol_to_codeword_);
}

template <size_t FRAME_SIZE>
int FixedFrameHuffmanCodec<FRAME_SIZE>::FourValueSymbolToInt(
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

template <size_t FRAME_SIZE>
void FixedFrameHuffmanCodec<FRAME_SIZE>::BuildCodeTableRecursive(
    const FixedFrameHuffmanNode* node,
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

}  // codec
}  // signal_content

#endif /* SIGNAL_CONTENT_CODEC_FIXED_FRAME_HUFFMAN_H_ */
