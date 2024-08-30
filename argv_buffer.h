#ifndef ARGV_BUFFER_H
#define ARGV_BUFFER_H

#include <memory>
#include <string>
#include <vector>

class argv_buffer {
 public:
  using char_ptr_t = char*;
  explicit argv_buffer(const std::vector<std::string>& vec);
  argv_buffer(const argv_buffer& any) = delete;
  argv_buffer& operator=(const argv_buffer& any) = delete;
  argv_buffer(argv_buffer&& any) = delete;
  argv_buffer& operator=(argv_buffer&& any) = delete;

  char** get();
  ~argv_buffer();

 private:
  std::vector<std::string> buffer_;
  std::unique_ptr<char_ptr_t[]> retchar_;
};

#endif  // ARGV_BUFFER_H
