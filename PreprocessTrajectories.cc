#include <iostream>
#include <numeric>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;

vector<size_t> getPermutation(const vector<Int_t>& vec);
template <typename T>
void applyPermutation(vector<T>& vec, const vector<size_t>& p);

int main(int argc, char** argv)
{
  Int_t event, track, parent;
  Char_t particle[50], process[50];
  Double_t energy,
      xmomentum, ymomentum, zmomentum,
      initialx,  initialy,  initialz,
      finalx,    finaly,    finalz;

  Int_t prevevent = 0;
  vector<Int_t> trackVec, parentVec;
  vector<string> particleVec, processVec;
  vector<Double_t> energyVec,
      xmomentumVec, ymomentumVec, zmomentumVec,
      initialxVec,  initialyVec,  initialzVec,
      finalxVec,    finalyVec,    finalzVec;

  TFile* inputFile = new TFile(argv[1]);
  TTree* inputTree = static_cast<TTree*>(inputFile->Get("muonData;1"));
  inputTree->SetBranchAddress("eventID",   &event);
  inputTree->SetBranchAddress("trackID",   &track);
  inputTree->SetBranchAddress("parentID",  &parent);
  inputTree->SetBranchAddress("particle",  &particle);
  inputTree->SetBranchAddress("process",   &process);
  inputTree->SetBranchAddress("kenergy",   &energy);
  inputTree->SetBranchAddress("xMomentum", &xmomentum);
  inputTree->SetBranchAddress("yMomentum", &ymomentum);
  inputTree->SetBranchAddress("zMomentum", &zmomentum);
  inputTree->SetBranchAddress("initialX",  &initialx);
  inputTree->SetBranchAddress("initialY",  &initialy);
  inputTree->SetBranchAddress("initialZ",  &initialz);
  inputTree->SetBranchAddress("finalX",    &finalx);
  inputTree->SetBranchAddress("finalY",    &finaly);
  inputTree->SetBranchAddress("finalZ",    &finalz);

  TFile* outputFile = new TFile(argv[2], "RECREATE");
  outputFile->cd();

  TTree* outputTree = new TTree("muonData", "Processed Muon Data");
  outputTree->Branch("eventID",   &prevevent);
  outputTree->Branch("trackID",   &trackVec);
  outputTree->Branch("parentID",  &parentVec);
  outputTree->Branch("particle",  &particleVec);
  outputTree->Branch("process",   &processVec);
  outputTree->Branch("kenergy",   &energyVec);
  outputTree->Branch("xMomentum", &xmomentumVec);
  outputTree->Branch("yMomentum", &ymomentumVec);
  outputTree->Branch("zMomentum", &zmomentumVec);
  outputTree->Branch("initialX",  &initialxVec);
  outputTree->Branch("initialY",  &initialyVec);
  outputTree->Branch("initialZ",  &initialzVec);
  outputTree->Branch("finalX",    &finalxVec);
  outputTree->Branch("finalY",    &finalyVec);
  outputTree->Branch("finalZ",    &finalzVec);

  Long_t nentries = inputTree->GetEntries();

  for(Long_t i = 0; i <= nentries; i++)
  {
    if(i < nentries) inputTree->GetEntry(i);

    if(event != prevevent || i == nentries)
    {
      if(prevevent % 100 == 0)
      {
        cout << "Filling " << prevevent << endl;
      }

      auto p = getPermutation(trackVec);

      applyPermutation(trackVec,     p);
      applyPermutation(parentVec,    p);
      applyPermutation(particleVec,  p);
      applyPermutation(processVec,   p);
      applyPermutation(energyVec,    p);
      applyPermutation(xmomentumVec, p);
      applyPermutation(ymomentumVec, p);
      applyPermutation(zmomentumVec, p);
      applyPermutation(initialxVec,  p);
      applyPermutation(initialyVec,  p);
      applyPermutation(initialzVec,  p);
      applyPermutation(finalxVec,    p);
      applyPermutation(finalyVec,    p);
      applyPermutation(finalzVec,    p);

      outputTree->Fill();

      prevevent = event;

      trackVec    .clear();
      parentVec   .clear();
      particleVec .clear();
      processVec  .clear();
      energyVec   .clear();
      xmomentumVec.clear();
      ymomentumVec.clear();
      zmomentumVec.clear();
      initialxVec .clear();
      initialyVec .clear();
      initialzVec .clear();
      finalxVec   .clear();
      finalyVec   .clear();
      finalzVec   .clear();

      if(i == nentries) break;
    }

    trackVec    .push_back(track);
    parentVec   .push_back(parent);
    particleVec .push_back(particle);
    processVec  .push_back(process);
    energyVec   .push_back(energy);
    xmomentumVec.push_back(xmomentum);
    ymomentumVec.push_back(ymomentum);
    zmomentumVec.push_back(zmomentum);
    initialxVec .push_back(initialx);
    initialyVec .push_back(initialy);
    initialzVec .push_back(initialz);
    finalxVec   .push_back(finalx);
    finalyVec   .push_back(finaly);
    finalzVec   .push_back(finalz);
  }

  outputFile->Write();
  outputFile->Close();
  inputFile->Close();
  return 0;
}

vector<size_t> getPermutation(const vector<Int_t>& vec)
{
  vector<size_t> p(vec.size());
  iota(p.begin(), p.end(), 0);
  sort(p.begin(), p.end(),
            [vec](size_t i, size_t j){return vec[i] < vec[j];});
  return p;
}

template <typename T>
void applyPermutation(vector<T>& vec, const vector<size_t>& p)
{
  vector<bool> done(vec.size());
  for (size_t i = 0; i < vec.size(); ++i)
  {
    if (done[i])
    {
      continue;
    }
    done[i] = true;
    size_t prev_j = i;
    size_t j = p[i];
    while (i != j)
    {
      swap(vec[prev_j], vec[j]);
      done[j] = true;
      prev_j = j;
      j = p[j];
    }
  }
}
