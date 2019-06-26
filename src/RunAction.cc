#include "RunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "g4root.hh"

RunAction::RunAction(){}

RunAction::~RunAction(){}

void RunAction::BeginOfRunAction(const G4Run* run)
{
  if(isMaster) // WARNING : in sequential mode, isMaster == true
    BeginMaster();
  else
    BeginWorker();
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  if(isMaster)
    EndMaster(run);
  else
    EndWorker(run);
}

void RunAction::BeginMaster()
{
  bool sequential = (G4RunManager::GetRunManager()->GetRunManagerType() == G4RunManager::sequentialRM);
  if(sequential) CreateHistogram();
}

void RunAction::BeginWorker()
{
  CreateHistogram();
}

void RunAction::EndMaster(const G4Run* run)
{
  bool sequential = (G4RunManager::GetRunManager()->GetRunManagerType() == G4RunManager::sequentialRM);
  if(sequential) EndWorker(run);
}

void RunAction::EndWorker(const G4Run* run)
{
  if(run->GetNumberOfEvent() == 0) return;

  WriteHistogram();
  delete G4AnalysisManager::Instance();
}

void RunAction::CreateHistogram()
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);
  G4String filename = "muon";
  analysisManager->OpenFile(filename);
  analysisManager->CreateNtuple("muonData", "physics");

  analysisManager->CreateNtupleIColumn("eventID");
  analysisManager->CreateNtupleIColumn("trackID");
  analysisManager->CreateNtupleIColumn("parentID");
  analysisManager->CreateNtupleSColumn("particle");
  analysisManager->CreateNtupleSColumn("process");
  analysisManager->CreateNtupleDColumn("kenergy");
  analysisManager->CreateNtupleDColumn("xMomentum");
  analysisManager->CreateNtupleDColumn("yMomentum");
  analysisManager->CreateNtupleDColumn("zMomentum");
  analysisManager->CreateNtupleDColumn("initialX");
  analysisManager->CreateNtupleDColumn("initialY");
  analysisManager->CreateNtupleDColumn("initialZ");
  analysisManager->CreateNtupleDColumn("finalX");
  analysisManager->CreateNtupleDColumn("finalY");
  analysisManager->CreateNtupleDColumn("finalZ");

  analysisManager->FinishNtuple();
}

void RunAction::WriteHistogram()
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}
