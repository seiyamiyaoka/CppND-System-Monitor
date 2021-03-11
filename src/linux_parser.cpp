#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value;
  float mem_total;
  float mem_free;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "MemTotal:") {
          mem_total = value;
        }
        if (key == "MemFree:") {
          mem_free = value;
        }
      }
    }
  }
 return (mem_total - mem_free) / mem_total;
}

long LinuxParser::UpTime() {
  string line;
  string linestream;
  long uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return uptime;
  }
  return 0;
}

Jiffie LinuxParser::Jiffies() {
  std::string name;
  std::string linestream;
  std::string line;

  Jiffie jiffie;

  int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> name >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    jiffie.user = user;
    jiffie.nice = nice;
    jiffie.system = system;
    jiffie.idle = idle;
    jiffie.iowait = iowait;
    jiffie.irq = irq;
    jiffie.softirq = softirq;
    jiffie.steal = steal;
    jiffie.guest = guest;
    jiffie.guest_nice = guest_nice;
  }
  return jiffie;
}

// TODO: Read and return the number of active jiffies for a PID
PidJiffie LinuxParser::ActiveJiffies(int pid) {
  // file index
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  const int UTIME = 14;
  const int SUTIME = 15;
  const int CUTIME = 16;
  const int CSTIME = 17;
  const int STARTTIME = 22;
  
  PidJiffie pidJiffie;
  string utime, sutime, cutime, cstime, starttime, dust;
  string line;
  int counter = 1;
  
  string process_dir = LinuxParser::kProcDirectory + "/"+ to_string(pid) + "/"+ LinuxParser::kStatFilename;
  std::ifstream filestream(process_dir);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream) {
      switch (counter)
      {
      case UTIME:
        linestream >> utime;
        pidJiffie.utime = stol(utime);
        break;
      case SUTIME:
        linestream >> sutime;
        pidJiffie.sutime = stol(sutime);
        break;
      case CUTIME:
        linestream >> cutime;
        pidJiffie.cutime = stol(cutime);
        break;
      case CSTIME:
        linestream >> cstime;
        pidJiffie.cstime = stol(cstime);
        break;
      case STARTTIME:
        linestream >> starttime;
        pidJiffie.starttime = stol(starttime);
        break;
      default:
        linestream >> dust;
        break;
      }
      counter++;
      if(counter > 22)
        break;
    };
  }
  return pidJiffie;
}

long LinuxParser::ActiveJiffies() {
  Jiffie jiffie = LinuxParser::Jiffies();

  return jiffie.user + jiffie.nice + jiffie.system + jiffie.irq + jiffie.softirq + jiffie.steal;
}

long LinuxParser::IdleJiffies() {
  Jiffie jiffie = LinuxParser::Jiffies();

  return jiffie.idle + jiffie.iowait;
}

vector<string> LinuxParser::CpuUtilization() { return {}; }

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line, command;
  string process_dir = LinuxParser::kProcDirectory + "/"+ to_string(pid) + "/"+ LinuxParser::kCmdlineFilename;
  std::ifstream filestream(process_dir);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> command;
      return command;
    }
  }
  return "";
}

string LinuxParser::Ram(int pid) {
  string key, value, line, uid, result;
  string process_dir = LinuxParser::kProcDirectory + "/"+ to_string(pid) + "/"+ LinuxParser::kStatusFilename;
  std::ifstream filestream(process_dir);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if(key == "VmSize:")
          return std::to_string(int(std::stoi(value) * 0.001)); 
      }
    }
  }
  return "0";
}

string LinuxParser::Uid(int pid) {
  string key, value, line;
  string process_dir = LinuxParser::kProcDirectory + "/"+ to_string(pid) + "/"+ LinuxParser::kStatusFilename;
  std::ifstream filestream(process_dir);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if(key == "Uid:") {
          return value;
        }
      }
    }
  }
  return "";
}

string LinuxParser::User(int pid) {
  string line;
  string puser, ptmp, puid; // p is passwd.

  string uid = LinuxParser::Uid(pid);
  
  if (uid.size() < 1)
    return "";
  std::ifstream pfilestream(LinuxParser::kPasswordPath);
  if (pfilestream.is_open()) {
    while(std::getline(pfilestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream plinestream(line);
      while(plinestream >> puser >> ptmp >> puid) {
        if(puid == uid) {
          return puser;
        }
      }
    }
  }
  return "";
}

long int LinuxParser::UpTime(int pid) {
  long  starttime{0};
  long hertz = sysconf(_SC_CLK_TCK);
  string value, line;
  int count = 0;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid)+ "/" + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      ++count;
      switch(count){
        case 22: starttime = stol(value,nullptr,10); break;
      }
    }
  }
  return (UpTime() - (starttime / hertz)); 
}
