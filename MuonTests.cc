#include "G4MuonMinusBoundDecay.hh"
#include "G4Nucleus.hh"
#include "G4NistManager.hh"

int main(int argc,char** argv)
{
  G4double lambdac = G4MuonMinusBoundDecay::GetMuonCaptureRate(8, 16);
  G4double lambdad = G4MuonMinusBoundDecay::GetMuonDecayRate(8);
  G4double lambda = lambdac + lambdad;
  G4cout << (lambdac / lambda) << G4endl;

  lambdac = G4MuonMinusBoundDecay::GetMuonCaptureRate(1, 1);
  lambdad = G4MuonMinusBoundDecay::GetMuonDecayRate(1);
  lambda = lambdac + lambdad;
  G4cout << (lambdac / lambda) << G4endl;

  return 0;
}
