
#include "PrimaryGeneratorAction.hh"

#include "G4VUserDetectorConstruction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4TouchableHistory.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

// GENIE headers
#include "GHEP/GHepParticle.h"
#include "Ntuple/NtpMCTreeHeader.h"

// ROOT headers


// this program's headers
#include "MyEventRecord.hh"

#include <cstring>

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(MyEventRecord* myevtrec,
                                               G4String infname, G4int verbose)
  : fMyEventRecord(myevtrec)
  , fInputFileName(infname)
  , fInputFile(0)
  , fInputTree(0)
  , fEntry(-1)
  , fGenieRecord(0)
  , fVerbosity(verbose)
{
  
  //create a messenger for this class
  genMessenger = new PrimaryGeneratorMessenger(this);

  fInputFile = TFile::Open(fInputFileName.c_str(),"READONLY");
  if ( ! fInputFile ) {
    G4cerr << "%%% could not find input file \"" << fInputFileName << "\"" 
           << G4endl;
    exit(127);
  }

  genie::NtpMCTreeHeader* header = 0;
  fInputFile->GetObject("header",header);
  if ( ! header ) {
    G4cerr << "%%% could not find NtpMCTreeHeader in \"" << fInputFileName << "\"" 
           << G4endl;
    exit(127);
  }
  G4cout << "from GENIE header: " << G4endl
         << "  runnu " << header->runnu << G4endl
         << "  cvstag " << header->cvstag.GetName() << G4endl
         << "  format " << genie::NtpMCFormat::AsString(header->format) << G4endl
         << "  datime " << header->datime << G4endl
         << G4endl;

//  NtpMCFormat_t format;  ///< Event Record format (GENIE support multiple formats)
//  TObjString    cvstag;  ///< GENIE CVS Tag (to keep track of GENIE's version)
//  NtpMCDTime    datime;  ///< Date and Time that the event ntuple was generated
//  Long_t        runnu;   ///< MC Job run number

 fInputFile->GetObject("gtree",fInputTree);
  if ( ! fInputTree ) {
    G4cerr << "%%% could not find input tree \"gtree\" in \"" 
           << fInputFileName << "\"" << G4endl;
    exit(127);
  }
  fNEntries = fInputTree->GetEntries();

  fMyEventRecord->run            = header->runnu;
  fMyEventRecord->inputFileName  = fInputFileName;
  fMyEventRecord->inputNEntries  = fNEntries;
  fMyEventRecord->inputTotalPOTs = fInputTree->GetWeight();
  
  G4cout << "%%% input file \"" << fInputFileName << "\" has " 
         << fNEntries << " entries, total POTS "
         << fMyEventRecord->inputTotalPOTs << G4endl;

  fGenieRecord = new genie::NtpMCEventRecord;
  fInputTree->SetBranchAddress("gmcrec",&fGenieRecord); // gmcrec is branch name

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  if ( fInputFile ) fInputFile->Close();
  delete genMessenger;
  delete fGenieRecord;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of event
  // 
  ++fEntry;
  if ( fVerbosity > 0 ) {
    G4cout << G4endl << "---> GeneratePrimaries from GENIE file entry "
           << fEntry << G4endl;
  }

  if ( fEntry >= fNEntries ) {
    // no more input
    return;
  }
  fInputTree->GetEntry(fEntry);
  genie::EventRecord* gevtRec = fGenieRecord->event;

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  TLorentzVector* genieVtx = gevtRec->Vertex();
  G4double x = genieVtx->X() * m;  // GENIE uses meters
  G4double y = genieVtx->Y() * m;  // GENIE uses meters
  G4double z = genieVtx->Z() * m;  // GENIE uses meters
  G4double t = genieVtx->T() * second;  // GENIE uses seconds for time
  G4ThreeVector g4pos(x,y,z);
  G4PrimaryVertex* evtVertex = new G4PrimaryVertex(g4pos,t);

  int intank = 0;
  int inhall = -1;  // not determining this right yet

  G4Navigator* navigator = 
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  G4VPhysicalVolume* pvol = 0;

  pvol = navigator->LocateGlobalPointAndSetup(g4pos,0,false,true);

  std::string vtxvol = ( ( pvol ) ? pvol->GetName() : "<<no-pvol>>" );
  // really should walk up ... for hierarchy recording copy #

  std::string vtxmat = 
    ( ( pvol ) ? pvol->GetLogicalVolume()->GetMaterial()->GetName() : "<<unknown>>" );

  /*
  G4TouchableHistory* touchable = navigator->CreateTouchableHistory();
  //navigator->LocateGlobalPointAndUpdateTouchable(g4pos,touchable,false);
  // touchable not owned by navigator ... need to delete

  pvol = navigator->ResetHierarchyAndLocate(g4pos,G4ThreeVector(),*touchable);
  
  //touchable->GetVolume();  

  G4int ndepth = touchable->GetHistoryDepth();
  G4cout << "ndepth " << ndepth << G4endl;

  for (G4int idepth=0; idepth<ndepth; ++idepth) {
    pvol = touchable->GetVolume(idepth);
    if ( pvol ) {
      G4cout << "[" << idepth << "] " << pvol->GetName() << G4endl;
    } else {
      G4cout <<" [" << idepth << "]  no pvol " << G4endl;
    }
  }

  delete touchable;

  while ( pvol ) {
    G4String pvname = pvol->GetName();
    if ( lvname.find("TWATER")   == 0 ) intank = 1;
    if ( lvname.find("EXP_HALL") == 0 ) inhall = 1;
    //lv = lv->
  }
  */

  if ( vtxvol == "TWATER_PV" ) {
    intank = 1;
    G4cout << "=========> particles start in TWATER_PV entry " 
           << fEntry << G4endl;
  }
  // other posssibilities ? ... tank wall, base, cone, airspace?


  fMyEventRecord->clear();

  fMyEventRecord->entry  = fEntry;
  fMyEventRecord->nupdg  = gevtRec->Probe()->Pdg();
  fMyEventRecord->nuvtxx = genieVtx->X();
  fMyEventRecord->nuvtxy = genieVtx->Y();
  fMyEventRecord->nuvtxz = genieVtx->Z();
  fMyEventRecord->nuvtxt = genieVtx->T();

  fMyEventRecord->intank = intank;
  fMyEventRecord->inhall = inhall;
  //fMyEventRecord->vtxvol = vtxvol;
  //fMyEventRecord->vtxmat = vtxmat;
  //char * strncpy ( char * destination, const char * source, size_t num );
  fMyEventRecord->vtxvol[4095] = '\0';
  fMyEventRecord->vtxmat[4095] = '\0';
  strncpy(fMyEventRecord->vtxvol,vtxvol.c_str(),4094);
  strncpy(fMyEventRecord->vtxmat,vtxmat.c_str(),4094);

  fMyEventRecord->ntank  = 0;

  // in principle particles leaving the nucleus are offset from the
  // actual vertex and should start from
  //    evtVertex (meters) + part->V()*1.0e-15 (meters)
  // for now ... just originate everything at center...
  // and don't worry about femto-meter scale offsets
  // (or slightly bigger for charm / tau decays)

  if ( fVerbosity > 1 ) {
    G4cout << "genieVtx T " << genieVtx->T() << " " << std::flush;
    genieVtx->Print();
    G4cout << " within " <<  vtxvol << G4endl;
  }

  // loop over GENIE particles
  int nparticles = gevtRec->GetEntries();
  for (int ipart=0; ipart<nparticles; ++ipart ) {

    genie::GHepParticle* part = gevtRec->Particle(ipart);
    if ( part->Status() != 1 ) continue; // not to be tracked

    G4ParticleDefinition* partDef = particleTable->FindParticle(part->Pdg());
    if ( ! partDef ) {
      if ( fVerbosity > 0 ) {
        G4cout << "skipping PDG " << part->Pdg() << G4endl;
      }
      continue;
    }
    G4ThreeVector voffset(part->Vx(),part->Vy(),part->Vz());
    G4double toffset = part->Vt();

    G4PrimaryParticle* g4part = 
      new G4PrimaryParticle( partDef, 
                             part->Px()*GeV, part->Py()*GeV, part->Pz()*GeV,
                             part->E()*GeV);

    if ( intank ) {
      fMyEventRecord->AppendG4PrimaryParticle(evtVertex,g4part);
      delete g4part;
    } else {
      evtVertex->SetPrimary( g4part ); // really add to G4 to process
    }

    if ( fVerbosity > 1 ) {
      G4cout 
        << "pdg " << part->Pdg()
        << " E " << part->E()
        << " Pz() " << part->Pz()
        << " (" << part->Pz()*GeV << ") "
        << "voffset " << voffset << " fm, t " << toffset << G4endl;
    }
  }
  
  anEvent->AddPrimaryVertex(evtVertex);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

