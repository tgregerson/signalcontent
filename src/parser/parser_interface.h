/*
 * parser_interface.h
 *
 *  Created on: Mar 27, 2014
 *      Author: gregerso
 */

#ifndef PARSER_INTERFACE_H_
#define PARSER_INTERFACE_H_

#include <string>

#include "../base/four_value_logic.h"

namespace signal_content {
namespace parser {
class ParserInterface {
 public:
  ParserInterface(const std::string& filename);
  virtual ~ParserInterface();

  virtual base::queue_fv ParseAll() = 0;

 protected:

 private:
  file_;
};
}  // parser
}  // signal_content

#endif /* PARSER_INTERFACE_H_ */
