/*
 * lzw.h
 *
 *  Created on: Apr 7, 2014
 *      Author: gregerso
 */

#include <array>
#include <memory>

#include "../base/queue_fv.h"

#ifndef LZW_H_
#define LZW_H_

namespace signal_content {
namespace codec {

class LzwCodec {
 public:
  LzwCodec() : code_tree_root_(new Node256Ary(-1)),
               code_tree_root_binary_(new NodeBinary(-1)) {}

  // Assigns sequences of symbols from 'queue_fv' to codewords in the
  // the dictionary. Symbols are considered to be 8 bits, and codewords are
  // 12 bits.
  void PopulateDictionary(base::QueueFv* queue_fv);

 private:
  struct Node256Ary {
    Node256Ary(int cw) : codeword(cw) {}
    int codeword{-1};
    // Initializes all to nullptr.
    std::array<std::unique_ptr<Node256Ary>, 256> children;
  };

  struct NodeBinary {
    NodeBinary(int cw) : codeword(cw) {}
    int codeword{-1};
    // Initializes all to nullptr.
    std::array<std::unique_ptr<NodeBinary>, 2> children;
  };

  // Populates dictionaries with single symbol to codeword mappings.
  void PopulateInitialMappings();

  // Peels off as many bits as possible until a codeword is found.
  // Note that the number of bits removed will be a multiple of 8.
  // This method is meant to be called after the dictionary is finalized. It
  // does not insert into the dictionary.
  int GetCodeword256(base::QueueFv* queue_fv);
  int GetCodewordBinary(base::QueueFv* queue_fv);

  // Peels off 8 bits from queue (or all remaining bits if less than 8).
  char Get8Bits(base::QueueFv* queue_fv);

  // Code tree for encoding symbol stream to codewords.
  std::unique_ptr<Node256Ary> code_tree_root_;
  std::unique_ptr<NodeBinary> code_tree_root_binary_;
  // Dictionary for decoding codewords.
  std::array<std::vector<char>, 4096> codeword_to_symbol_;
  int next_codeword_slot_{0};
};

}  // namespace codec
}  // namespace signal_content

#endif /* LZW_H_ */
