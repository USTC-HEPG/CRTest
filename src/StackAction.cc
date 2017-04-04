/**
*   FILE : CRTest/src/StackAction.cc
*   Brief: Implementation of class StackAction
*/

#include "StackAction.hh"

#include "Analysis.hh"
#include "OpRecorder.hh"

#include "G4Track.hh"
#include "G4VProcess.hh"

#include "G4OpticalPhoton.hh"

StackAction::StackAction()
    : G4UserStackingAction()
{
}

StackAction::~StackAction()
{
	G4cout << "[-] INFO - StackAction deleted. " << G4endl;
}

G4ClassificationOfNewTrack
StackAction::ClassifyNewTrack(const G4Track *aTrack)
{
    OpRecorder *Recorder = OpRecorder::Instance();

	//Count what process generated the optical photons
	if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
	{
		// particle is secondary
		if (aTrack->GetCreatorProcess()->GetProcessName() 
			== "Scintillation"){
			ana::FillVertexForEvent(aTrack, ana::VertexType::Scintillation);
			Recorder->nScintTotal++;
		}
		else if (aTrack->GetCreatorProcess()->GetProcessName() 
			== "OpWLS"){
			ana::FillVertexForEvent(aTrack, ana::VertexType::OpWLS);
			Recorder->nWlsEmit++;
		}

	}

    return fUrgent;
}

void StackAction::NewStage() {}

void StackAction::PrepareNewEvent() {}