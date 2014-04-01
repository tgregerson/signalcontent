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

#include <queue>

#include "four_value_logic.h"

namespace signal_content {
namespace base {
typedef std::queue<FourValueLogic> queue_fv;
}  // namespace base
}  // namespace signal_content

#endif /* SIGNAL_CONTENT_BASE_QUEUE_FV_H_ */
