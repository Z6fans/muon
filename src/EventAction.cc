#include "EventAction.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4Trajectory.hh"
#include "g4root.hh"

EventAction::EventAction(){}

EventAction::~EventAction(){}

using namespace std;

void EventAction::EndOfEventAction(const G4Event* anEvent)
{
  G4int eventID = anEvent->GetEventID();
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  G4TrajectoryContainer* tc = anEvent->GetTrajectoryContainer();
  if(tc == nullptr) return;

  vector<G4Trajectory*>* trajectories = (vector<G4Trajectory*>*)tc->GetVector();

  for_each(trajectories->begin(), trajectories->end(), [analysisManager, eventID](G4Trajectory* t){
    G4int trackID = t->GetTrackID();
    G4int parentID = t->GetParentID();

    G4String particleName = t->GetParticleName();
    G4int particle = 0;
    if(particleName == "e-")         particle = 1;
    if(particleName == "e+")         particle = 2;
    if(particleName == "mu+")        particle = 3;
    if(particleName == "mu-")        particle = 4;
    if(particleName == "gamma")      particle = 5;
    if(particleName == "nu_e")       particle = 6;
    if(particleName == "anti_nu_e")  particle = 7;
    if(particleName == "nu_mu")      particle = 8;
    if(particleName == "anti_nu_mu") particle = 9;
    if(particleName == "proton")     particle = 10;
    if(particleName == "neutron")    particle = 11;

    G4double kenergy = t->GetInitialKineticEnergy() / eV;

    G4ThreeVector momentum = t->GetInitialMomentum();
    G4double xMomentum = momentum.x() / eV;
    G4double yMomentum = momentum.y() / eV;
    G4double zMomentum = momentum.z() / eV;

    G4ThreeVector initailPos = t->GetPoint(0)->GetPosition();
    G4double initialX = initailPos.x() / m;
    G4double initialY = initailPos.y() / m;
    G4double initialZ = initailPos.z() / m;

    G4ThreeVector finalPos = t->GetPoint(t->GetPointEntries() - 1)->GetPosition();
    G4double finalX = finalPos.x() / m;
    G4double finalY = finalPos.y() / m;
    G4double finalZ = finalPos.z() / m;

    analysisManager->FillNtupleIColumn(0,  eventID);
    analysisManager->FillNtupleIColumn(1,  trackID);
    analysisManager->FillNtupleIColumn(2,  parentID);
    analysisManager->FillNtupleIColumn(3,  particle);
    analysisManager->FillNtupleDColumn(4,  kenergy);
    analysisManager->FillNtupleDColumn(5,  xMomentum);
    analysisManager->FillNtupleDColumn(6,  yMomentum);
    analysisManager->FillNtupleDColumn(7,  zMomentum);
    analysisManager->FillNtupleDColumn(8,  initialX);
    analysisManager->FillNtupleDColumn(9,  initialY);
    analysisManager->FillNtupleDColumn(10, initialZ);
    analysisManager->FillNtupleDColumn(11, finalX);
    analysisManager->FillNtupleDColumn(12, finalY);
    analysisManager->FillNtupleDColumn(13, finalZ);

    analysisManager->AddNtupleRow();
  });
}
