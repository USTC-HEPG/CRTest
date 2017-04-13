/**
*   FILE : CRTest/src/Analysis.cc
*   Brief: Implementation of class Analysis
*/

#include "Analysis.hh"

#include "globals.hh"

Analysis* Analysis::fgInstance = NULL;

Analysis::Analysis()
	: fCurrentNtuple(-1), fMuon(NULL), fSD(NULL),
	fOpticalFirstColID(-1)
{
	rootData = G4RootAnalysisManager::Instance();
	rootData->SetFileName("CRTest");

	fMuon = new _Muon;

	fSD = new std::vector<VirtualSD*>;

	G4cout << "[+] INFO - CRTest_Analysis created." 
		<< G4endl;
}

Analysis::~Analysis(){
	delete rootData;
	delete fMuon;
}

Analysis* Analysis::Instance(){
	if(!fgInstance)
		fgInstance = new Analysis;
	return fgInstance;
}

G4bool Analysis::OpenFile(){
	return rootData->OpenFile();
}

G4bool Analysis::SaveFile(){
	rootData->Write();
	return rootData->CloseFile();
}

G4bool Analysis::CreateNtupleForRun(){
	if(fCurrentNtuple != -1)
		return false;
	fCurrentNtuple =
		rootData->CreateNtuple("Run","Muon track & SD outputs");
	assert(fCurrentNtuple == 0);

	// #for each sd in fCrySD (std::vector)
	for(G4int i = 0 ; i < fSD->size() ; i++)
		(*fSD)[i]->CreateEntry(fCurrentNtuple, rootData);
	// sd->CreateEntry(rootData, fCurrentNtuple)

	// #ifdef CRTest_DEBUG_OPTICAL
	// CreateNtupleIColumn(fCurrentNtuple, "op.[scint,wls,det]")
		// or Call OpRecorder::CreateEntry ?
	fOpticalFirstColID = 
		rootData->CreateNtupleIColumn(fCurrentNtuple, "op.scint");
	rootData->CreateNtupleIColumn(fCurrentNtuple, "op.groove");
	rootData->CreateNtupleIColumn(fCurrentNtuple, "op.fiber");
	rootData->CreateNtupleIColumn(fCurrentNtuple, "op.wls");
	rootData->CreateNtupleIColumn(fCurrentNtuple, "op.pmt");
	rootData->CreateNtupleIColumn(fCurrentNtuple, "op.det");


	return true;
}

G4bool Analysis::FillMuonTrackForRun(const G4Track* theMuon){

	if(theMuon->GetParentID() != 0)
		return false;
	
	(fMuon->Ek).push_back(theMuon->GetKineticEnergy() / GeV );
	(fMuon->time).push_back(theMuon->GetGlobalTime() / ns );

	G4ThreeVector pos = theMuon->GetPosition();
	(fMuon->x).push_back(pos.x() / cm );
	(fMuon->y).push_back(pos.y() / cm );
	(fMuon->z).push_back(pos.z() / cm );

	G4ThreeVector pmu = theMuon->GetMomentumDirection();
	(fMuon->px).push_back(pmu.x());
	(fMuon->py).push_back(pmu.y());
	(fMuon->pz).push_back(pmu.z());

	return true;
}

G4bool Analysis::FillEntryForRun(){

	// #for each sd in fCrySD (std::vector)
	// sd->FillEntry(fCurrentNtuple)
	for(G4int i = 0 ; i < fSD->size() ; i++)
		(*fSD)[i]->FillEntry(0, rootData);

	// #ifdef CRTest_DEBUG_OPTICAL
	// FillNtupleIColumn(fCurrentNtuple, op_debug->nCol[scint,wls,det])
		// or Call OpRecorder::FillEntry ?
	G4int colID = fOpticalFirstColID;
	OpRecorder* Rec = OpRecorder::Instance();
	rootData->FillNtupleIColumn(0,colID,Rec->nScintTotal);
	rootData->FillNtupleIColumn(0,colID+1,Rec->nScint2Groove);
	rootData->FillNtupleIColumn(0,colID+2,Rec->nGroove2Cladding);
	rootData->FillNtupleIColumn(0,colID+3,Rec->nWlsEmit);
	rootData->FillNtupleIColumn(0,colID+4,Rec->nCore2PMT);
	rootData->FillNtupleIColumn(0,colID+5,Rec->nDetection);

	rootData->AddNtupleRow();

	// Clear/Reset fMuon
	delete fMuon;
	fMuon = new _Muon;

	return true;
}

G4int Analysis::CreateNtupleForEvent(G4int eventID){
	
	if(fCurrentNtuple < 0) return -1;
	
	G4int ntupleID = rootData->CreateNtuple(
		"Event"+std::to_string(eventID),
		"Storege Event Vertex.");

	rootData->CreateNtupleIColumn(ntupleID, "type");  // id = 0
	rootData->CreateNtupleIColumn(ntupleID, "id"); // id = 1
	rootData->CreateNtupleDColumn(ntupleID, "E");	// eV
	rootData->CreateNtupleDColumn(ntupleID, "t");	// ns
	rootData->CreateNtupleDColumn(ntupleID, "x"); // cm
	rootData->CreateNtupleDColumn(ntupleID, "y");	// cm
	rootData->CreateNtupleDColumn(ntupleID, "z");	// cm
	rootData->CreateNtupleDColumn(ntupleID, "px");
	rootData->CreateNtupleDColumn(ntupleID, "py");
	rootData->CreateNtupleDColumn(ntupleID, "pz");

	rootData->FinishNtuple(ntupleID);

	fCurrentNtuple = ntupleID;
	return ntupleID;
}

G4bool Analysis::FillOpPhotonTrackForEvent(
	const G4Track* theTrack, OpPhotonType type)
{
	if(std::find(TypeList.begin(),TypeList.end(),type)
		== TypeList.end())
		return false;

	rootData->FillNtupleIColumn(fCurrentNtuple, 0, type);
	rootData->FillNtupleIColumn(fCurrentNtuple, 1, theTrack->GetTrackID());

	rootData->FillNtupleDColumn(fCurrentNtuple, 2, theTrack->GetKineticEnergy() / eV);
	rootData->FillNtupleDColumn(fCurrentNtuple, 3, theTrack->GetGlobalTime());

	G4ThreeVector position = theTrack->GetPosition();
	rootData->FillNtupleDColumn(fCurrentNtuple, 4, position.x()/ cm);
	rootData->FillNtupleDColumn(fCurrentNtuple, 5, position.y()/ cm);
	rootData->FillNtupleDColumn(fCurrentNtuple, 6, position.z()/ cm);

	G4ThreeVector direction = theTrack->GetMomentumDirection();
	rootData->FillNtupleDColumn(fCurrentNtuple, 7, direction.x());
	rootData->FillNtupleDColumn(fCurrentNtuple, 8, direction.y());
	rootData->FillNtupleDColumn(fCurrentNtuple, 9, direction.z());

	return rootData->AddNtupleRow(fCurrentNtuple);
}

G4bool Analysis::RegisterSD(VirtualSD* sd){

	if(std::find(fSD->begin(),fSD->end(),sd)
		== fSD->end())
	{
		fSD->push_back(sd);
		return true;
	}
	else
		return false;
}