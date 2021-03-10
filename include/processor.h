#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  int total, total_idle;
};

#endif