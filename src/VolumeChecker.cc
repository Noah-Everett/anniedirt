#include "VolumeChecker.hh"

std::map<G4String, VolumeChecker*> VolumeChecker::fInstances;

// Static methods to get or create instances
VolumeChecker& VolumeChecker::GetInstance(const G4String& rootVolumeName, bool verbose)
{
    auto it = fInstances.find(rootVolumeName);
    if (it == fInstances.end()) {
        G4LogicalVolumeStore* logVolStore = G4LogicalVolumeStore::GetInstance();
        G4LogicalVolume* rootLV = nullptr;
        for (auto logVol : *logVolStore) {
            if (logVol->GetName() == rootVolumeName) {
                rootLV = logVol;
                break;
            }
        }
        if (rootLV == nullptr) {
            std::cerr << "Logical volume '" << rootVolumeName << "' not found!" << std::endl;
            throw std::runtime_error("Root volume not found");
        }
        G4VPhysicalVolume* rootPV = rootLV->GetDaughter(0);
        fInstances[rootVolumeName] = new VolumeChecker(rootVolumeName, rootPV, verbose);
    } else {
        it->second->SetVerbose(verbose);
    }
    return *fInstances[rootVolumeName];
}

VolumeChecker& VolumeChecker::GetInstance(G4VPhysicalVolume* rootVolume, bool verbose)
{
    return GetInstance(rootVolume->GetName(), verbose);
}

VolumeChecker& VolumeChecker::GetInstance(G4LogicalVolume* rootLogicalVolume, bool verbose)
{
    if (rootLogicalVolume->GetNoDaughters() == 0) {
        std::cerr << "Provided root logical volume has no daughters!" << std::endl;
        throw std::runtime_error("Invalid root volume");
    }
    return GetInstance(rootLogicalVolume->GetDaughter(0), verbose);
}

// Remove a specific instance
void VolumeChecker::RemoveInstance(const G4String& rootVolumeName)
{
    auto it = fInstances.find(rootVolumeName);
    if (it != fInstances.end()) {
        delete it->second;
        fInstances.erase(it);
    }
}

// Clear all instances
void VolumeChecker::ClearAllInstances()
{
    for (auto& pair : fInstances) {
        delete pair.second;
    }
    fInstances.clear();
}

// Constructor
VolumeChecker::VolumeChecker(const G4String& rootVolumeName, G4VPhysicalVolume* rootVolume, bool verbose)
    : fRootVolumeName(rootVolumeName), fRootPV(rootVolume), fVerbose(verbose)
{
    InitializeRootVolume();
}

// Destructor
VolumeChecker::~VolumeChecker()
{
    // Clean up any dynamically allocated resources
}

// Initialize root volume
void VolumeChecker::InitializeRootVolume()
{
    if (fRootPV == nullptr) {
        std::cerr << "Root volume is nullptr!" << std::endl;
        return;
    }
    UpdateChildVolumeList();
    if (fVerbose) {
        std::cout << "Initialized VolumeChecker for root volume: " << fRootVolumeName << std::endl;
        PrintChildList();
    }
}

// Update root volume
void VolumeChecker::UpdateRootVolume(G4VPhysicalVolume* newRootVolume)
{
    fRootPV = newRootVolume;
    InitializeRootVolume();
}

// Check if a volume is a child of the root volume (by pointer)
bool VolumeChecker::IsChild(G4VPhysicalVolume* volumeToCheck) const
{
    if (fVerbose) {
        std::cout << "Running IsChild() on " << volumeToCheck->GetName() << std::endl;
        PrintChildList();
    }
    bool isChild = RecursiveChildCheck(volumeToCheck);
    if (fVerbose) {
        std::cout << "Checking if volume " << volumeToCheck->GetName()
                  << " is a child of " << fRootVolumeName
                  << ": " << (isChild ? "Yes" : "No") << std::endl;
    }
    return isChild;
}

// Check if a volume is a child of the root volume (by name)
bool VolumeChecker::IsChild(const G4String& volumeName) const
{
    if (fVerbose) {
        std::cout << "Running IsChild() on " << volumeName << std::endl;
        PrintChildList();
    }
    auto volumeIt = fNameToVolumeMap.find(volumeName);
    if (volumeIt == fNameToVolumeMap.end()) {
        if (fVerbose) {
            std::cerr << "Volume '" << volumeName << "' not found in the volume hierarchy of '" << fRootVolumeName << "'!" << std::endl;
        }
        return false;
    }
    bool isChild = RecursiveChildCheck(volumeIt->second);
    if (fVerbose) {
        std::cout << "Checking if volume " << volumeName
                  << " is a child of " << fRootVolumeName
                  << ": " << (isChild ? "Yes" : "No") << std::endl;
    }
    return isChild;
}

// Update the list of child volumes
void VolumeChecker::UpdateChildVolumeList()
{
    fChildVolumes.clear();
    fNameToVolumeMap.clear();
    if (!fRootPV) return;

    std::queue<G4LogicalVolume*> queue;
    queue.push(fRootPV->GetLogicalVolume());

    while (!queue.empty()) {
        G4LogicalVolume* currentLV = queue.front();
        queue.pop();

        for (int i = 0; i < currentLV->GetNoDaughters(); ++i) {
            G4VPhysicalVolume* daughter = currentLV->GetDaughter(i);
            fChildVolumes.insert(daughter);
            fNameToVolumeMap[daughter->GetName()] = daughter;
            queue.push(daughter->GetLogicalVolume());
        }
    }

    if (fVerbose) {
        std::cout << "Updated child volume list for " << fRootVolumeName << std::endl;
        PrintChildList();
    }
}

// Recursive check for child volumes
bool VolumeChecker::RecursiveChildCheck(G4VPhysicalVolume* volumeToCheck) const
{
    if (fChildVolumes.find(volumeToCheck) != fChildVolumes.end()) {
        return true;
    }

    G4LogicalVolume* parentLV = volumeToCheck->GetMotherLogical();
    if (parentLV == nullptr) {
        return false;
    }

    G4VPhysicalVolume* parentPV = parentLV->GetDaughter(0); // Assuming the first daughter is the parent
    return RecursiveChildCheck(parentPV);
}

// Print the list of child volumes
void VolumeChecker::PrintChildList() const
{
    std::cout << "Child volumes of " << fRootVolumeName << ":" << std::endl;
    for (const auto& pair : fNameToVolumeMap) {
        std::cout << "  - " << pair.first << std::endl;
    }
}
