#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm> 
#include <iostream>

#include "process.h"
#include "processor.h"
#include "linux_parser.h" // file sysytem parser
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;


// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  // fetch all proc ids
  std::vector<int> pids = LinuxParser::Pids();
  //  https://cpprefjp.github.io/reference/set/set.html
  std::set<int> uniq_pids;
  // already exist pid set
  for(auto process : processes_) {
    uniq_pids.insert(process.Pid());
  }
  // all pids loop
  for(auto pid : pids) {
    // process create or update
    Process p;
    p.Pid(pid);
    p.Uptime(System::UpTime());
    p.Cpu();
    if (uniq_pids.find(pid) == uniq_pids.end()) {
      processes_.push_back(p);
    }
  }
  //  sort
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}
// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {
  if(kernel_.size() < 1) {
    kernel_ = LinuxParser::Kernel();
  }
  return kernel_;
}
void System::Kernel(std::string version) {
  kernel_ = version;
}
// TODO: Return the system's memory utilization
float System::MemoryUtilization() {
  return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() {
  return LinuxParser::OperatingSystem();
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() {
  return LinuxParser::RunningProcesses();
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() {
  return  LinuxParser::TotalProcesses();
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() {
  return  LinuxParser::UpTime();
}