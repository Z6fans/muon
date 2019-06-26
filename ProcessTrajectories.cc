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

#define filemm "results/waterTube/10000_1GeV/muminus.root"
#define filemp "results/waterTube/10000_1GeV/muplus.root"
#define treename "cylinderTest;1"
#define numevents 10000

#define plotmode 1

#define part 11
#define valname "initialZ"
#define title "Initial Z of Neutrons in m"
#define bins 20
#define xmin 0
#define xmax 5
#define ymin 0.5
#define ymax 1e3

using namespace std;

void plotBoth(TTree* tmm, TTree* tmp);
void plotNumNeutronsFromCapture(TTree* tmm);
void plotBothInitials(TTree* tmm, TTree* tmp);

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

  TTree* tmm = (TTree*)fmm.Get(treename);
  TTree* tmp = (TTree*)fmp.Get(treename);

  switch(plotmode)
  {
  case 0: plotBoth(tmm, tmp);
    break;
  case 1: plotNumNeutronsFromCapture(tmm);
    break;
  case 2: plotBothInitials(tmm, tmp);
    break;
  default:;
  }

  c1->Update();
  theApp.Run();
  return 0;
}

void plotBoth(TTree* tmm, TTree* tmp)
{
  static Int_t particlemm, particlemp;
  static Double_t valmm, valmp;

  tmm->SetBranchAddress("particle", &particlemm);
  tmp->SetBranchAddress("particle", &particlemp);

  tmm->SetBranchAddress(valname, &valmm);
  tmp->SetBranchAddress(valname, &valmp);

  TH1I *histmm = new TH1I("mu-", title, bins, xmin, xmax);
  TH1I *histmp = new TH1I("mu+", title, bins, xmin, xmax);

  Int_t nentriesmm = (Int_t)tmm->GetEntries();
  Int_t nentriesmp = (Int_t)tmp->GetEntries();

  for (Int_t i=0; i < nentriesmm; i++)
  {
     tmm->GetEntry(i);

     if(particlemm == part)
     {
       histmm->Fill(valmm);
     }
  }

  for (Int_t i=0; i < nentriesmp; i++)
  {
     tmp->GetEntry(i);

     if(particlemp == part)
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
  static Int_t event, id, parent, particle;
  static Double_t xi, yi, zi, xf, yf, zf;
  tmm->SetBranchAddress("eventID",  &event);
  tmm->SetBranchAddress("trackID",  &id);
  tmm->SetBranchAddress("parentID", &parent);
  tmm->SetBranchAddress("particle", &particle);
  tmm->SetBranchAddress("initialX",  &xi);
  tmm->SetBranchAddress("initialY",  &yi);
  tmm->SetBranchAddress("initialZ",  &zi);
  tmm->SetBranchAddress("finalX",   &xf);
  tmm->SetBranchAddress("finalY",   &yf);
  tmm->SetBranchAddress("finalZ",   &zf);

  TH1F *hist = new TH1F("Neutrons", "Number of Neutrons per Capture", bins, xmin, xmax);

  Int_t nneutrons[numevents];
  Bool_t haddecay[numevents];
  Double_t mux[numevents];
  Double_t muy[numevents];
  Double_t muz[numevents];

  for(Int_t i = 0; i < numevents; i++)
  {
    nneutrons[i] = 0;
    haddecay[i] = false;
    mux[i] = 0;
    muy[i] = 0;
    muz[i] = 0;
  }

  Int_t nentries = (Int_t)tmm->GetEntries();

  for (Int_t i = 0; i < nentries; i++)
  {
     tmm->GetEntry(i);

     if(particle == 7 && parent == 1)
     {
       haddecay[event] = true;
     }

     if(particle == 4 && id == 1)
     {
       mux[event] = xf;
       muy[event] = yf;
       muz[event] = zf;
     }
  }

  for(Int_t i = 0; i < nentries; i++)
  {
    tmm->GetEntry(i);

    if(particle == 11 && parent == 1)
    {
      Double_t dx = xi - mux[event];
      Double_t dy = yi - muy[event];
      Double_t dz = zi - muz[event];
      Double_t dd = dx*dx + dy*dy + dz*dz;

      if(dd <= 0.001)
      {
        nneutrons[event] ++;
      }
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
  static Int_t particlemm, particlemp, idmm, idmp;
  static Double_t valmm, valmp;

  tmm->SetBranchAddress("particle", &particlemm);
  tmp->SetBranchAddress("particle", &particlemp);

  tmm->SetBranchAddress("trackID", &idmm);
  tmp->SetBranchAddress("trackID", &idmp);

  tmm->SetBranchAddress(valname, &valmm);
  tmp->SetBranchAddress(valname, &valmp);

  TH1I *histmm = new TH1I("mu-", title, bins, xmin, xmax);
  TH1I *histmp = new TH1I("mu+", title, bins, xmin, xmax);

  Int_t nentriesmm = (Int_t)tmm->GetEntries();
  Int_t nentriesmp = (Int_t)tmp->GetEntries();

  for (Int_t i=0; i < nentriesmm; i++)
  {
     tmm->GetEntry(i);

     if(particlemm == 4 && idmm == 1)
     {
       histmm->Fill(valmm);
     }
  }

  for (Int_t i=0; i < nentriesmp; i++)
  {
     tmp->GetEntry(i);

     if(particlemp == 3 && idmp == 1)
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
