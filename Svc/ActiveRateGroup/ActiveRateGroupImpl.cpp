/*
* \author: Tim Canham
* \file:
* \brief
*
* This file implements the ActiveRateGroup component,
* which invokes a set of components the comprise the rate group.
*
*   Copyright 2014-2015, by the California Institute of Technology.
*   ALL RIGHTS RESERVED. United States Government Sponsorship
*   acknowledged.
*
*/

#include <Svc/ActiveRateGroup/ActiveRateGroupImpl.hpp>
#include <ActiveRateGroupImplCfg.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
//#include <Os/Log.hpp>
#include <logLib.h>
#include <Fw/Types/EightyCharString.hpp>

namespace Svc {

    ActiveRateGroupImpl::ActiveRateGroupImpl(const char* compName, NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts) :
            ActiveRateGroupComponentBase(compName),
            m_cycles(0), 
            m_maxTime(0),
            m_cycleStarted(false),
            m_overrunThrottle(0),
            m_cycleSlips(0),interrupt_counter(0),average_elapsed_time(0),highest_slip(0),lowest_slip(CYCLE_SLIP_THRESHOLD*2), average_slip(0),slip_counter(0) {
        FW_ASSERT(contexts);
        FW_ASSERT(numContexts == static_cast<NATIVE_UINT_TYPE>(this->getNum_RateGroupMemberOut_OutputPorts()),numContexts,this->getNum_RateGroupMemberOut_OutputPorts());
        FW_ASSERT(FW_NUM_ARRAY_ELEMENTS(this->m_contexts) == this->getNum_RateGroupMemberOut_OutputPorts(),
                static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_contexts)),
                this->getNum_RateGroupMemberOut_OutputPorts());

        // copy context values
        for (NATIVE_INT_TYPE entry = 0; entry < this->getNum_RateGroupMemberOut_OutputPorts(); entry++) {
            this->m_contexts[entry] = contexts[entry];
        }
    }
    
    void ActiveRateGroupImpl::init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance) {
        ActiveRateGroupComponentBase::init(queueDepth,instance);
    }

    ActiveRateGroupImpl::~ActiveRateGroupImpl(void) {

    }

    void ActiveRateGroupImpl::preamble(void) {
        this->log_DIAGNOSTIC_RateGroupStarted();
    }

    void ActiveRateGroupImpl::CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {


        Fw::EightyCharString name = this->getObjName();
        Fw::EightyCharString exp("RG5");
        U32 prev_interrupt_counter = interrupt_counter;

        TimerVal end;

        this->m_cycleStarted = false;

        // invoke any members of the rate group
        for (NATIVE_INT_TYPE port = 0; port < this->getNum_RateGroupMemberOut_OutputPorts(); port++) {
            if (this->isConnected_RateGroupMemberOut_OutputPort(port)) {
                this->RateGroupMemberOut_out(port,this->m_contexts[port]);
            }
        }

        // grab timer for end of cycle
        end.take();

        // get rate group execution time
        U32 cycle_time = end.diffUSec(cycleStart);

        if(exp == name)
        {
          if(prev_interrupt_counter < MAX_LOOP) {
            elapsed_time_usec[prev_interrupt_counter] = cycle_time;
            average_elapsed_time += cycle_time;
            interrupt_counter++;
            prev_interrupt_counter = interrupt_counter;
          }
static bool done = false;
          if(!done && prev_interrupt_counter >= MAX_LOOP) {
            done = true;
            logMsg("ARG5 average_elapsed_time calc average_elapsed_time / MAX_LOOP -> %u / %u \n", average_elapsed_time,MAX_LOOP,0,0,0,0);
            logMsg("ARG5 average_elapsed_time %u \n", (average_elapsed_time/MAX_LOOP),0,0,0,0,0);
            for(U32 i = 0; i < MAX_LOOP-1; i++)
            {
              if(elapsed_time_usec[i] > CYCLE_SLIP_THRESHOLD)
              {
                highest_slip = (elapsed_time_usec[i] > highest_slip) ? elapsed_time_usec[i] : highest_slip;
                lowest_slip = (elapsed_time_usec[i] < lowest_slip) ? elapsed_time_usec[i] : lowest_slip;
                average_slip += elapsed_time_usec[i];
                slip_counter++;
              }
            }
            logMsg("ARG5 highest slip %u\n", highest_slip, 0,0,0,0,0);
            logMsg("ARG5 lowest slip %u\n", lowest_slip, 0,0,0,0,0);
            logMsg("ARG5 average slip %u\n", average_slip/slip_counter, 0,0,0,0,0);
            logMsg("ARG5 number of slips %u\n", slip_counter, 0,0,0,0,0);
          }
        }


        // check to see if the time has exceeded the previous maximum
        if (cycle_time > this->m_maxTime) {
            this->m_maxTime = cycle_time;
        }

        // update cycle telemetry
        this->tlmWrite_RgMaxTime(this->m_maxTime);

        // check for cycle slip. That will happen if new cycle message has been received
        // which will cause flag will be set again.
        if (this->m_cycleStarted) {
            this->m_cycleSlips++;
            if (this->m_overrunThrottle < ACTIVE_RATE_GROUP_OVERRUN_THROTTLE) {
                this->log_WARNING_HI_RateGroupCycleSlip(this->m_cycles);
                this->m_overrunThrottle++;
            }
            // update cycle cycle slips
            this->tlmWrite_RgCycleSlips(this->m_cycleSlips);
        } else { // if cycle is okay start decrementing throttle value
            if (this->m_overrunThrottle > 0) {
                this->m_overrunThrottle--;
            }
        }

        // increment cycle
        this->m_cycles++;

    }

    void ActiveRateGroupImpl::CycleIn_preMsgHook(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {
        // set flag to indicate cycle has started. Check in thread for overflow.
        this->m_cycleStarted = true;
    }

    void ActiveRateGroupImpl::PingIn_handler(NATIVE_INT_TYPE portNum, U32 key) {
        // return the key to health
        this->PingOut_out(0,key);
    }


}
