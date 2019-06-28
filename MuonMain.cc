#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc,char** argv)
{
  G4RunManager* runManager = new G4RunManager;

  runManager->SetUserInitialization(new DetectorConstruction);
  runManager->SetUserInitialization(new QBBC(0));
  runManager->SetUserInitialization(new ActionInitialization);

  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if(argc==1)
  {
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
    delete visManager;
  }
  else
  {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }

  delete runManager;
  return 0;
}
