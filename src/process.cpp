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
  // file index
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  const int UTIME = 14;
  const int SUTIME = 15;
  const int CUTIME = 16;
  const int CSTIME = 17;
  const int STARTTIME = 22;
  if (this->uptime_ < 1) {
    cpu_ =  0.0;
  } else {
    string utime, sutime, cutime, cstime, starttime, dust;
  float total_time, second, hertz = sysconf(_SC_CLK_TCK);
  string line;
  int counter = 1;
  
  string process_dir = LinuxParser::kProcDirectory + "/"+ to_string(Process::Pid()) + "/"+ LinuxParser::kStatFilename;
  std::ifstream filestream(process_dir);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream) {
      switch (counter)
      {
      case UTIME:
        linestream >> utime;
        break;
      case SUTIME:
        linestream >> sutime;
        break;
      case CUTIME:
        linestream >> cutime;
        break;
      case CSTIME:
        linestream >> cstime;
        break;
      case STARTTIME:
        linestream >> starttime;
        break;
      default:
        linestream >> dust;
        break;
      }
      counter++;
      if(counter > 22)
        break;
    };
    start_time_ = std::stol(starttime);
    total_time = std::stoi(utime) + std::stoi(sutime) + std::stoi(cutime) + std::stoi(cstime);
    second = this->uptime_ - (std::stoi(starttime) / hertz);
    cpu_ =  (float(total_time) / hertz) / second;
    }
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