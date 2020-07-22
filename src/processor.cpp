#include "processor.h"
#include "linux_parser.h"

// Returns the aggregate CPU utilization
float Processor::Utilization() {
  processorUtilization_ = LinuxParser::SystemProcessorUtilization();

  return processorUtilization_;
}