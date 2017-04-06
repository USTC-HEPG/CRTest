/**
*   FILE : CRTest/src/StepAction.cc
*   Brief: Implementation of class StepAction
*/

#include "StepAction.hh"

#include "Analysis.hh"
#include "OpRecorder.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4SteppingManager.hh"
#include "G4VPhysicalVolume.hh"

#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4SystemOfUnits.hh"

StepAction::StepAction()
{
}

StepAction::~StepAction()
{
    G4cout << "[-] INFO - StepAction deleted. " << G4endl;
}

void StepAction::UserSteppingAction(const G4Step *aStep)
{

    OpRecorder *Recorder = OpRecorder::Instance();

    G4Track *theTrack = aStep->GetTrack();
    G4StepPoint *thePrePoint = aStep->GetPreStepPoint();
    G4StepPoint *thePostPoint = aStep->GetPostStepPoint();
    G4VPhysicalVolume *thePrePV = thePrePoint->GetPhysicalVolume();
    G4VPhysicalVolume *thePostPV = thePostPoint->GetPhysicalVolume();

    const G4VProcess *theProcess = fpSteppingManager->GetfCurrentProcess();

    //  for Optical
    if (theTrack->GetParticleDefinition() !=
        G4OpticalPhoton::OpticalPhotonDefinition())
        return;

    //
    // Boundary Check
    //
	ana::VertexType type = ana::Final;
    if (thePostPoint->GetStepStatus() == fGeomBoundary)
	{
        assert(theProcess->GetProcessName() == "OpBoundary");
        G4OpBoundaryProcess *boundary = (G4OpBoundaryProcess *)theProcess;
		G4OpBoundaryProcessStatus status = boundary->GetStatus();
		G4bool gotThrough = 
			(status == Transmission || status == FresnelRefraction);
		if(gotThrough){
			// OpPthoton got through boundary
			if (thePrePV->GetName() == "Detector_PV" &&
				thePostPV->GetName() == "Groove_PV")
			{
				type = ana::Scint2Groove;
				Recorder->nScint2Groove ++;

			}
			else if (thePrePV->GetName() == "Groove_PV" &&
				thePostPV->GetName() == "Cladding_PV")
			{
				type = ana::Groove2Cladding;
				Recorder->nGroove2Cladding += 1;
			}
			else if (thePrePV->GetName() == "Cladding_PV" &&
				thePostPV->GetName() == "Core_PV")
			{
				type = ana::Cladding2Core;
				Recorder->nCladding2Core += 1;
			}
			// TODO : REMOVE after GDML setup completed
			else if (thePrePV->GetName() == "Groove_PV" &&
				thePostPV->GetName() == "Core_PV")
			{
				type = ana::Groove2Cladding;
				Recorder->nGroove2Cladding += 1;
			}
		}
        else if (thePrePV->GetName() == "Core_PV" &&
                 thePostPV->GetName() == "PMT_PV")
        {
			// OpPhoton hit PMT photocathode
            type = ana::Fiber2Pmt;
            Recorder->nCore2PMT += 1;
            if (status == Detection)
				Recorder->nDetection += 1;
        }
		// For Debug boundary details
        else if (thePrePV->GetName() == "Core_PV" &&
                 thePostPV->GetName() == "Groove_PV")
        {
            Recorder->nDebug += 1;
            Recorder->SetBoundaryName("Core2Groove");
            BoundaryStats(boundary);
            //theTrack->SetTrackStatus(G4TrackStatus::fStopAndKill);
            return;
        }
    }
	if(type != ana::Final)
		ana::FillVertexForEvent(theTrack, type);
}

G4bool StepAction::BoundaryStats(G4OpBoundaryProcess *boundary)
{
    OpRecorder *Recorder = OpRecorder::Instance();
    switch (boundary->GetStatus())
    {
    case FresnelRefraction:;
    case Transmission:
        Recorder->nBoundaryTransmission++;
        break;
    case Absorption:;
    case Detection:
        Recorder->nBoundaryAbsorption++;
        break;
    case FresnelReflection:;
    case TotalInternalReflection:;
    case LambertianReflection:;
    case LobeReflection:;
    case SpikeReflection:;
    case BackScattering:
        Recorder->nBoundaryReflection++;
        break;
    case Undefined:
        Recorder->nBoundaryUndefined++;
        break;
    case NotAtBoundary:;
    case SameMaterial:;
    case StepTooSmall:;
    case NoRINDEX:;
        Recorder->nBoundaryWARNNING++;
        break;
    default:
        return false;
    }
    return true;
}