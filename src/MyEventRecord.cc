#include "MyEventRecord.hh"

#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"

#include <iomanip>

MyEventRecord::MyEventRecord() 
{
  run = entry = iter = niter = -1;
  clear();

  inputNEntries = 0;
  inputTotalPOTs = 0;
  nG4Repeat = nG4BeamOn = nG4Events = 0;
}

MyEventRecord::~MyEventRecord()
{

}

void MyEventRecord::clear()
{
  // don't clear run/entry/iter/niter
  nupdg = 0;
  nuvtxx = nuvtxy = nuvtxz = nuvtxt = 0;
  intank = -1;
  inhall = -1;
  // vtxvol = vtxmat = "<<none>>";
  strncpy(vtxvol,"<<none>>",4096);
  strncpy(vtxmat,"<<none>>",4096);
  ntank  = 0;
  
}

void MyEventRecord::AppendG4PrimaryParticle(G4PrimaryVertex* g4vert,
                                            G4PrimaryParticle* g4part)
{
  if ( ! g4vert || ! g4part ) return;

  ++ntank;
  if ( ntank < mxtank ) {
    // add the info
    pdgtank[ntank-1] = g4part->GetPDGcode();
    primary[ntank-1] = 1;
    vx[ntank-1] = g4vert->GetX0() / cm;
    vy[ntank-1] = g4vert->GetY0() / cm;
    vz[ntank-1] = g4vert->GetZ0() / cm;
    vt[ntank-1] = g4vert->GetT0() / nanosecond;
    px[ntank-1] = g4part->GetPx() / GeV;
    py[ntank-1] = g4part->GetPy() / GeV;
    pz[ntank-1] = g4part->GetPz() / GeV;
    E[ntank-1]  = g4part->GetTotalEnergy() / GeV;
    kE[ntank-1] = g4part->GetKineticEnergy() / GeV;

    EraseNeutrino();

  } else {
    // too many 
    G4cerr << "MyEventRecord::AppendG4PrimaryParticle " << run << "/" << entry
           << " pushing ntank to " << ntank << " (max " << mxtank << ")"
           << G4endl;
  }
}

void MyEventRecord::AppendG4Step(G4Track* g4trk, G4StepPoint* g4steppt)
{
  if ( ! g4trk || ! g4steppt ) return;

  ++ntank;
  if ( ntank < mxtank ) {
    // add the info
    G4ThreeVector x3 = g4steppt->GetPosition();
    G4double t       = g4steppt->GetGlobalTime();
    G4ThreeVector p3 = g4steppt->GetMomentum();
    G4double e       = g4steppt->GetTotalEnergy();
    G4double ke      = g4steppt->GetKineticEnergy();

    pdgtank[ntank-1] = g4trk->GetParticleDefinition()->GetPDGEncoding();
    primary[ntank-1] = 0;
    vx[ntank-1] = x3.x() / cm;
    vy[ntank-1] = x3.y() / cm;
    vz[ntank-1] = x3.z() / cm;
    vt[ntank-1] = t / nanosecond;
    px[ntank-1] = p3.x() / GeV;
    py[ntank-1] = p3.y() / GeV;
    pz[ntank-1] = p3.z() / GeV;
    E[ntank-1]  = e / GeV;
    kE[ntank-1] = ke / GeV;

    EraseNeutrino();

  } else {
    // too many 
    G4cerr << "MyEventRecord::AppendG4PrimaryParticle " << run << "/" << entry
           << " pushing ntank to " << ntank << " (max " << mxtank << ")"
           << G4endl;
  }

}

void MyEventRecord::EraseNeutrino()
{
  // clear the last entry if it is a neutrino, back up the count
  int lastpdg_abs = abs(pdgtank[ntank-1]);

  if ( lastpdg_abs == 12 ||
       lastpdg_abs == 14 ||
       lastpdg_abs == 16    ) --ntank;
}

std::ostream& operator<<(std::ostream& os, const MyEventRecord& mer)
{
  os << "MyEventRecord r" << mer.run << " e" << mer.entry
     << " (" << mer.iter << "/" << mer.niter << ")" << std::endl;
  os << "   nupdg " << mer.nupdg << " vtx(" << mer.nuvtxx << ","
     << mer.nuvtxy << "," << mer.nuvtxz << ";" << mer.nuvtxt << ") "
     << std::endl;
  os << "   ntank " << mer.ntank 
     << " vtx intank " << mer.intank
     << " inhall " << mer.inhall
     << " vol " << mer.vtxvol << " mat " << mer.vtxmat
     << std::endl;
  for (int itank=0; itank<mer.ntank; ++itank) {
    os << "    [" << std::setw(4) << itank << "] "
       << std::setw(8) << mer.pdgtank[itank]
       << " prim" << mer.primary[itank]
       << " v(" << mer.vx[itank] << "," << mer.vy[itank] << "," << mer.vz[itank]
       << ";" << mer.vt[itank] << ")"
       << " p(" << mer.px[itank] << "," << mer.py[itank] << "," << mer.pz[itank]
       << ";" << mer.E[itank] << ") ke=" << mer.kE[itank]
       << std::endl;
  }
  return os;
}
