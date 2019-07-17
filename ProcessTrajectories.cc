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

using namespace std;

typedef vector<Int_t>    ivec;
typedef vector<string>   svec;
typedef vector<Double_t> dvec;

typedef struct
{
  enum{STRING, DOUBLE, NONE} tag;
  string s;
  Double_t n;
}
result;

typedef function<void (result)> yielder;
typedef function<void (TTree*, yielder)> treeprocess;
typedef function<void (ivec*, svec*, svec*, dvec*,
                       dvec*, dvec*, dvec*,
                       dvec*, dvec*, dvec*,
                       dvec*, dvec*, dvec*, yielder)> eventprocess;
typedef function<result (Int_t, string, string, Double_t,
                         Double_t, Double_t, Double_t,
                         Double_t, Double_t, Double_t,
                         Double_t, Double_t, Double_t)> trajprocess;

eventprocess iterTrajectories(trajprocess computeresult);
treeprocess iterEvents(eventprocess fill);
void twoHist(TTree* tmm, TTree* tmp, treeprocess fill,
             string title, Int_t bins,
             Double_t xmin, Double_t xmax,
             Double_t ymax, Double_t ymin = 0.5);
void oneHist(TTree* t, treeprocess fill,
             string title, string shorttitle, Int_t bins,
             Double_t xmin, Double_t xmax,
             Double_t ymax, Double_t ymin = 0.5);
void printYielder(result r);
void printBoth(TTree* tmm, TTree* tmp, treeprocess fill);

int main(int argc,char** argv)
{
  string datadir = argv[1];

  TApplication theApp("App", &argc, argv);

  TCanvas *c1 = new TCanvas("c1", "c1", 1600, 800);
  c1->Connect("Closed()", "TApplication", &theApp, "Terminate()");

  gStyle->SetPalette(1);
  gROOT->SetStyle("Plain");

  c1->SetLogy();

  TFile fmm((datadir + "muminus.root").data());
  TFile fmp((datadir + "muplus.root").data());

  TTree* tmm = static_cast<TTree*>(fmm.Get("muonData"));
  TTree* tmp = static_cast<TTree*>(fmp.Get("muonData"));

  auto compres = [](Int_t, string particle, string process, Double_t,
      Double_t, Double_t, Double_t,
      Double_t, Double_t, Double_t,
      Double_t, Double_t, Double_t)
  {
    if(!particle.compare("neutron"))
    {
      return result {result::STRING, process, 0};
    }
    else
    {
      return result {result::NONE, "", 0};
    }
  };

  /*twoHist(tmm, tmp, iterEvents(iterTrajectories(compres)),
          "Final Z of Initial Muons in m", 240, 74, 80, 1e6);*/

  printBoth(tmm, tmp, iterEvents(iterTrajectories(compres)));

  c1->Update();
  theApp.Run();

  return 0;
}

/*void plotNumNeutronsFromCapture(TTree* tmm)
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

void printCaptureProducts(TTree* tmm)
{
  static Char_t particle[50], process[50];
  static Int_t event, parent;

  tmm->SetBranchAddress("particle", &particle);
  tmm->SetBranchAddress("process", &process);
  tmm->SetBranchAddress("eventID", &event);
  tmm->SetBranchAddress("parentID", &parent);

  Bool_t haddecay[numevents];

  for(Int_t i = 0; i < numevents; i++)
  {
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
  }

  for (Long64_t i=0; i < nentries; i++)
  {
    tmm->GetEntry(i);

    if(streq("muMinusCaptureAtRest", process) && parent == 1 && !haddecay[event])
    {
      cout << event << ": " << particle << endl;
    }
  }
}*/

eventprocess iterTrajectories(trajprocess computeresult)
{
  return [=](ivec* parents, svec* particles, svec* processes, dvec* energies,
      dvec* xmomenta,  dvec* ymomenta,  dvec* zmomenta,
      dvec* initialxs, dvec* initialys, dvec* initialzs,
      dvec* finalxs,   dvec* finalys,   dvec* finalzs, yielder yield)
  {
    unsigned long ntracks = particles->size();

    for(unsigned long i = 0; i < ntracks; i++)
    {
      yield(computeresult(parents  ->at(i), particles->at(i), processes->at(i), energies->at(i),
                          xmomenta ->at(i), ymomenta ->at(i), zmomenta ->at(i),
                          initialxs->at(i), initialys->at(i), initialzs->at(i),
                          finalxs  ->at(i), finalys  ->at(i), finalzs  ->at(i)));
    }
  };
}

treeprocess iterEvents(eventprocess fill)
{
  return [=](TTree* t, yielder yield)
  {
    ivec* parents   = nullptr;
    svec* particles = nullptr;
    svec* processes = nullptr;
    dvec* energies  = nullptr;
    dvec* xmomenta  = nullptr;
    dvec* ymomenta  = nullptr;
    dvec* zmomenta  = nullptr;
    dvec* initialxs = nullptr;
    dvec* initialys = nullptr;
    dvec* initialzs = nullptr;
    dvec* finalxs   = nullptr;
    dvec* finalys   = nullptr;
    dvec* finalzs   = nullptr;

    t->SetBranchAddress("parentID",  &parents);
    t->SetBranchAddress("particle",  &particles);
    t->SetBranchAddress("process",   &processes);
    t->SetBranchAddress("kenergy",   &energies);
    t->SetBranchAddress("xMomentum", &xmomenta);
    t->SetBranchAddress("yMomentum", &ymomenta);
    t->SetBranchAddress("zMomentum", &zmomenta);
    t->SetBranchAddress("initialX",  &initialxs);
    t->SetBranchAddress("initialY",  &initialys);
    t->SetBranchAddress("initialZ",  &initialzs);
    t->SetBranchAddress("finalX",    &finalxs);
    t->SetBranchAddress("finalY",    &finalys);
    t->SetBranchAddress("finalZ",    &finalzs);

    Long64_t nevents = t->GetEntries();

    for (Long64_t i = 0; i < nevents; i++)
    {
      t->GetEntry(i);

      fill(parents, particles, processes, energies,
           xmomenta, ymomenta, zmomenta,
           initialxs, initialys, initialzs,
           finalxs, finalys, finalzs, yield);
    }
  };
}

void twoHist(TTree* tmm, TTree* tmp, treeprocess fill,
             string title, Int_t bins,
             Double_t xmin, Double_t xmax,
             Double_t ymax, Double_t ymin)
{
  TH1F *histmm = new TH1F("mu-", title.data(), bins, xmin, xmax);
  TH1F *histmp = new TH1F("mu+", title.data(), bins, xmin, xmax);

  fill(tmm, [=](result r){if(r.tag == result::DOUBLE) histmm->Fill(r.n);});
  fill(tmp, [=](result r){if(r.tag == result::DOUBLE) histmp->Fill(r.n);});

  THStack *hs = new THStack("hs", title.data());

  histmm->SetLineColor(2);
  histmp->SetLineColor(1);

  hs->Add(histmm);
  hs->Add(histmp);

  hs->Draw("nostack");
  hs->SetMinimum(ymin);
  hs->SetMaximum(ymax);
  hs->Draw("nostack");
}

void oneHist(TTree* t, treeprocess fill,
             string title, string shorttitle, Int_t bins,
             Double_t xmin, Double_t xmax,
             Double_t ymax, Double_t ymin)
{
  TH1F *hist = new TH1F(shorttitle.data(), title.data(), bins, xmin, xmax);

  fill(t, [=](result r){if(r.tag == result::DOUBLE) hist->Fill(r.n);});

  hist->SetLineColor(2);
  hist->Draw();
  hist->SetMinimum(ymin);
  hist->SetMaximum(ymax);
  hist->Draw();
}

void printYielder(result r)
{
  if(r.tag == result::STRING)
  {
    cout << r.s << endl;
  }
  else if(r.tag == result::DOUBLE)
  {
    cout << r.n << endl;
  }
}

void printBoth(TTree* tmm, TTree* tmp, treeprocess fill)
{
  cout << "###mm" << endl;
  fill(tmm, printYielder);

  cout << "###mp" << endl;
  fill(tmp, printYielder);
}
