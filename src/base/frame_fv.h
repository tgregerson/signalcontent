/*
 * frame_fv.h
 *
 *  Created on: Apr 2, 2014
 *      Author: gregerso
 *
 * Define some containers for storing streams of four-value logic and methods
 * for converting between containers.
 *
 */

#ifndef SIGNAL_CONTENT_BASE_FRAME_FV_H_
#define SIGNAL_CONTENT_BASE_FRAME_FV_H_

#include <array>
#include <deque>
#include <queue>
#include <vector>

#include "four_value_logic.h"
#include "queue_fv.h"

namespace signal_content {
namespace base {

// A frame is simply an array of four-value logic. Conceptually, it is assumed
// that there will be some logical relationship between values stored in the
// same frame position across multiple frames.

template <size_t FRAME_SIZE>
using FrameFv = std::array<FourValueLogic, FRAME_SIZE>;

// A FrameQueue is a queue of frames, where each frame is the same size.
template <size_t FRAME_SIZE>
using FrameQueue = std::queue<FrameFv<FRAME_SIZE>>;

// A FrameDeque is a deque of frames, where each frame is the same size.
template <size_t FRAME_SIZE>
using FrameDeque = std::deque<FrameFv<FRAME_SIZE>>;

// A FrameVector is a vector of frames, where each frame is the same size.
template <size_t FRAME_SIZE>
using FrameVector = std::vector<FrameFv<FRAME_SIZE>>;

// Methods for constructing frame containers from a QueueFv.
// They assume the QueueFv is an R-Value, hence its contents may be
// destroyed.
template <size_t FRAME_SIZE>
FrameQueue<FRAME_SIZE> ConvertToFrameQueue(QueueFv&& q) {
  FrameQueue<FRAME_SIZE> frame_queue;
  if (q.size() % FRAME_SIZE != 0) {
    throw std::runtime_error("Queue is not a multiple of frame size.");
  }
  for (size_t frame_num = 0; frame_num * FRAME_SIZE < q.size(); ++frame_num) {
    FrameFv<FRAME_SIZE> frame;
    for (int bit = 0; bit < FRAME_SIZE; ++bit) {
      frame.at(bit) = q.front();
      q.pop();
    }
    // Frame is destroyed by move.
    frame_queue.emplace(std::move(frame));
  }
  return frame_queue;
}

template <size_t FRAME_SIZE>
FrameDeque<FRAME_SIZE> ConvertToFrameDeque(QueueFv&& q) {
  FrameDeque<FRAME_SIZE> frame_deque;
  if (q.size() % FRAME_SIZE != 0) {
    throw std::runtime_error("Queue is not a multiple of frame size.");
  }
  for (size_t frame_num = 0; frame_num * FRAME_SIZE < q.size(); ++frame_num) {
    FrameFv<FRAME_SIZE> frame;
    for (int bit = 0; bit < FRAME_SIZE; ++bit) {
      frame.at(bit) = q.front();
      q.pop();
    }
    // Frame is destroyed by move.
    frame_deque.emplace_back(std::move(frame));
  }
  return frame_deque;
}

template <size_t FRAME_SIZE>
FrameVector<FRAME_SIZE> ConvertToFrameVector(QueueFv&& q) {
  FrameVector<FRAME_SIZE> frame_vector;
  if (q.size() % FRAME_SIZE != 0) {
    throw std::runtime_error("Queue is not a multiple of frame size.");
  }
  for (size_t frame_num = 0; frame_num * FRAME_SIZE < q.size(); ++frame_num) {
    FrameFv<FRAME_SIZE> frame;
    for (int bit = 0; bit < FRAME_SIZE; ++bit) {
      frame.at(bit) = q.front();
      q.pop();
    }
    // Frame is destroyed by move.
    frame_vector.emplace_back(std::move(frame));
  }
  return frame_vector;
}

}  // base
}  // signal_content

#endif /* SIGNAL_CONTENT_BASE_FRAME_FV_H_ */
