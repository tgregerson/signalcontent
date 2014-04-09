/*
 * queue_fv.h
 *
 *  Created on: Mar 27, 2014
 *      Author: gregerso
 *
 *  A wrapper for the queue data structure that uses four-value logic.
 */

#ifndef SIGNAL_CONTENT_BASE_QUEUE_FV_H_
#define SIGNAL_CONTENT_BASE_QUEUE_FV_H_

#include <list>
#include <queue>

#include "four_value_logic.h"

namespace signal_content {
namespace base {
typedef std::queue<FourValueLogic, std::list<FourValueLogic>> QueueFv;

// Convert a specified number of bits for a data type to a queue of four value
// logic. The bits taken are the least significant bits.
template <typename T, size_t NUM_BITS>
QueueFv QueueFvFromBits(T bits) {
  QueueFv q;
  for (int i = 0; i < NUM_BITS; ++i) {
    bool bit = bits & 0x1;
    q.push(FourValueLogicFromBool(bit));
    bits = bits >> 1;
  }
  return q;
}
}  // namespace base
}  // namespace signal_content

#endif /* SIGNAL_CONTENT_BASE_QUEUE_FV_H_ */
