#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"

class G4Run;

class RunAction : public G4UserRunAction
{
public:

  RunAction();
  ~RunAction();

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

private:

  void CreateHistogram();
  void WriteHistogram();

  void BeginMaster();
  void EndMaster(const G4Run*);

  void BeginWorker();
  void EndWorker(const G4Run*);

};

#endif // RUNACTION_HH
