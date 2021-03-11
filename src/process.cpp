#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
void Process::Pid(int pid) {
  pid_ = pid;
};
void Process::Uptime(long uptime) {
  uptime_ = uptime;
}
void Process::Cpu() {
  if (this->uptime_ < 1) {
    cpu_ =  0.0;
  } else {
    long total_time;
    float second, hertz = sysconf(_SC_CLK_TCK);

    PidJiffie pidJiffie = LinuxParser::ActiveJiffies(Process::Pid());
    start_time_ = pidJiffie.starttime;
    total_time = pidJiffie.utime + pidJiffie.sutime + pidJiffie.cutime + pidJiffie.cstime;
    second = this->uptime_ - (pidJiffie.starttime / hertz);
    cpu_ =  (float(total_time) / hertz) / second;
  }
}
int Process::Pid() { return this->pid_; }

float Process::CpuUtilization() {
  return cpu_;
}

string Process::Command() {
  return LinuxParser::Command(Process::Pid());
}

string Process::Ram() {
  return LinuxParser::Ram(Process::Pid());
}

string Process::User() {
  return LinuxParser::User(Process::Pid());
}

long Process::UpTime() {
  return LinuxParser::UpTime(Process::Pid());
}

bool Process::operator<(Process const& a) const {
  return this->cpu_ > a.cpu_;
}