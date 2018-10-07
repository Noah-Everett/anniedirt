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
// $Id: PrimaryGeneratorMessenger.cc,v 1.1.1.1 2011-12-06 17:49:40 rhatcher Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithADouble.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(
                                          PrimaryGeneratorAction* Gun)
:Action(Gun)
{
  /*
  gunDir = new G4UIdirectory("/g4sa/gun/");
  gunDir->SetGuidance("PrimaryGenerator control");
   
  RndmCmd = new G4UIcmdWithAString("/g4sa/gun/rndm",this);
  RndmCmd->SetGuidance("Shoot randomly the incident particle.");
  RndmCmd->SetGuidance("  Choice : on(default), off");
  RndmCmd->SetParameterName("choice",true);
  RndmCmd->SetDefaultValue("on");
  RndmCmd->SetCandidates("on off");
  RndmCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  PDGCodeCmd = new G4UIcmdWithAnInteger("/g4sa/gun/pdgcode",this);
  PDGCodeCmd->SetGuidance("Select particle PDG code to shoot.");
  PDGCodeCmd->SetParameterName("PDG",false);
  PDGCodeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  EnergyCmd = new G4UIcmdWithADoubleAndUnit("/g4sa/gun/energy",this);
  EnergyCmd->SetGuidance("Set particle energy.");
  EnergyCmd->SetParameterName("energy",false);
  EnergyCmd->SetUnitCategory("Energy");
  EnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  OriginCmd = new G4UIcmdWith3VectorAndUnit("/g4sa/gun/origin",this);
  OriginCmd->SetGuidance("Set origin for particle gun.");
  OriginCmd->SetParameterName("x","y","z",true,true);
  OriginCmd->SetUnitCategory("Length");
  OriginCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  OriginSmearCmd = new G4UIcmdWith3VectorAndUnit("/g4sa/gun/osmear",this);
  OriginSmearCmd->SetGuidance("Set smear on origin for particle gun.");
  OriginSmearCmd->SetGuidance("Variation will be +/- this in each direction.");
  OriginSmearCmd->SetParameterName("dx","dy","dz",true,true);
  OriginSmearCmd->SetUnitCategory("Length");
  OriginSmearCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  DirectionCmd = new G4UIcmdWith3Vector("/g4sa/gun/direction",this);
  DirectionCmd->SetGuidance("Set direction cosines for particle gun.");
  DirectionCmd->SetGuidance("Components will be normalized to 1.");
  DirectionCmd->SetParameterName("dirx","diry","dirz",true,true);
  DirectionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  DirThetaCmd = new G4UIcmdWithADouble("/g4sa/gun/dirtheta",this);
  DirThetaCmd->SetGuidance("Smear direction flat up to theta (random phi).");
  DirThetaCmd->SetParameterName("theta",false);
  DirThetaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  /*
  delete DirThetaCmd;
  delete DirectionCmd;
  delete OriginSmearCmd;
  delete OriginCmd;
  delete EnergyCmd;
  delete PDGCodeCmd;
  delete RndmCmd;
  delete gunDir;
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(
                                        G4UIcommand* command, G4String newValue)
{ 

  /*
  if( command == RndmCmd )
   { Action->SetRndmFlag(newValue);}

  if( command == PDGCodeCmd )
    { Action->SetPDGCode(PDGCodeCmd->GetNewIntValue(newValue));}

  if( command == EnergyCmd )
    { Action->SetEnergy(EnergyCmd->GetNewDoubleValue(newValue));}

  if( command == OriginCmd )
    { Action->SetOrigin(OriginCmd->GetNew3VectorValue(newValue));}

  if( command == OriginSmearCmd )
    { Action->SetOriginSmear(OriginSmearCmd->GetNew3VectorValue(newValue));}

  if( command == DirectionCmd )
    { Action->SetDirection(DirectionCmd->GetNew3VectorValue(newValue));}

  if( command == DirThetaCmd )
    { Action->SetDirTheta(DirThetaCmd->GetNewDoubleValue(newValue));}
  */

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

