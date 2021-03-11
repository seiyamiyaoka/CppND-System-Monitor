#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <fstream>

float Processor::Utilization() {
  std::string name;
  std::string linestream;
  std::string line;
  
  int prevIdle;
  int prevNonIdle;
  int prevTotal;

  int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  int nonIdle;
  int total;
  int totald;
  int idled;
  float result;
  
  //   https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  prevIdle = this->idle + this->iowait;
  prevNonIdle = this->user + this->idle + this->nice + this->irq + this->softirq + this->steal;

  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> name >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    
    idle = LinuxParser::IdleJiffies();
    nonIdle = LinuxParser::ActiveJiffies();
    
    prevTotal = prevIdle + prevNonIdle;
    total = idle + nonIdle;
    
    totald = total - prevTotal;
    idled = idle - prevIdle;
    result = float(totald - idled)/(totald * 100);
    
    // Keep the current value for use in the next loop.
    this->user = user;
    this->guest_nice = guest_nice;
    this->nice = nice;
    this->idle = idle;
    this->iowait = iowait;
    this->system = system;
    this->irq = irq;
    this->softirq = softirq;
    this->steal = steal;
    
    return result;
  }
  return 0.0;
}