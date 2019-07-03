#include <iostream>
#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TStyle.h"
#include "TH1I.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "THStack.h"

#define filemm "results/waterTube/10000_10GeV/muminus.root"
#define filemp "results/waterTube/10000_10GeV/muplus.root"
#define treename "muonData;1"
#define numevents 10000

#define plotmode 2

#define part "neutron"
#define valname "finalZ"
#define title "Final Z of Initial Muons in m"
#define bins 100
#define xmin 30
#define xmax 80
#define ymin 0.5
#define ymax 1e3
#define ignoreParent true

using namespace std;

void plotBoth(TTree* tmm, TTree* tmp);
void plotNumNeutronsFromCapture(TTree* tmm);
void plotBothInitials(TTree* tmm, TTree* tmp);
void printProcess(TTree* tmm, TTree* tmp);
void printPhotonuclearProducts(TTree* tmm, TTree* tmp);
bool streq(const Char_t* s1, const Char_t* s2);

int main(int argc,char** argv) 
{
  TApplication theApp("App", &argc, argv);

  TCanvas *c1 = new TCanvas("c1", "c1", 1600, 800);
  c1->Connect("Closed()", "TApplication", &theApp, "Terminate()");

  gStyle->SetPalette(1);
  gROOT->SetStyle("Plain");

  c1->SetLogy();

  TFile fmm(filemm);
  TFile fmp(filemp);

  TTree* tmm = static_cast<TTree*>(fmm.Get(treename));
  TTree* tmp = static_cast<TTree*>(fmp.Get(treename));

  switch(plotmode)
  {
  case 0: plotBoth(tmm, tmp);
    break;
  case 1: plotNumNeutronsFromCapture(tmm);
    break;
  case 2: plotBothInitials(tmm, tmp);
    break;
  case 3: printProcess(tmm, tmp);
    break;
  case 4: printPhotonuclearProducts(tmm, tmp);
    break;
  default:;
  }

  c1->Update();
  theApp.Run();

  return 0;
}

void plotBoth(TTree* tmm, TTree* tmp)
{
  static Char_t particlemm[50], particlemp[50];
  static Double_t valmm, valmp;
  static Int_t parentmm, parentmp;

  tmm->SetBranchAddress("particle", &particlemm);
  tmp->SetBranchAddress("particle", &particlemp);

  tmm->SetBranchAddress(valname, &valmm);
  tmp->SetBranchAddress(valname, &valmp);

  tmm->SetBranchAddress("parentID", &parentmm);
  tmp->SetBranchAddress("parentID", &parentmp);

  TH1I *histmm = new TH1I("mu-", title, bins, xmin, xmax);
  TH1I *histmp = new TH1I("mu+", title, bins, xmin, xmax);

  Long64_t nentriesmm = tmm->GetEntries();
  Long64_t nentriesmp = tmp->GetEntries();

  for (Long64_t i=0; i < nentriesmm; i++)
  {
    tmm->GetEntry(i);

    if(streq(particlemm, part) && (parentmm == 1 || ignoreParent))
    {
      histmm->Fill(valmm);
    }
  }

  for (Long64_t i=0; i < nentriesmp; i++)
  {
    tmp->GetEntry(i);

    if(streq(particlemp, part) && (parentmp == 1 || ignoreParent))
    {
      histmp->Fill(valmp);
    }
  }

  THStack *hs = new THStack("hs", title);

  histmm->SetLineColor(2);
  histmp->SetLineColor(1);

  hs->Add(histmm);
  hs->Add(histmp);

  hs->Draw("nostack");
  hs->SetMinimum(ymin);
  hs->SetMaximum(ymax);
  hs->Draw("nostack");
}

void plotNumNeutronsFromCapture(TTree* tmm)
{
  static Int_t event, id, parent;
  static Char_t particle[50], process[50];
  tmm->SetBranchAddress("eventID",  &event);
  tmm->SetBranchAddress("trackID",  &id);
  tmm->SetBranchAddress("parentID", &parent);
  tmm->SetBranchAddress("particle", &particle);
  tmm->SetBranchAddress("process", &process);

  TH1F *hist = new TH1F("Neutrons", "Number of Neutrons per Capture", bins, xmin, xmax);

  Int_t nneutrons[numevents];
  Bool_t haddecay[numevents];

  for(Int_t i = 0; i < numevents; i++)
  {
    nneutrons[i] = 0;
    haddecay[i] = false;
  }

  Long64_t nentries = tmm->GetEntries();

  for (Long64_t i = 0; i < nentries; i++)
  {
    tmm->GetEntry(i);

    if(streq("anti_nu_e", particle) && streq("muMinusCaptureAtRest", process) && parent == 1)
    {
      haddecay[event] = true;
    }

    if(streq("neutron", particle) && streq("muMinusCaptureAtRest", process) && parent == 1)
    {
      nneutrons[event] ++;
    }
  }

  for(Int_t i = 0; i < numevents; i++)
  {
    if(!haddecay[i])
    {
      hist->Fill(nneutrons[i]);
    }
  }

  hist->SetLineColor(2);
  hist->Draw();
  hist->SetMinimum(ymin);
  hist->SetMaximum(ymax);
  hist->Draw();
}

void plotBothInitials(TTree* tmm, TTree* tmp)
{
  static Int_t idmm, idmp;
  static Char_t particlemm[50], particlemp[50];
  static Double_t valmm, valmp;

  tmm->SetBranchAddress("particle", &particlemm);
  tmp->SetBranchAddress("particle", &particlemp);

  tmm->SetBranchAddress("trackID", &idmm);
  tmp->SetBranchAddress("trackID", &idmp);

  tmm->SetBranchAddress(valname, &valmm);
  tmp->SetBranchAddress(valname, &valmp);

  TH1I *histmm = new TH1I("mu-", title, bins, xmin, xmax);
  TH1I *histmp = new TH1I("mu+", title, bins, xmin, xmax);

  Long64_t nentriesmm = tmm->GetEntries();
  Long64_t nentriesmp = tmp->GetEntries();

  for (Long64_t i=0; i < nentriesmm; i++)
  {
    tmm->GetEntry(i);

    if(streq("mu-", particlemm) && idmm == 1)
    {
      histmm->Fill(valmm);
    }
  }

  for (Long64_t i=0; i < nentriesmp; i++)
  {
    tmp->GetEntry(i);

    if(streq("mu+", particlemp) && idmp == 1)
    {
      histmp->Fill(valmp);
    }
  }

  THStack *hs = new THStack("hs", title);

  histmm->SetLineColor(2);
  histmp->SetLineColor(1);

  hs->Add(histmm);
  hs->Add(histmp);

  hs->Draw("nostack");
  hs->SetMinimum(ymin);
  hs->SetMaximum(ymax);
  hs->Draw("nostack");
}

void printProcess(TTree* tmm, TTree* tmp)
{
  static Char_t particlemm[50], particlemp[50], processmm[50], processmp[50];

  tmm->SetBranchAddress("particle", &particlemm);
  tmp->SetBranchAddress("particle", &particlemp);
  tmm->SetBranchAddress("process", &processmm);
  tmp->SetBranchAddress("process", &processmp);

  Long64_t nentriesmm = tmm->GetEntries();
  Long64_t nentriesmp = tmp->GetEntries();

  cout << "###mm" << endl;

  for (Long64_t i=0; i < nentriesmm; i++)
  {
    tmm->GetEntry(i);

    if(streq(part, particlemm))
    {
      cout << processmm << endl;
    }
  }

  cout << "###mp" << endl;

  for (Long64_t i=0; i < nentriesmp; i++)
  {
    tmp->GetEntry(i);

    if(streq(part, particlemp))
    {
      cout << processmp << endl;
    }
  }
}

void printPhotonuclearProducts(TTree* tmm, TTree* tmp)
{
  static Char_t particlemm[50], particlemp[50], processmm[50], processmp[50];
  static Int_t eventmm, eventmp, parentmm, parentmp;

  tmm->SetBranchAddress("particle", &particlemm);
  tmp->SetBranchAddress("particle", &particlemp);
  tmm->SetBranchAddress("process", &processmm);
  tmp->SetBranchAddress("process", &processmp);
  tmm->SetBranchAddress("eventID", &eventmm);
  tmp->SetBranchAddress("eventID", &eventmp);
  tmm->SetBranchAddress("parentID", &parentmm);
  tmp->SetBranchAddress("parentID", &parentmp);

  Long64_t nentriesmm = tmm->GetEntries();
  Long64_t nentriesmp = tmp->GetEntries();

  cout << "###mm" << endl;

  for (Long64_t i=0; i < nentriesmm; i++)
  {
    tmm->GetEntry(i);

    if(streq("photonNuclear", processmm))
    {
      cout << eventmm << ": " << particlemm << "<" << parentmm << endl;
    }
  }

  cout << "###mp" << endl;

  for (Long64_t i=0; i < nentriesmp; i++)
  {
    tmp->GetEntry(i);

    if(streq("photonNuclear", processmp))
    {
      cout << eventmp << ": " << particlemp << "<" << parentmp << endl;
    }
  }
}

bool streq(const Char_t* s1, const Char_t* s2)
{
  return !strncmp(s1, s2, max(strlen(s1), strlen(s2)));
}
