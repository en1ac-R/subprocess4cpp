#include <subprocess.h>

#include <iostream>  // std::cerr

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Unexpected argc: " << argc << '\n';
    return EXIT_FAILURE;
  }

  const char* exe{argv[1]};
  std::cout << "Run executable: " << exe << '\n';

  std::vector<std::string> args{exe};

  std::string capture_cout;
  std::string capture_cerr;

  subprocess::run(args, &capture_cout, &capture_cerr);

  std::cout << "cout size: " << capture_cout.size() << '\n'
            << "cerr size: " << capture_cerr.size() << '\n';

  if (capture_cout.size() != capture_cerr.size()) {
    std::cerr << "Buffer sizes are not consistent\n";
    return EXIT_FAILURE;
  }
}
