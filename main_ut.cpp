#include <gtest/gtest.h>  // testing::InitGoogleTest

#include <iostream>  // std::cerr

auto main(int argc, char** argv) -> int {
  testing::InitGoogleTest(&argc, argv);

  if (argc == 1) {
    return RUN_ALL_TESTS();
  }

  std::cerr << "argc: " << argc << '\n' << "Unexpected arguments:\n";
  for (int i{1}; i < argc; ++i) {
    std::cerr << argv[i] << '\n';
  }

  return EXIT_FAILURE;
}
