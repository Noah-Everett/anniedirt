//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: EventAction.cc,v 1.2 2012-01-23 21:16:50 rhatcher Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EventAction.hh"

#include "RunAction.hh"
#include "EventActionMessenger.hh"

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

#include "Randomize.hh"
#include <iomanip>

#include "MyEventRecord.hh"

#include "TString.h"  // for Form()
#include "TFile.h"
#include "TTree.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(MyEventRecord* myevtrec,
                         RunAction* run, G4int verbose)
  : fMyEventRecord(myevtrec)
  , runAct(run)
  , printModulo(1)
  , fVerbosity(verbose)
  , eventMessenger(0)
  , fOutputFile(0)
  , fOutputTree(0)
{
  eventMessenger = new EventActionMessenger(this);

  // create output file
  // pull run # from fMyEventRecord which was initialized by
  // PrimaryGeneratorAction ctor which opened the GENIE file

  std::string ofname = Form("annie_tank_flux.%d.root",fMyEventRecord->run);
  fOutputFile = TFile::Open(ofname.c_str(),"RECREATE");
  fOutputTree = new TTree("tankflux","ANNIE tank flux");

  fOutputTree->Branch("run",&(fMyEventRecord->run),"run/I");
  fOutputTree->Branch("entry",&(fMyEventRecord->entry),"entry/I");
  fOutputTree->Branch("iter",&(fMyEventRecord->iter),"iter/I");
  fOutputTree->Branch("niter",&(fMyEventRecord->niter),"niter/I");

  fOutputTree->Branch("nupdg",&(fMyEventRecord->nupdg),"nupdg/I");
  fOutputTree->Branch("nuvtxx",&(fMyEventRecord->nuvtxx),"nuvtxx/D");
  fOutputTree->Branch("nuvtxy",&(fMyEventRecord->nuvtxy),"nuvtxy/D");
  fOutputTree->Branch("nuvtxz",&(fMyEventRecord->nuvtxz),"nuvtxz/D");
  fOutputTree->Branch("nuvtxt",&(fMyEventRecord->nuvtxt),"nuvtxt/D");

  fOutputTree->Branch("intank",&(fMyEventRecord->intank),"intank/I");
  fOutputTree->Branch("inhall",&(fMyEventRecord->inhall),"inhall/I");

  // can I assume std::string.c_str() address won't change???
  // nope
  //fOutputTree->Branch("vtxvol",&(fMyEventRecord->vtxvol.c_str()),"vtxvol/C");
  //fOutputTree->Branch("vtxmat",&(fMyEventRecord->vtxmat.c_str()),"vtxmat/C");
  fOutputTree->Branch("vtxvol",&(fMyEventRecord->vtxvol[0]),"vtxvol/C");
  fOutputTree->Branch("vtxmat",&(fMyEventRecord->vtxmat[0]),"vtxmat/C");

  fOutputTree->Branch("ntank",&(fMyEventRecord->ntank),"ntank/I");

  fOutputTree->Branch("pdgtank",&(fMyEventRecord->pdgtank[0]),"pdgtank[ntank]/I");
  fOutputTree->Branch("primary",&(fMyEventRecord->primary[0]),"primary[ntank]/I");

  fOutputTree->Branch("vx",&(fMyEventRecord->vx[0]),"vx[ntank]/D");
  fOutputTree->Branch("vy",&(fMyEventRecord->vy[0]),"vy[ntank]/D");
  fOutputTree->Branch("vz",&(fMyEventRecord->vz[0]),"vz[ntank]/D");
  fOutputTree->Branch("vt",&(fMyEventRecord->vt[0]),"vt[ntank]/D");

  fOutputTree->Branch("px",&(fMyEventRecord->px[0]),"px[ntank]/D");
  fOutputTree->Branch("py",&(fMyEventRecord->py[0]),"py[ntank]/D");
  fOutputTree->Branch("pz",&(fMyEventRecord->pz[0]),"pz[ntank]/D");
  fOutputTree->Branch("E",&(fMyEventRecord->E[0]),"E[ntank]/D");
  fOutputTree->Branch("kE",&(fMyEventRecord->kE[0]),"kE[ntank]/D");


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{
  delete eventMessenger;

  double potsPerInput = 0;
  if (  fMyEventRecord->inputNEntries ) {
    potsPerInput = fMyEventRecord->inputTotalPOTs / 
                   (double)fMyEventRecord->inputNEntries;
  }
  fMyEventRecord->POTsPerPass = 
    (double)(fMyEventRecord->nG4BeamOn) * potsPerInput;
  fMyEventRecord->POTs = 
    (double)(fMyEventRecord->nG4Events) * potsPerInput;

  // close output file
  if ( fOutputFile ) {
    fOutputFile->cd();
    if ( fOutputTree ) {
      fOutputTree->Write();
      delete fOutputTree;
    }

    // metadata tree ...
    TTree* metaTree = new TTree("tankmeta","ANNIE tank flux metadata");
    
    // ack
    char inputFluxName[4096] = { '\0' };
    strncpy(inputFluxName,fMyEventRecord->inputFileName.c_str(),4095);

    metaTree->Branch("inputFluxName",inputFluxName,"inputFluxName/C");
    metaTree->Branch("inputNEntries",&(fMyEventRecord->inputNEntries),"inputNEntries/L");
    metaTree->Branch("inputTotalPOTs",&(fMyEventRecord->inputTotalPOTs),"inputTotalPOTs/D");

    metaTree->Branch("nG4Repeat",&(fMyEventRecord->nG4Repeat),"nG4Repeat/L");
    metaTree->Branch("nG4BeamOn",&(fMyEventRecord->nG4BeamOn),"nG4BeamOn/L");
    metaTree->Branch("nG4Events",&(fMyEventRecord->nG4Events),"nG4Events/L");
    metaTree->Branch("POTsPerPass",&(fMyEventRecord->POTsPerPass),"POTsPerPass/D");
    metaTree->Branch("POTs",&(fMyEventRecord->POTs),"POTs/D");

    metaTree->Fill();  // just once
    metaTree->Write();
    delete metaTree;

    fOutputFile->Close();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* evt)
{  
  G4int evtNb = evt->GetEventID();
  if (evtNb%printModulo == 0 && fVerbosity > 0) { 
    G4cout << "---> Begin of event: " << evtNb << G4endl;
    CLHEP::HepRandom::showEngineStatus();
  }
  
  G4PrimaryVertex* vtx0 = evt->GetPrimaryVertex();
  if ( ! vtx0 ) {
    G4cerr << "no primary vertex" << G4endl;
    return;
  }
  if ( ! vtx0->GetPrimary() ) {
    G4cerr << "no primary particle" << G4endl;
    return;
  }

  if ( fVerbosity > 1 ) {
    G4cout << "BeginOfEventAction: " << G4endl;
    evt->Print();
    evt->GetPrimaryVertex()->Print();
    //    evt->GetPrimaryVertex()->GetPrimary()->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* evt)
{

  ++(fMyEventRecord->nG4Events);  // total # of GENIE events processed

  if ( fMyEventRecord->ntank > 0 ) {
    // write this in output file -- already have branches attached
    // to fMyEventRecord
    fOutputTree->Fill();
    
    std::cout << (*fMyEventRecord) << std::endl;
    //if ( fMyEventRecord->intank == 0 ) {
    //  std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ that one" << std::endl;
    //}
  }

  //print per event (modulo n)
  //
  G4int evtNb = evt->GetEventID();
  if (evtNb%printModulo == 0 && fVerbosity > 0 ) {
    G4cout << "---> End of event: " << G4endl;
  }
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
