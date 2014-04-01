/*
 * macros.h
 *
 *  Created on: Apr 1, 2014
 *      Author: gregerso
 *
 *  Convenience macros.
 */

#ifndef SIGNAL_CONTENT_BASE_MACROS_H_
#define SIGNAL_CONTENT_BASE_MACROS_H_

#include <cassert>

template<typename T> inline T* CHECK_NOTNULL(T* ptr) { assert(ptr != nullptr); return ptr; }

#endif /* SIGNAL_CONTENT_BASE_MACROS_H_ */
