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
// $Id: SteppingAction.cc,v 1.3 2012-11-14 21:12:51 rhatcher Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"

#include "G4VUserDetectorConstruction.hh"
#include "EventAction.hh"

#include "G4LogicalVolume.hh"
#include "G4Material.hh"

#include "G4Step.hh"

#include "MyEventRecord.hh"

////#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(MyEventRecord* myevtrec,
                               G4VUserDetectorConstruction* det,
                               EventAction* evt, G4int verbose)
  : fMyEventRecord(myevtrec)
  , detector(det)
  , eventaction(evt)
  , fVerbosity(verbose)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  if ( preStepPoint &&
       preStepPoint->GetTouchableHandle() ) {
    G4VPhysicalVolume* pv_start = 
      preStepPoint->GetTouchableHandle()->GetVolume();
    if ( pv_start ) {
      G4LogicalVolume*   lv_start = pv_start->GetLogicalVolume();
      G4Material*       mat_start = lv_start->GetMaterial();
    } else {
      std::cerr << "no pv_start" << std::endl;
    }
  }

  // want to catch things _as_ they enter TWATER_PV
  // but be wary of stuff

  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  if ( postStepPoint &&
       postStepPoint->GetTouchableHandle() ) {
    G4VPhysicalVolume* pv_end = 
      postStepPoint->GetTouchableHandle()->GetVolume();
    if ( ! pv_end ) return;

    G4LogicalVolume*   lv_end = pv_end->GetLogicalVolume();
    G4Material*       mat_end = lv_end->GetMaterial();
   
    if ( pv_end->GetName() == "TWATER_PV" ) {
      // record this info and kill the particle
      G4Track* aTrack = aStep->GetTrack();
      fMyEventRecord->AppendG4Step(aTrack,postStepPoint);
      aTrack->SetTrackStatus(fStopAndKill);

    }
 
  }

  /*
  // get volume of the current step
  G4VPhysicalVolume* pvolume
    = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  G4LogicalVolume* lvolume = pvolume->GetLogicalVolume();
  G4Material* material = lvolume->GetMaterial();

  // if ( material->GetName() != "Scintillator" ) return;

  //  int pdg = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
  // this should work for both g4.9.4 and g4.9.3
  int pdg = aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding();

  if ( // aStep->GetTrack()->GetKineticEnergy() > 1.0*GeV  &&
       // aStep->GetTrack()->GetTrackID() == 1 &&
       fVerbosity > 0 ) {
    G4cout << " step in " << material->GetName()
           << " PDG " << pdg
           << " trackid " << aStep->GetTrack()->GetTrackID()
           << " KE "  << aStep->GetTrack()->GetKineticEnergy()
           << " begin at " << (aStep->GetPreStepPoint()->GetPosition()/cm)
           << " end at " << (aStep->GetPostStepPoint()->GetPosition()/cm)
           << G4endl;
  }

  // collect energy and track length step by step
  G4double edep = aStep->GetTotalEnergyDeposit();
  
  //G4double stepl = 0.;
  //if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.)
  //  stepl = aStep->GetStepLength();

  G4ThreeVector avgPos =
    0.5 * ( aStep->GetPreStepPoint()->GetPosition() +
            aStep->GetPostStepPoint()->GetPosition()  );

  ///no!//G4Track* track = aStep->GetTrack();
  ///no!//G4ThreeVector rayOrigin = track->GetPosition();
  ///no!//G4ThreeVector rayDircos = track->GetMomentumDirection();

  
  //N03//if (volume == detector->GetAbsorber()) eventaction->AddAbs(edep,stepl);
  //N03//if (volume == detector->GetGap())      eventaction->AddGap(edep,stepl);
  
  //example of saving random number seed of this event, under condition
  //// if (condition) G4RunManager::GetRunManager()->rndmSaveThisEvent(); 
  */

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
double SteppingAction::distRayToPoint(const G4ThreeVector& rayOrigin,
                                      const G4ThreeVector& rayDircos,
                                      const G4ThreeVector& point)
{
  // calculate minimum distance from a line (ray) in 3-space to a point
  // line a a function of one variable "s"
  // points on the line are:  p_line = rayOrigin + rayDircos*s
  // find "s" that minimizes
  //    sqrt ( sum_i ( rayOrigin[i] + rayDircos[i]*s - point[i] )^2 ) = Q
  // which can be minimize by minimizing the square of Q
  // take derivative and set equal to zero
  double sumNum = 0;
  double sumDen = 0;
  for (int i = 0; i < 3; ++i ) {
    sumNum += rayDircos[i]*(point[i]-rayOrigin[i]);
    sumDen += rayDircos[i]*rayDircos[i];
  }
  double s = 0;
  if ( sumDen != 0.0 ) {
    s = sumNum / sumDen;
  } else {
    G4cout << "distRayToPoint sumDen == 0" << G4endl;
  }
  double sumd2 = 0;
  G4ThreeVector pline;
  for (int i = 0; i < 3; ++i ) {
    pline[i]  = rayOrigin[i] + rayDircos[i]*s;
    double d1 = pline[i] - point[i];
    sumd2    += d1*d1;
  }

  return sqrt(sumd2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
