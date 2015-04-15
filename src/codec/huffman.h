/*
 * huffman.h
 *
 *  Created on: Apr 3, 2014
 *      Author: gregerso
 */

#ifndef SIGNAL_CONTENT_CODEC_HUFFMAN_H_
#define SIGNAL_CONTENT_CODEC_HUFFMAN_H_

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

struct HuffmanNode;

class HuffmanCodec {
 public:
  HuffmanCodec(const base::VFrameDeque& frame_deque, size_t symbol_bits);
  ~HuffmanCodec();

  std::vector<bool> Encode(const base::VFrameDeque& frames);
  std::vector<bool> EncodeFrame(const base::VFrameFv& frame);
  std::vector<int> Decode(const std::vector<bool>& bits);
  void PrintCodeTable() const;
  void PrintCompressionData() const;

 private:
  // Represent a series of four-value bits as a multi-bit symbol.
  int FourValueBitsToSymbol(
      const base::FourValueLogic* fv_array, size_t num_bits) const;

  // Extract integer symbols for an entire frame.
  std::vector<int> FrameToSymbols(const base::VFrameFv& frame);

  void BuildCodeTableRecursive(
      const HuffmanNode* node,
      std::vector<bool>* code,
      std::unordered_map<int, std::vector<bool>>* table);

  size_t frame_size_;
  size_t symbol_bits_;
  std::unordered_map<int, std::vector<bool>> symbol_to_codeword_;
  std::unordered_map<int, size_t> symbol_to_freq_;
  HuffmanNode* code_tree_root_{nullptr};
};

struct HuffmanNode {
  HuffmanNode(bool leaf, int freq, int sym, HuffmanNode* l, HuffmanNode* r,
              HuffmanNode* p)
      : is_leaf_node(leaf), frequency(freq), symbol(sym), left(l), right(r),
        parent(p) {}
  ~HuffmanNode() {
    if (left != nullptr) {
      delete left;
    }
    if (right != nullptr) {
      delete right;
    }
  }

  bool is_leaf_node{true};
  int frequency{0};
  int symbol{0};
  HuffmanNode* left{nullptr};
  HuffmanNode* right{nullptr};
  HuffmanNode* parent{nullptr};
};

// Functor for priority queue.
struct HuffmanNodePointerGreater {
  bool operator() (const HuffmanNode* a, const HuffmanNode* b) {
    return CHECK_NOTNULL(a)->frequency > CHECK_NOTNULL(b)->frequency;
  }
};

}  // codec
}  // signal_content

#endif /* SIGNAL_CONTENT_CODEC_HUFFMAN_H_ */
