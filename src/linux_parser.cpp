#include <dirent.h>
#include <unistd.h>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "linux_parser.h"

// System

// Returns the system's memory utilization
float LinuxParser::MemoryUtilization() {
  std::string key;
  double value = 0.0;
  double totalMemory = 0.0;
  double freeMemory = 0.0;
  double memUtilization = 0.0;
  double memUtilizationPercentage = 0.0;
  std::string line;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    for (int i = 0; i < 2; ++i) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >> key >> value;

      switch (i) {
        case 0:
          totalMemory = value;
          break;
        case 1:
          freeMemory = value;
          break;
      }
    }

    memUtilization = totalMemory - freeMemory;
    memUtilizationPercentage = memUtilization / totalMemory;

    return memUtilizationPercentage;
  }

  return 0.0;
}

// Returns the system uptime
long LinuxParser::UpTime() {
  long uptime = 0;
  std::string line;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// Pending -- BONUS: Update this to use std::filesystem
// Returns a vector of Process ids in the system
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      std::string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Returns the total number of processes
int LinuxParser::TotalProcesses() {
  std::string key;
  int value;
  std::string line;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;

      if (key == "processes") {
        return value;
      }
    }
  }

  return 0;
}

// Returns the number of running processes
int LinuxParser::RunningProcesses() {
  std::string key;
  int value;
  std::string line;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;

      if (key == "procs_running") return value;
    }
  }

  return 0;
}

// Returns the Operating System name
std::string LinuxParser::OperatingSystem() {
  std::string line;
  std::string key;
  std::string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Returns the Returns the system's kernel identifier
std::string LinuxParser::Kernel() {
  std::string os;
  std::string version;
  std::string kernel;
  std::string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// System Processor

// Parses and Stores CPU values in a vector
void LinuxParser::ParseCPUStatsFile(std::vector<long long>& cpuStats) {
  std::string line;
  std::string cpu;
  long long time;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    linestream >> cpu;  // Discarding the first field
    for (int i = 0; i < 10; ++i) {
      linestream >> time;
      cpuStats.push_back(time);
    }
  }
}

// Logic from here --
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
// Returns the System Processor Utilization
float LinuxParser::SystemProcessorUtilization() {
  std::vector<long long> cpuStatsT1;
  std::vector<long long> cpuStatsT2;

  long long userT, niceT, systemT, idleT, iowaitT;
  long long irqT, softirqT, stealT, guestT, guest_niceT;

  ParseCPUStatsFile(cpuStatsT1);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ParseCPUStatsFile(cpuStatsT2);

  userT = cpuStatsT2[0] - cpuStatsT1[0];
  niceT = cpuStatsT2[1] - cpuStatsT1[1];
  systemT = cpuStatsT2[2] - cpuStatsT1[2];
  idleT = cpuStatsT2[3] - cpuStatsT1[3];
  iowaitT = cpuStatsT2[4] - cpuStatsT1[4];
  irqT = cpuStatsT2[5] - cpuStatsT1[5];
  softirqT = cpuStatsT2[6] - cpuStatsT1[6];
  stealT = cpuStatsT2[7] - cpuStatsT1[7];
  guestT = cpuStatsT2[8] - cpuStatsT1[8];
  guest_niceT = cpuStatsT2[9] - cpuStatsT1[9];

  long idleTime = idleT + iowaitT;

  // guest and guest_nice are already accounted in user and nice respectively
  long nonIdleTime = (userT - guestT) + (niceT - guest_niceT) + systemT + irqT +
                     softirqT + stealT;

  long totalTime = idleTime + nonIdleTime;

  float systemProcessorUtilization =
      static_cast<float>(totalTime - idleTime) / totalTime;

  return systemProcessorUtilization;
}

// Processes

// CPU
/*
// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }
*/

// Reads and returns the parsed Process Stat File
void LinuxParser::ParseProcessStatFile(std::vector<long>& parseProcessStatFile,
                                       int pid) {
  std::string line;
  std::string value1, value2;
  long value;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    linestream >> value >> value1 >> value2;
    parseProcessStatFile.push_back(
        0);  // To match with /proc/pid/stat description starting with 1
    parseProcessStatFile.push_back(value);
    parseProcessStatFile.push_back(0);  // value1 discarded
    parseProcessStatFile.push_back(0);  // value2 discarded

    // Parsing only first 22 values needed for this task
    for (std::size_t i = 4; i < 23; ++i) {
      linestream >> value;
      parseProcessStatFile.push_back(value);
    }
  }
}

// Logic from here -
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
// Returns CPU utilization
float LinuxParser::ProcessCpuUtilization(int pid) {
  std::vector<long> processStatParse;
  ParseProcessStatFile(processStatParse, pid);

  long systemUptime = LinuxParser::UpTime();

  long utime = processStatParse[14];
  long stime = processStatParse[15];
  long cutime = processStatParse[16];
  long cstime = processStatParse[17];
  long starttime = processStatParse[22];

  long systemTicks = sysconf(_SC_CLK_TCK);

  double total_time = utime + stime + cutime + cstime;

  double elapsed_time = systemUptime - (double(starttime) / systemTicks);

  double cpuUtilization = (total_time / systemTicks) / elapsed_time;

  return cpuUtilization;
}

// Reads and returns the command associated with a process
std::string LinuxParser::Command(int pid) {
  std::string command;
  std::string line;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> command;
    return command;
  }

  return std::string();
}

// Reads and returns the memory used by a process
std::string LinuxParser::Ram(int pid) {
  std::string memUtilized;

  std::string key;
  int value;
  std::string line;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        value = value / 1024;  // Memsize in mB
        memUtilized = std::to_string(value);

        return memUtilized;
      }
    }
  }

  return std::string();
}

// Reads and returns the user ID associated with a process
int LinuxParser::Uid(int pid) {
  std::string key;
  int value;
  std::string line;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return value;
      }
    }
  }

  return 0;
}

// Reads and returns the user associated with a process
std::string LinuxParser::User(int pid) {
  int userid = Uid(pid);
  std::string line;
  std::string username, ePassword;
  int uid;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> ePassword >> uid;
      if (uid == userid) {
        return username;
      }
    }
  }

  return std::string();
}

// Returns the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::vector<long> parsedProcessStatFile;
  ParseProcessStatFile(parsedProcessStatFile, pid);

  long processUptime = parsedProcessStatFile[22] / sysconf(_SC_CLK_TCK);

  return processUptime;
}