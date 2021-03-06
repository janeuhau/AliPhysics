/**
 * @file 
 * 
 * @ingroup pwg2_forward_scripts
 */

/** 
 * Run first pass of the analysis - that is read in ESD and produce AOD
 * 
 * @param esddir    ESD input directory. Any file matching the pattern 
 *                  *AliESDs*.root are added to the chain 
 * @param nEvents   Number of events to process.  If 0 or less, then 
 *                  all events are analysed
 * @param proof     Run in proof mode 
 * @param mc        Run over MC data
 *
 * If PROOF mode is selected, then Terminate will be run on the master node 
 * in any case. 
 * 
 *
 * @ingroup pwg2_forward_scripts
 */
void CreateResponseMatrices(const char* aoddir=".", 
	        Int_t       nEvents=-1, 
		const char* trig="INEL",
		Double_t    vzMin=-10,
		Double_t    vzMax=10,
		char* output="responseMatrices.root",
	        Int_t       proof=0)
{
  // --- Libraries to load -------------------------------------------
  gROOT->Macro("$ALICE_ROOT/PWGLF/FORWARD/analysis2/scripts/LoadLibs.C");

  // --- Check for proof mode, and possibly upload pars --------------
  if (proof> 0) { 
    gROOT->LoadMacro("$ALICE_ROOT/PWGLF/FORWARD/analysis2/scripts/LoadPars.C");
    LoadPars(proof);
  }
  
  // --- Our data chain ----------------------------------------------
  gROOT->LoadMacro("$ALICE_ROOT/PWGLF/FORWARD/analysis2/scripts/MakeChain.C");
  TChain* chain = MakeChain("AOD", aoddir,true);
  // If 0 or less events is select, choose all 
  if (nEvents <= 0) nEvents = chain->GetEntries();

  // --- Creating the manager and handlers ---------------------------
  AliAnalysisManager *mgr  = new AliAnalysisManager("Forward Train", 
						    "Forward Multiplicity");
  AliAnalysisManager::SetCommonFileName(output);

  // --- ESD input handler -------------------------------------------
  AliAODInputHandler *aodInputHandler = new AliAODInputHandler();
  mgr->SetInputEventHandler(aodInputHandler);      
       
  // --- Add tasks ---------------------------------------------------
  // Forward 
  gROOT->LoadMacro("$ALICE_ROOT/PWGLF/FORWARD/analysis2/AddTaskCreateResponseMatrices.C");
  AddTaskCreateResponseMatrices(trig, vzMin, vzMax);
  
  
  // --- Run the analysis --------------------------------------------
  TStopwatch t;
  if (!mgr->InitAnalysis()) {
    Error("MakeMult", "Failed to initialize analysis train!");
    return;
  }
  // Skip terminate if we're so requested and not in Proof or full mode
  mgr->SetSkipTerminate(false);
  // Some informative output 
  mgr->PrintStatus();
  // mgr->SetDebugLevel(3);
  if (mgr->GetDebugLevel() < 1 && !proof) 
    mgr->SetUseProgressBar(kTRUE,100);

  // Run the train 
  t.Start();
  Printf("=== RUNNING ANALYSIS ==================================");
  mgr->StartAnalysis(proof ? "proof" : "local", chain, nEvents);
  t.Stop();
  t.Print();
}
//
// EOF
//
