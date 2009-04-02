#ifndef HPP_THREAD_LOCKABLE
#define HPP_THREAD_LOCKABLE

class ILockable {
public:
  virtual ~ILockable (void) { }

  virtual void lock (void) = 0;
  virtual void unlock (void) = 0;
  virtual bool trylock (void) = 0;
};

#endif
