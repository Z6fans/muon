#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#define detectormode 1

DetectorConstruction::DetectorConstruction(): G4VUserDetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* superK(G4double hr, G4double hz, G4double pad, G4double thic,
                          G4Material* exterior, G4Material* wall, G4Material* interior);
G4VPhysicalVolume* bigTube(G4double l, G4double r, G4Material* mat);

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* H2O  = nist->FindOrBuildMaterial("G4_WATER");
  G4Material* steel  = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

  switch(detectormode)
  {
  case 0: return superK(40.*m, 40.*m, 20.*m, 3.*cm,
                        air, steel, H2O);
  case 1: return bigTube(200.*m, 20.*m, H2O);
  default: return nullptr;
  }
}

G4VPhysicalVolume* superK(G4double r, G4double z, G4double pad, G4double d,
                          G4Material* extr, G4Material* wall, G4Material* intr)
{
  G4Box*  wrldSolid = new G4Box("World", r + 2 * pad, r + 2 * pad, z + 2 * pad);
  G4Tubs* detrSolid = new G4Tubs("Detector",          0.*m,    r/2, z/2, 0.*deg, 360.*deg);
  G4Tubs* wallSolid = new G4Tubs("DetectorWall",      r/2 - d, r/2, z/2, 0.*deg, 360.*deg);
  G4Tubs* capBSolid = new G4Tubs("DetectorCapBottom", 0.*m,    r/2, d/2, 0.*deg, 360.*deg);
  G4Tubs* capTSolid = new G4Tubs("DetectorCapTop",    0.*m,    r/2, d/2, 0.*deg, 360.*deg);

  G4LogicalVolume* wrldLogic = new G4LogicalVolume(wrldSolid, extr, "World");
  G4LogicalVolume* detrLogic = new G4LogicalVolume(detrSolid, intr, "Detector");
  G4LogicalVolume* wallLogic = new G4LogicalVolume(wallSolid, wall, "DetectorWall");
  G4LogicalVolume* capBLogic = new G4LogicalVolume(capBSolid, wall, "DetectorCapBottom");
  G4LogicalVolume* capTLogic = new G4LogicalVolume(capTSolid, wall, "DetectorCapTop");

  G4VPhysicalVolume* wrldPhys = new G4PVPlacement(nullptr, G4ThreeVector(0.*m, 0.*m, 0.*m),
                                                  wrldLogic, "World", nullptr, false, 0);
  G4VPhysicalVolume* detrPhys = new G4PVPlacement(nullptr, G4ThreeVector(0.*m, 0.*m, 0.*m),
                                                  detrLogic, "Detector", wrldLogic, false, 0);
  G4VPhysicalVolume* wallPhys = new G4PVPlacement(nullptr, G4ThreeVector(0.*m, 0.*m, 0.*m),
                                                  wallLogic, "DetectorWall", wrldLogic, false, 0);
  G4VPhysicalVolume* capBPhys = new G4PVPlacement(nullptr, G4ThreeVector(0.*m, 0.*m, d/2 - z/2),
                                                  capBLogic, "DetectorCapBottom", wrldLogic, false, 0);
  G4VPhysicalVolume* capTPhys = new G4PVPlacement(nullptr, G4ThreeVector(0.*m, 0.*m, z/2 - d/2),
                                                  capTLogic, "DetectorCapTop", wrldLogic, false, 0);

  return wrldPhys;
}

G4VPhysicalVolume* bigTube(G4double l, G4double r, G4Material* mat)
{
  G4Tubs*            worldSolid = new G4Tubs("World", 0, r, l, 0.*deg, 360.*deg);
  G4LogicalVolume*   worldLogic = new G4LogicalVolume(worldSolid, mat, "World");
  G4VPhysicalVolume* worldPhys  = new G4PVPlacement(nullptr, G4ThreeVector(0.*m, 0.*m, 0.*m),
                                                    worldLogic, "World", nullptr, false, 0);

  return worldPhys;
}
