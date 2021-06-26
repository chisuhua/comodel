#pragma once
#include "GlobalEvent.h"
// #include "sim/serialize.hh"

//
// Event to terminate simulation at a particular cycle/instruction
//
class GlobalSimLoopExitEvent : public GlobalEvent
{
  protected:
    // string explaining why we're terminating
    std::string cause;
    int code;
    Tick repeat;

  public:
    GlobalSimLoopExitEvent(Tick when, const std::string &_cause, int c,
                           Tick repeat = 0);
    GlobalSimLoopExitEvent(const std::string &_cause, int c, Tick repeat = 0);

    const std::string getCause() const { return cause; }
    int getCode() const { return code; }

    void process();     // process event

    virtual const char *description() const;
};

class LocalSimLoopExitEvent : public Event
{
  protected:
    // string explaining why we're terminating
    std::string cause;
    int code;
    Tick repeat;

  public:
    LocalSimLoopExitEvent();
    LocalSimLoopExitEvent(const std::string &_cause, int c, Tick repeat = 0);

    const std::string getCause() const { return cause; }
    int getCode() const { return code; }

    void process() override;     // process event

    const char *description() const override;

};

//
// Event class to terminate simulation after 'n' related events have
// occurred using a shared counter: used to terminate when *all*
// threads have reached a particular instruction count
//
class CountedExitEvent : public Event
{
  private:
    std::string cause;  // string explaining why we're terminating
    int &downCounter;   // decrement & terminate if zero

  public:
    CountedExitEvent(const std::string &_cause, int &_downCounter);

    void process() override;     // process event

    const char *description() const override;
};

