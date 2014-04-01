/*
 * parser_interface.h
 *
 *  Created on: Mar 27, 2014
 *      Author: gregerso
 */

#ifndef SIGNAL_CONTENT_PARSER_PARSER_INTERFACE_H_
#define SIGNAL_CONTENT_PARSER_PARSER_INTERFACE_H_

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

#endif /* SIGNAL_CONTENT_PARSER_PARSER_INTERFACE_H_ */
