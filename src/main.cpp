#include "sequent.hpp"
#include <iostream>
#include <memory>
#include <vector>

int main() {
  Prop a = std::make_shared<Var>("A"); // 명제 상수 도입
  auto id = Identity(a);
  Sequent idSeq = id.apply({});
  std::cout << idSeq.toString() << '\n';

  auto notr = NotR();
  Sequent notrSeq = notr.apply({idSeq});
  std::cout << notrSeq.toString() << '\n';

  auto orr2 = OrR_2(a);
  Sequent orr2Seq = orr2.apply({notrSeq});
  std::cout << orr2Seq.toString() << '\n';

  auto permr = PermutationR(0);
  Sequent permrSeq = permr.apply({orr2Seq});
  std::cout << permrSeq.toString() << '\n';

  auto orr1 = OrR_1(std::make_shared<Not>(a));
  Sequent orr1Seq = orr1.apply({permrSeq});
  std::cout << orr1Seq.toString() << '\n';

  auto contrr = ContractionR();
  Sequent contrrSeq = contrr.apply({orr1Seq});
  std::cout << contrrSeq.toString() << '\n';

  return 0;
}
