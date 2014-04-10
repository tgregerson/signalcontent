/*
 * lzw.h
 *
 *  Created on: Apr 7, 2014
 *      Author: gregerso
 */

#include <array>
#include <memory>

#include "queue_fv.h"

#ifndef LZW_H_
#define LZW_H_

namespace signal_content {
namespace codec {

class LzwCodec {
 public:

 private:
  struct Node256Ary {
    int codeword{-1};
    // Initializes all to nullptr.
    std::array<std::unique_ptr<Node256Ary>, 256> children;
  };

  struct NodeBinary {
    int codeword{-1};
    // Initializes all to nullptr.
    std::array<std::unique_ptr<NodeBinary>, 2> children;
  };

  // Populates dictionaries with single symbol to codeword mappings.
  void PopulateInitialMappings();

  // Peels off as many bits as possible until a codeword is found.
  // Note that the number of bits removed will be a multiple of 8.
  int GetCodeword256(base::QueueFv* queue_fv);
  int GetCodewordBinary(base::QueueFv* queue_fv);

  // Code tree for encoding symbol stream to codewords.
  std::unique_ptr<Node256Ary> code_tree_root_;
  std::unique_ptr<NodeBinary> code_tree_root_binary_;
  // Dictionary for decoding codewords.
  std::array<std::vector<char>, 4096> codeword_to_symbol_;
};

}  // namespace codec
}  // namespace signal_content

#endif /* LZW_H_ */
