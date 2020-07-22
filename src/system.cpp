#include <unistd.h>
#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

// Returns the system's CPU
Processor& System::Cpu() { return cpu_; }

// Returns a container composed of the system's processes
std::vector<Process>& System::Processes() {
  processes_.clear();
  std::vector<int> pids = LinuxParser::Pids();

  for (auto pid : pids) {
    Process p(pid);
    p.CpuUtilization();
    processes_.emplace_back(p);
  }

  std::sort(processes_.begin(), processes_.end(), std::greater<Process>());
  return processes_;
}

// Returns the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Returns the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }

// Returns the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Returns the nummber of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Returns the system's kernel identifier
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Returns the Operating System name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }
