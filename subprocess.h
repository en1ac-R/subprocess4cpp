#ifndef SUBPROCESS_H_
#define SUBPROCESS_H_

#include <string>
#include <thread>
#include <vector>

/// \brief Class to start a new process
class subprocess {
  friend class subprocess_test;

 private:
  static std::string verify_exe(const std::vector<std::string>& args);

  class thread_exe {
   private:
    static inline std::mutex lock_;
    static inline std::vector<pid_t> pids_;
    static inline bool thread_go_ = false;
    static inline std::unique_ptr<std::thread> thread_ = nullptr;
    static void thread_func();

   public:
    static void go_thread(pid_t&);
  };

  class output_pipe {
   private:
    int fd_write_, fd_read_;

   public:
    output_pipe();
    output_pipe(const output_pipe& any) = delete;
    output_pipe& operator=(const output_pipe& any) = delete;
    output_pipe(output_pipe&& any) = delete;
    output_pipe& operator=(output_pipe&& any) = delete;

    int fd_write() const;
    int fd_read() const;
    void close_fd_write();
    void close_fd_read();

    ~output_pipe();
  };

 public:
  /// \brief Start a process without capturing it's output
  /// \param cmd Command with arguments to execute
  /// \throw \ref std::runtime_error if exit code is not 0
  static void run(const std::vector<std::string>& args);

  /// \brief Start a process without capturing it's output
  /// \param cmd Command with arguments to execute
  /// \param[out] exit_code Pointer where the exit code of the finished
  ///   processed will be saved. Pointer should be valid and can't be null
  /// \throw \ref std::runtime_error if not able to start the process
  static void run(const std::vector<std::string>& args, int* exit_code);

  /// \brief Start a process and capture it's output
  /// \param cmd Command with arguments to execute
  /// \param[out] capture_cout Pointer where the \b stdout of the finished
  ///   processed will be saved. Pointer should be valid and can't be null
  /// \param[out] capture_cerr Pointer where the \b stderr of the finished
  ///   processed will be saved. Pointer should be valid and can't be null
  /// \throw \ref std::runtime_error if exit code is not 0
  static void run(const std::vector<std::string>& args,
                  std::string* capture_cout, std::string* capture_cerr);

  /// \brief Start a detached process without capturing it's output
  /// \details The process is started in detached mode, and the caller does not
  /// wait for an execution to finish. We don't know how long the command runs
  /// or whether it's ended successfully
  /// \param cmd Command with arguments to execute
  /// \throw \ref std::runtime_error if the command fails to launch, e.g., a
  /// path to the program to launch is not correct
  static void run_detached(const std::vector<std::string>& args);
};

#endif  // SUBPROCESS_H_
