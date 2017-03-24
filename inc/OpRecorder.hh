/*
*   File : CRTest/inc/OpRecorder.hh
*   Brief: Record optical process information
*   Description:
*       Record optical photon : count, energy, direction
*       Scintillation, ScintToFiber, WLS, PMT
*/

#ifndef CRTest_OpRecorder_h
#define CRTest_OpRecorder_h

#include "globals.hh"

class OpRecorder{
public:
    OpRecorder();
    ~OpRecorder();
    static OpRecorder* Instance();
    void Reset();
public:
    G4int nScintTotal;
    G4int nScintToFiber;
    G4int nWlsEmit;
private:
    static OpRecorder* fgInstance;
};


#endif // CRTest_OpRecorder_h