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
  HuffmanCodec(const base::VFrameDeque& frame_deque,
               size_t frame_size,
               size_t symbol_bits);
 private:
  // Encodes a four-value symbol into binary.
  int FourValueSymbolToInt(
      const base::FourValueLogic* fv_array, size_t num_bits) const;

  void BuildCodeTableRecursive(
      const HuffmanNode* node,
      std::vector<bool>* code,
      std::unordered_map<int, std::vector<bool>>* table);

  size_t frame_size_;
  std::unordered_map<int, std::vector<bool>> symbol_to_codeword_;
  HuffmanNode* code_tree_root_{nullptr};
};

struct HuffmanNode {
  HuffmanNode(bool leaf, int freq, int sym, HuffmanNode* l, HuffmanNode* r,
              HuffmanNode* p)
      : is_leaf_node(leaf), frequency(freq), symbol(sym), left(l), right(r),
        parent(p) {}
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
