#include <argv_buffer.h>
#include <errno.h>
#include <poll.h>
#include <spawn.h>
#include <subprocess.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>

void subprocess::thread_exe::thread_func() {
  std::vector<pid_t> temp;
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    {
      std::lock_guard<std::mutex> lock(lock_);
      temp.insert(temp.end(), pids_.begin(), pids_.end());
      pids_.clear();
    }
    for (auto it = temp.begin(); it != temp.end();) {
      int status;
      pid_t ret;
      ret = waitpid(*it, &status, WNOHANG);

      if (ret == -1) {
        throw std::runtime_error("wait_error");
      }

      if (ret == 0) {
        it++;
      } else if (ret == *it) {
        it = temp.erase(it);
      } else {
        throw std::runtime_error("wait_error");
      }
    }
  }
}

void subprocess::thread_exe::go_thread(pid_t& pid) {
  std::lock_guard<std::mutex> lock(lock_);
  pids_.push_back(pid);
  if (thread_ == nullptr) {
    thread_ = std::make_unique<std::thread>(thread_func);
    thread_->detach();
    thread_go_ = true;
  }
}

std::string subprocess::verify_exe(const std::vector<std::string>& args) {
  if (args.empty()) {
    throw std::runtime_error("At least argv[0] expected");
  }
  namespace fs = std::filesystem;
  const fs::path p{args[0]};

  // Проверка существования файла
  if (!fs::exists(p)) {
    throw std::runtime_error("the_file_does_not_exist");
  }
  // Проверка на то, что файл является regular
  if (!fs::is_regular_file(p)) {
    throw std::runtime_error("the_file_is_not_a_regular_file");
  }
  // Проверка на то, что путь к файлу является абсолютным
  if (!p.is_absolute()) {
    throw std::runtime_error("the_file_path_is_not_absolute");
  }

  return args[0];
}

subprocess::output_pipe::output_pipe() {
  int pipes[2];
  if (pipe(pipes) == -1) throw std::runtime_error("pipe_error");
  fd_write_ = pipes[1];
  fd_read_ = pipes[0];
}

int subprocess::output_pipe::fd_write() const { return fd_write_; }

int subprocess::output_pipe::fd_read() const { return fd_read_; }

void subprocess::output_pipe::close_fd_write() {
  if (fd_write_ == -1) return;
  if (close(fd_write_) != 0) {
    std::cerr << "close_pipe_error" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  fd_write_ = -1;
}

void subprocess::output_pipe::close_fd_read() {
  if (fd_read_ == -1) return;
  if (close(fd_read_) != 0) {
    std::cerr << "close_pipe_error" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  fd_read_ = -1;
}

subprocess::output_pipe::~output_pipe() {
  close_fd_write();
  close_fd_read();
}

void subprocess::run(const std::vector<std::string>& args, int* exit_code) {
  const std::string exe{verify_exe(args)};

  if (exit_code == nullptr) {
    throw std::runtime_error("exit_code_error");
  }

  argv_buffer buf(args);
  pid_t pid;
  int status;

  if (posix_spawn(&pid, exe.c_str(), nullptr, nullptr, buf.get(), nullptr) !=
      0) {
    throw std::runtime_error("posix_spawn_error");
  }
  // Если posix_spawn выполнился без ошибок, то ожидаем процесс
  if (waitpid(pid, &status, 0) != pid) {
    throw std::runtime_error("pid_error");
  }
  // Проверяем, нормально ли завершился процесс
  if (WIFEXITED(status) == 0) {
    throw std::runtime_error("exit_error");
  }

  *exit_code = WEXITSTATUS(status);
}

void subprocess::run(const std::vector<std::string>& args) {
  int exit_code;

  run(args, &exit_code);

  if (exit_code != 0) {
    throw std::runtime_error("cmd_error");
  }
}

void subprocess::run(const std::vector<std::string>& args,
                     std::string* capture_cout, std::string* capture_cerr) {
  const std::string exe{verify_exe(args)};
  if (capture_cout == nullptr || capture_cerr == nullptr) {
    throw std::runtime_error("out_not_valid");
  }

  subprocess::output_pipe stdout_pipe;
  subprocess::output_pipe stderr_pipe;

  posix_spawn_file_actions_t file_actions;
  if (posix_spawn_file_actions_init(&file_actions) != 0) {
    throw std::runtime_error("posix_spawn_file_actions_init_error");
  }

  posix_spawn_file_actions_addclose(&file_actions, stdout_pipe.fd_read());
  posix_spawn_file_actions_addclose(&file_actions, stderr_pipe.fd_read());

  if (posix_spawn_file_actions_adddup2(&file_actions, stdout_pipe.fd_write(),
                                       STDOUT_FILENO) != 0) {
    throw std::runtime_error("posix_spawn_file_actions_adddup2_error");
  }
  if (posix_spawn_file_actions_adddup2(&file_actions, stderr_pipe.fd_write(),
                                       STDERR_FILENO) != 0) {
    throw std::runtime_error("posix_spawn_file_actions_adddup2_error");
  }

  argv_buffer buf(args);
  pid_t pid;

  if (posix_spawn(&pid, exe.c_str(), &file_actions, nullptr, buf.get(),
                  nullptr) != 0) {
    throw std::runtime_error("posix_spawn_error");
  }

  static constexpr std::size_t buf_size{1024};
  char buffer[buf_size];
  char errbuff[buf_size];
  pollfd plist[]{{stdout_pipe.fd_read(), POLLIN, 0},
                 {stderr_pipe.fd_read(), POLLIN, 0}};

  static constexpr std::size_t plist_size{2};
  static constexpr int timeout{-1};

  stdout_pipe.close_fd_write();
  stderr_pipe.close_fd_write();

  while (true) {
    bool stop{true};

    const int rval{poll(plist, plist_size, timeout)};
    if (rval <= 0) {
      throw std::runtime_error("poll_error");
    }

    if (plist[0].revents & POLLIN) {
      const ssize_t size = read(stdout_pipe.fd_read(), &buffer, buf_size);
      if (size <= 0) throw std::runtime_error("read_stdout_error");
      capture_cout->append(buffer, size);
      plist[0].revents &= ~POLLIN;
    }
    if (plist[1].revents & POLLIN) {
      const ssize_t size = read(stderr_pipe.fd_read(), &errbuff, buf_size);
      if (size <= 0) throw std::runtime_error("read_stderr_error");
      capture_cerr->append(errbuff, size);
      plist[1].revents &= ~POLLIN;
    }

    if ((plist[0].revents & POLLHUP) == 0) {
      stop = false;
    } else {
      while (true) {
        const ssize_t size = read(stdout_pipe.fd_read(), &buffer, buf_size);
        if (size == 0) break;
        if (size < 0) throw std::runtime_error("read_error");
        capture_cout->append(buffer, size);
      }
      plist[0].revents &= ~POLLHUP;
    }

    if ((plist[1].revents & POLLHUP) == 0) {
      stop = false;
    } else {
      while (true) {
        const ssize_t size = read(stderr_pipe.fd_read(), &errbuff, buf_size);
        if (size == 0) break;
        if (size < 0) throw std::runtime_error("read_error");
        capture_cerr->append(errbuff, size);
      }
      plist[1].revents &= ~POLLHUP;
    }

    if (plist[1].revents != 0 || plist[0].revents != 0) {
      throw std::runtime_error("another_poll_bit");
    }

    if (stop) break;
  }

  int status;
  if (waitpid(pid, &status, 0) != pid) {
    throw std::runtime_error("pid_error");
  }

  if (WIFEXITED(status) == 0) {
    throw std::runtime_error("exit_error");
  }

  if (posix_spawn_file_actions_destroy(&file_actions) != 0) {
    throw std::runtime_error("posix_spawn_file_actions_destroy_error");
  }
}

void subprocess::run_detached(const std::vector<std::string>& args) {
  std::string exe{verify_exe(args)};
  argv_buffer buf(args);
  pid_t pid;

  if (posix_spawn(&pid, exe.c_str(), nullptr, nullptr, buf.get(), nullptr) !=
      0) {
    throw std::runtime_error("posix_spawn_error");
  }

  thread_exe::go_thread(pid);
}
