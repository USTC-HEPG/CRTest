/*
*   File : CRTest/inc/SysMessenger.hh
*
*   Brief: Definition of User Interface in G4 session
*
*   Interact with RunManager
*   SysConstruction
*       Geometry, Material, Limit, ...
*   CryDetector
*       Component, ...
*   Generator
*       Mode, PrimaryVertex, ...
*   PhysicsList
*       Options
*   Run & Event Action
*       Data Format
*   
*/
#ifndef CRTest_SysMessenger_h
#define CRTest_SysMessenger_h

#include "G4UImessenger.hh"

#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "G4String.hh"

class SysMessenger : public G4UImessenger
{
public:
    SysMessenger(G4RunManager*);
    virtual ~SysMessenger();

public:
    virtual G4String GetCurrentValue(G4UIcommand *);
    virtual void SetNewValue(G4UIcommand *,G4String);

private:
    G4RunManager* fRunManager;
    
    G4UIdirectory* fCRTestDir;
    G4UIdirectory* fGeomDir;

    // TODO : use cmd collection?
    G4UIcmdWithADoubleAndUnit* fSpacingCmd;
};

#endif // CRTest_SysMessenger_h