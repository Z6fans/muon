#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#define ISRANDOM false

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(nullptr),
  numRuns(0)
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="mu-");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleEnergy(1.*GeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  numRuns++;
  G4cout << numRuns << " runs" << G4endl;

  G4double x, y, z, xm, ym, zm;

  if(ISRANDOM)
  {
    G4double r = G4RandFlat::shoot(0.,20. - 0.03);
    G4double th = G4RandFlat::shoot(0.,CLHEP::twopi);

    x = r * std::cos(th) * m;
    y = r * std::sin(th) * m;
    z = G4RandFlat::shoot(-20. + 0.03,20. - 0.03) * m;

    xm = G4RandGauss::shoot();
    ym = G4RandGauss::shoot();
    zm = G4RandGauss::shoot();
  }
  else
  {
    x = 0;
    y = 0;
    z = 80 * m;

    xm = 0;
    ym = 0;
    zm = -1;
  }

  fParticleGun->SetParticlePosition(G4ThreeVector(x,y,z));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(xm,ym,zm));

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
