#include <Svc/RateGroupDriver/RateGroupDriverImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <cstring>
#include <Fw/Types/Assert.hpp>
#include <stdio.h>

namespace Svc {

    RateGroupDriverImpl::RateGroupDriverImpl(const char* compName, I32 dividers[], I32 numDividers) :
        RateGroupDriverComponentBase(compName),
    m_ticks(0),m_rollover(1),interrupt_counter(0)
    {

        // double check arguments
        FW_ASSERT(dividers);
        FW_ASSERT(numDividers <= static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)),
                numDividers,
                static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)));
        // verify port/table size matches
        FW_ASSERT(FW_NUM_ARRAY_ELEMENTS(this->m_dividers) == this->getNum_CycleOut_OutputPorts(),
                static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)),
                this->getNum_CycleOut_OutputPorts());
        // clear table
        ::memset(this->m_dividers,0,sizeof(this->m_dividers));
        for (NATIVE_INT_TYPE entry = 0; entry < numDividers; entry++) {
            this->m_dividers[entry] = dividers[entry];
            // rollover value should be product of all dividers to make sure integer rollover doesn't jump cycles
            // only use non-zero dividers
            if (dividers[entry] != 0) {
                this->m_rollover *= dividers[entry];
            }
        }

	(void)memset(elapsed_times, 0, sizeof elapsed_times);

    }

    RateGroupDriverImpl::~RateGroupDriverImpl(void) {

    }
    
    void RateGroupDriverImpl::init(void) {
        RateGroupDriverComponentBase::init();
    }

    void RateGroupDriverImpl::print_elapsed_times(times *elapsed_time, const uint32_t len, const uint32_t threshold)
{
  FW_ASSERT(elapsed_time);
  FW_ASSERT(len > 0);
  uint32_t highest_usec = 0;
  uint32_t lowest_usec = threshold;
  uint32_t average_usec = 0;
  uint32_t average_isr_usec = 0;
  uint32_t skipped = 0;

  for(uint32_t i = 0; i < len; i++)
  {
    if(elapsed_time[i].cycle == 0)
    {
      skipped++;
      printf("skipping entry %u\n", i);
      continue;
    }

    if(i < len)
    {
      uint32_t delta_isr_usec = 100;
      uint32_t diff_isr = elapsed_time[i+1].isr_start.diffUSec(elapsed_time[i].isr_start);
      if(diff_isr > (threshold + delta_isr_usec) ||
         diff_isr < (threshold - delta_isr_usec))
      {
	      Os::IntervalTimer::RawTime isr1 = elapsed_time[i].start.getTimerVal();
	      Os::IntervalTimer::RawTime isr2 = elapsed_time[i+1].start.getTimerVal();
        printf("ISR1 counter %u time is %u.%u\n", i, isr1.upper, isr1.lower);
        printf("ISR2 counter %u time is %u.%u\n", i+1, isr2.upper, isr2.lower);
        printf("ISR Diff time is %u usecs\n", diff_isr);
      }
      average_isr_usec += diff_isr;
    }

    uint32_t diff_usecs = elapsed_time[i].end.diffUSec(elapsed_time[i].start);
    if(diff_usecs > threshold)
    {
      Os::IntervalTimer::RawTime endtime = elapsed_time[i].end.getTimerVal();
      Os::IntervalTimer::RawTime starttime = elapsed_time[i].start.getTimerVal();
      printf("Cycle Slip %u: \n", elapsed_time[i].cycle);
      printf("\t\tend time  : %u.%u\n", endtime.upper, endtime.lower);
      printf("\t\tstart time: %u.%u\n", starttime.upper, starttime.lower);
    }
    highest_usec = (diff_usecs > highest_usec) ? diff_usecs : highest_usec;
    lowest_usec = (diff_usecs < lowest_usec) ? diff_usecs : lowest_usec;
    average_usec += diff_usecs;
  }
  FW_ASSERT(len > skipped);
  average_usec /= (len-skipped);
  printf("highest duration: %u usec\n", highest_usec);
  printf("lowest duration : %u usec\n", lowest_usec);
  printf("average duration: %u usec\n", average_usec);
  printf("average isr duration: %u usec\n", average_isr_usec/(len-skipped));
  printf("skipped         : %u\n", skipped);
}

    void RateGroupDriverImpl::CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {

	      U32 prev_interrupt_counter = 0;
	      if(portNum == RATE_GROUP_NUM)
	      {
		      prev_interrupt_counter = interrupt_counter;
		      if(prev_interrupt_counter < MAX_LOOP)
		      {
			      elapsed_times[prev_interrupt_counter].cycle = prev_interrupt_counter;
			      elapsed_times[prev_interrupt_counter].start.take();
			      elapsed_times[prev_interrupt_counter].isr_start = cycleStart;
		      }
	      }

        // Loop through each divider. For a given port, the port will be called when the divider value
        // divides evenly into the number of ticks. For example, if the divider value for a port is 4,
        // it would be called every fourth invocation of the CycleIn port.
        for (NATIVE_INT_TYPE entry = 0; entry < static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)); entry++) {
            if (this->m_dividers[entry] != 0) {
                if (this->isConnected_CycleOut_OutputPort(entry)) {
                    if ((this->m_ticks % this->m_dividers[entry]) == 0) {
                        this->CycleOut_out(entry,cycleStart);
                    }
                }
            }
        }
        
        // rollover the tick value when the tick count reaches the rollover value
        // the rollover value is the product of all the dividers. See comment in constructor.
        this->m_ticks = (this->m_ticks + 1) % this->m_rollover;

        if(portNum == RATE_GROUP_NUM)
        {
          if(prev_interrupt_counter < MAX_LOOP)
          {
			      elapsed_times[prev_interrupt_counter].end.take();
            interrupt_counter++;
            prev_interrupt_counter = interrupt_counter;
          }

          static bool done = false;
          if(!done && prev_interrupt_counter >= MAX_LOOP)
          {
            print_elapsed_times(elapsed_times, MAX_LOOP-1, CYCLE_SLIP_THRESHOLD); // last entry is 0
            done = true;
          }
        }
    }

}
