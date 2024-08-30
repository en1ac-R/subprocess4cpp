#include <argv_buffer.h>

#include <stdexcept>

argv_buffer::argv_buffer(const std::vector<std::string>& vec) : buffer_(vec) {
  if (vec.size() == 0) throw std::runtime_error("At least argv[0] expected");
  retchar_ = std::make_unique<char_ptr_t[]>(buffer_.size() + 1);

  for (std::size_t i = 0; i < buffer_.size(); ++i) {
    retchar_[i] = buffer_[i].data();
  }

  retchar_[buffer_.size()] = nullptr;
}

char** argv_buffer::get() { return retchar_.get(); }

argv_buffer::~argv_buffer() = default;
