#ifndef HPP_THREAD_RUNNABLE
#define HPP_THREAD_RUNNABLE

class IRunnable {
protected:
  bool running;
public:
  IRunnable (void) { this->running = false; }
  virtual ~IRunnable (void) { }

  virtual void * run (void *) = 0;
  
  inline bool isRunning (void) const { return this->running; }
};

#endif
