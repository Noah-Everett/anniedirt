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
// $Id: DetectorMessenger.cc,v 1.1.1.1 2011-12-06 17:49:40 rhatcher Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(
                                           DetectorConstruction* Det)
:Detector(Det)
{ 
  novaDir = new G4UIdirectory("/g4sa/");
  novaDir->SetGuidance("UI commands of this example");
  
  detDir = new G4UIdirectory("/g4sa/det/");
  detDir->SetGuidance("detector control");
       
  SetGDMLCmd = new G4UIcmdWithAString("/g4sa/det/setgdml",this);
  SetGDMLCmd->SetGuidance("Select GDML file.");
  SetGDMLCmd->SetParameterName("filename",false);
  SetGDMLCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  SetOverlapCheckCmd = new G4UIcmdWithAString("/g4sa/det/setoverlapcheck",this);
  SetOverlapCheckCmd->SetGuidance("Enable/Disable GDML Parser OverlapCheck.");
  SetOverlapCheckCmd->SetParameterName("choice",false);
  SetOverlapCheckCmd->SetDefaultValue("on");
  SetOverlapCheckCmd->SetCandidates("on off true false 1 0");
  SetOverlapCheckCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  MagFieldCmd = new G4UIcmdWithADoubleAndUnit("/g4sa/det/setField",this);  
  MagFieldCmd->SetGuidance("Define magnetic field.");
  MagFieldCmd->SetGuidance("Magnetic field will be in Z direction.");
  MagFieldCmd->SetParameterName("Bz",false);
  MagFieldCmd->SetUnitCategory("Magnetic flux density");
  MagFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  
 
  UpdateCmd = new G4UIcmdWithoutParameter("/g4sa/det/update",this);
  UpdateCmd->SetGuidance("Update geometry.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  UpdateCmd->AvailableForStates(G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete SetGDMLCmd;
  delete SetOverlapCheckCmd;
  delete MagFieldCmd;
  delete UpdateCmd;
  delete detDir;
  delete novaDir;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if( command == SetGDMLCmd )
   { Detector->SetGDMLFile(newValue);}

  if( command == SetOverlapCheckCmd )
    { Detector->SetOverlapCheck(newValue); }
  
  if( command == MagFieldCmd )
   { Detector->SetMagField(MagFieldCmd->GetNewDoubleValue(newValue));}

  if( command == UpdateCmd )
   { Detector->UpdateGeometry(); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
