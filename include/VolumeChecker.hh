#ifndef VolumeChecker_hh
#define VolumeChecker_hh

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include <map>
#include <unordered_set>
#include <queue>
#include <string>
#include <iostream>

class VolumeChecker
{
public:
    static VolumeChecker& GetInstance(const G4String& rootVolumeName, bool verbose = false);
    static VolumeChecker& GetInstance(G4VPhysicalVolume* rootVolume, bool verbose = false);
    static VolumeChecker& GetInstance(G4LogicalVolume* rootLogicalVolume, bool verbose = false);

    static void RemoveInstance(const G4String& rootVolumeName);
    static void ClearAllInstances();

    bool IsChild(G4VPhysicalVolume* volumeToCheck) const;
    bool IsChild(const G4String& volumeName) const;

    void UpdateRootVolume(G4VPhysicalVolume* newRootVolume);
    void SetVerbose(bool verbose) { fVerbose = verbose; }
    bool GetVerbose() const { return fVerbose; }
    void PrintChildList() const;

private:
    VolumeChecker(const G4String& rootVolumeName, G4VPhysicalVolume* rootVolume, bool verbose);
    ~VolumeChecker();

    // Delete copy constructor and assignment operator
    VolumeChecker(const VolumeChecker&) = delete;
    VolumeChecker& operator=(const VolumeChecker&) = delete;

    static std::map<G4String, VolumeChecker*> fInstances;

    G4String fRootVolumeName;
    G4VPhysicalVolume* fRootPV;
    std::unordered_set<G4VPhysicalVolume*> fChildVolumes;
    std::map<G4String, G4VPhysicalVolume*> fNameToVolumeMap;
    bool fVerbose;

    void InitializeRootVolume();
    void UpdateChildVolumeList();
    bool RecursiveChildCheck(G4VPhysicalVolume* volumeToCheck) const;
};

#endif // VolumeChecker_hh
