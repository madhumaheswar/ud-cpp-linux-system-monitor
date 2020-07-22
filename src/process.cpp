#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

Process::Process(int pid) : pid_(pid) {}

// Returns this process's ID
int Process::Pid() { return pid_; }

// Returns the user (name) that generated this process
std::string Process::User() {
  user_ = LinuxParser::User(pid_);

  return user_;
}

// Returns the command that generated this process
std::string Process::Command() {
  command_ = LinuxParser::Command(pid_);

  return command_;
}

// Returns this process's CPU utilization
float Process::CpuUtilization() {
  cpuUtilization_ = LinuxParser::ProcessCpuUtilization(pid_);

  return cpuUtilization_;
}

// Returns this process's memory utilization
std::string Process::Ram() {
  memoryUtilization_ = LinuxParser::Ram(pid_);

  return memoryUtilization_;
};

// Returns the age of this process (in seconds)
long int Process::UpTime() {
  uptime_ = LinuxParser::UpTime(pid_);

  return uptime_;
}

// Overloads the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return cpuUtilization_ < a.cpuUtilization_;
}

// Overloads the "greater than" comparison operator for Process objects
bool Process::operator>(Process const& a) const {
  return cpuUtilization_ > a.cpuUtilization_;
}
