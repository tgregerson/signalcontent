#ifndef SIGNAL_CONTENT_BASE_FOUR_VALUE_LOGIC_H_
#define SIGNAL_CONTENT_BASE_FOUR_VALUE_LOGIC_H_

namespace signal_content {
namespace base {
  enum class FourValueLogic : char {
    ZERO = 0,
    ONE = 1,
    X = 2,
    Z = 3
  };

  FourValueLogic FourValueLogicFromBool(bool bit) {
    return bit ? FourValueLogic::ONE : FourValueLogic::ZERO;
  }
}  // namespace base
}  // namespace four_value_logic

#endif /* SIGNAL_CONTENT_BASE_FOUR_VALUE_LOGIC_H_ */
