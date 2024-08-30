#include <argv_buffer.h>
#include <gtest/gtest.h>

#include <stdexcept>

class argv_buffer_test : public ::testing::Test {
 public:
};

TEST_F(argv_buffer_test, simple1) {
  std::vector<std::string> argv;

  const char* str1 = "one";
  const char* str2 = "twojfosidjfisodjfiosdjf";
  const char* str3 =
      "threeeeeeeeeeeeeeeeeeeeeeeekjelkjwe;lkrjweklqjkqljwer;lqwjrqwjr";

  argv.push_back(str1);
  argv.push_back(str2);
  argv.push_back(str3);

  argv_buffer foo(argv);
  char** args = foo.get();

  ASSERT_STREQ(*args, str1);

  ++args;
  ASSERT_STREQ(*args, str2);

  ++args;
  ASSERT_STREQ(*args, str3);

  ++args;
  ASSERT_EQ(*args, nullptr);
}

TEST_F(argv_buffer_test, simple_2) {
  std::vector<std::string> argv;

  const char* str = ";kjfweiujeipfueiowfiuerhiurguihreuigheruighre";

  argv.push_back(str);

  argv_buffer qq(argv);

  char** args = qq.get();

  ASSERT_STREQ(*args, str);

  ++args;
  ASSERT_EQ(*args, nullptr);
}

TEST_F(argv_buffer_test, simple_3) {
  std::vector<std::string> argv;

  ASSERT_THROW(argv_buffer fq(argv), std::runtime_error);
}
