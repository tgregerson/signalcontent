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

  inline FourValueLogic FourValueLogicFromBool(bool bit) {
    return bit ? FourValueLogic::ONE : FourValueLogic::ZERO;
  }

  inline bool FourValueLogicToBool(FourValueLogic fvl) {
    return fvl != FourValueLogic::ONE;
  }

  inline char FVLtoChar(FourValueLogic l) {
    switch (l) {
      case FourValueLogic::ZERO: return '0';
      case FourValueLogic::ONE: return '1';
      case FourValueLogic::X: return 'X';
      case FourValueLogic::Z: return 'Z';
      default: return '?';
    }
  }

  // Assumes vector starts with highest-order bit.
  inline unsigned int FVLtoUInt(std::vector<FourValueLogic> word) {
    int val = 0;
    for (FourValueLogic bit : word) {
      val = val << 1;
      if (bit == FourValueLogic::ONE) {
        ++val;
      }
    }
    return val;
  }
}  // namespace base
}  // namespace four_value_logic

#endif /* SIGNAL_CONTENT_BASE_FOUR_VALUE_LOGIC_H_ */
