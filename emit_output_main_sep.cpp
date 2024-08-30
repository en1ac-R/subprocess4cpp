#include <iostream>  // std::cout

int main() {
  static constexpr std::size_t n{100000};
  for (std::size_t i{0}; i < n; ++i) {
    std::cout << "std::cout output [" << i << "]\n";
  }
  for (std::size_t i{0}; i < n; ++i) {
    std::cerr << "std::cerr output [" << i << "]\n";
  }
}
