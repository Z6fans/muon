#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"

class EventAction: public G4UserEventAction
{
public:
  EventAction();
  virtual ~EventAction();
  void EndOfEventAction(const G4Event* anEvent);
};

#endif // EVENTACTION_HH
