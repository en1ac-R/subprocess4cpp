#include <gtest/gtest.h>
#include <subprocess.h>

#include <stdexcept>
class subprocess_test : public ::testing::Test {
 public:
  static void make_output_pipe() { subprocess::output_pipe foo; }
};

TEST_F(subprocess_test, no_args) {
  std::vector<std::string> argv;

  ASSERT_THROW(subprocess::run(argv), std::runtime_error);
}

TEST_F(subprocess_test, bad_args) {
  std::vector<std::string> argv;

  argv.push_back("/bin/ls");
  argv.push_back("/path/to/not/a/file");

  ASSERT_THROW(subprocess::run(argv), std::runtime_error);
}

TEST_F(subprocess_test, one_arg) {
  std::vector<std::string> argv;
  argv.push_back("/bin/pwd");
  subprocess::run(argv);
}

TEST_F(subprocess_test, some_args) {
  std::vector<std::string> argv;
  argv.push_back("/bin/echo");
  argv.push_back("hello,");
  argv.push_back("how");
  argv.push_back("are");
  argv.push_back("you?");

  subprocess::run(argv);
}

TEST_F(subprocess_test, f_no_exist) {
  std::vector<std::string> argv;
  argv.push_back("/bin/nothing");
  ASSERT_THROW(subprocess::run(argv), std::runtime_error);
}

TEST_F(subprocess_test, path_not_absolute) {
  std::vector<std::string> argv;
  argv.push_back("bin/echo");
  ASSERT_THROW(subprocess::run(argv), std::runtime_error);
}

TEST_F(subprocess_test, ls_pipe) {
  std::vector<std::string> argv;
  argv.push_back("/bin/ls");
  argv.push_back("|");
  argv.push_back("grep");
  argv.push_back(".");
  ASSERT_THROW(subprocess::run(argv), std::runtime_error);
}
TEST_F(subprocess_test, exit_code_bad_path) {
  std::vector<std::string> argv;
  argv.push_back("/bin/ls");
  argv.push_back("/this/is/not/a/file");

  int exit_code{};

  subprocess::run(argv, &exit_code);

  ASSERT_EQ(exit_code, 2);
}

TEST_F(subprocess_test, exit_code_true_path) {
  std::vector<std::string> argv;
  argv.push_back("/bin/ls");
  argv.push_back("/home");

  int exit_code{};

  subprocess::run(argv, &exit_code);

  ASSERT_EQ(exit_code, 0);
}

TEST_F(subprocess_test, exit_code_true_path_initialized) {
  std::vector<std::string> argv;
  argv.push_back("/bin/ls");
  argv.push_back("/home");

  int exit_code{-1};

  subprocess::run(argv, &exit_code);

  ASSERT_EQ(exit_code, 0);
}

TEST_F(subprocess_test, exit_code_bad_args) {
  std::vector<std::string> argv;
  argv.push_back("/bin/ls");
  argv.push_back("|");
  argv.push_back("grep");
  argv.push_back(".");

  int exit_code{};

  subprocess::run(argv, &exit_code);

  ASSERT_EQ(exit_code, 2);
}

TEST_F(subprocess_test, exit_code_not_valid) {
  std::vector<std::string> argv;
  argv.push_back("/bin/ls");

  ASSERT_THROW(subprocess::run(argv, nullptr), std::runtime_error);
}

TEST_F(subprocess_test, ret_out) {
  std::vector<std::string> argv;
  argv.push_back("/bin/echo");
  argv.push_back("hello");

  std::string capture_cout;
  std::string capture_cerr;

  subprocess::run(argv, &capture_cout, &capture_cerr);

  ASSERT_STREQ(capture_cout.c_str(), "hello\n");
  ASSERT_EQ(capture_cerr.size(), 0);
}

TEST_F(subprocess_test, err_out) {
  std::vector<std::string> argv;
  argv.push_back("/bin/ls");
  argv.push_back("/path/to/not/a/file");

  std::string capture_cout;
  std::string capture_cerr;

  subprocess::run(argv, &capture_cout, &capture_cerr);

  ASSERT_EQ(capture_cout.size(), 0);
  ASSERT_STREQ(capture_cerr.c_str(),
               "/bin/ls: cannot access '/path/to/not/a/file': No such file or "
               "directory\n");
}

TEST_F(subprocess_test, out_not_valid) {
  std::vector<std::string> argv;
  argv.push_back("/bin/pwd");

  std::string* capture_cout = nullptr;
  std::string* capture_cerr = nullptr;

  ASSERT_THROW(subprocess::run(argv, capture_cout, capture_cerr),
               std::runtime_error);
}

TEST_F(subprocess_test, output_pipe_test) {
  for (std::size_t i{0}; i < 1000; ++i) {
    subprocess_test::make_output_pipe();
  }
}

TEST_F(subprocess_test, thread_test) {
  std::vector<std::string> argv;
  argv.push_back("/bin/date");

  for (int i{0}; i < 10; i++) {
    subprocess::run_detached(argv);
    subprocess::run_detached(argv);
    sleep(0.5);
  }
  sleep(1);
  for (int i{0}; i < 3; i++) {
    subprocess::run_detached(argv);
  }

  sleep(30);
}
