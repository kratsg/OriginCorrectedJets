// EL include(s):
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include "JetInterface/IJetModifier.h"

// make unique pointers
#include <CxxUtils/make_unique.h>

// package include(s):
#include "xAODAnaHelpers/HelperFunctions.h"
#include "OriginCorrectedJets/OriginCorrection.h"
#include <xAODAnaHelpers/tools/ReturnCheck.h>

// ROOT include(s):
#include "TEnv.h"
#include "TSystem.h"


// this is needed to distribute the algorithm to the workers
ClassImp(OriginCorrection)

OriginCorrection :: OriginCorrection (std::string className) :
    Algorithm(className),
    m_pseudoJetGetterTool         (CxxUtils::make_unique<PseudoJetGetter>("PseudoJetGetterTool_"+className)),
    m_jetFromPseudoJetTool        (CxxUtils::make_unique<JetFromPseudojet>("JetFromPseudoJetTool_"+className)),
    m_jetFinderTool               (CxxUtils::make_unique<JetFinder>("JetFinderTool_"+className)),
    m_originCorrectionTool        (CxxUtils::make_unique<JetRecTool>("JetRec_OriginCorrectionTool_"+className)),
    m_jetChargeTool               (CxxUtils::make_unique<JetChargeTool>("JetChargeTool_"+className)),
    m_jetPullTool                 (CxxUtils::make_unique<JetPullTool>("JetPullTool_"+className)),
    m_energyCorrelatorTool        (CxxUtils::make_unique<EnergyCorrelatorTool>("EnergyCorrelatorTool_"+className)),
    m_energyCorrelatorRatiosTool  (CxxUtils::make_unique<EnergyCorrelatorRatiosTool>("EnergyCorrelatorRatiosTool_"+className)),
    m_ktSplittingScaleTool        (CxxUtils::make_unique<KTSplittingScaleTool>("KTSplittingScaleTool_"+className)),
    m_dipolarityTool              (CxxUtils::make_unique<DipolarityTool>("DipolarityTool_"+className)),
    m_centerOfMassShapesTool      (CxxUtils::make_unique<CenterOfMassShapesTool>("CenterOfMassShapesTool_"+className)),
    m_jetWidthTool                (CxxUtils::make_unique<JetWidthTool>("JetWidthTool_"+className))

{
  Info("OriginCorrection()", "Calling constructor");

  // read debug flag from .config file
  m_debug                   = false;

  m_inContainerName         = "";
  m_outContainerName         = "";
  m_jet_alg = "AntiKt";
  m_pt_min = 0.0;
  m_radius = 1.0;
}

EL::StatusCode  OriginCorrection :: configure ()
{
  if ( !getConfig().empty() ) {

    Info("configure()", "Configuring OriginCorrection Interface. User configuration read from : %s ", getConfig().c_str());

    TEnv* config = new TEnv(getConfig(true).c_str());

    // read debug flag from .config file
    m_debug                   = config->GetValue("Debug" , m_debug);
    // input container to be read from TEvent or TStore
    m_inContainerName         = config->GetValue("InputContainer",  m_inContainerName.c_str());
    m_outContainerName        = config->GetValue("OutputContainer",  m_outContainerName.c_str());

    m_jet_alg                 = config->GetValue("JetAlgorithm", m_jet_alg.c_str());
    m_pt_min                  = config->GetValue("PtMin", m_pt_min);
    m_radius                  = config->GetValue("JetRadius", m_radius);

    config->Print();

    delete config; config = nullptr;
  }

  // If there is no InputContainer we must stop
  if ( m_inContainerName.empty() ) {
    Error("configure()", "InputContainer is empty!");
    return EL::StatusCode::FAILURE;
  }

  if(m_outContainerName.empty()) m_outContainerName = "OriginCorrected"+m_inContainerName;

  if ( !getConfig().empty() )
    Info("configure()", "OriginCorrection Interface succesfully configured! ");

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode OriginCorrection :: setupJob (EL::Job& job)
{
  Info("setupJob()", "Calling setupJob");

  job.useXAOD ();
  xAOD::Init( "OriginCorrection" ).ignore(); // call before opening first file

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode OriginCorrection :: histInitialize ()
{
  RETURN_CHECK("xAH::Algorithm::algInitialize()", xAH::Algorithm::algInitialize(), "");
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode OriginCorrection :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode OriginCorrection :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }


EL::StatusCode OriginCorrection :: initialize ()
{
  Info("initialize()", "Initializing OriginCorrection Interface... ");

  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  if ( this->configure() == EL::StatusCode::FAILURE ) {
    Error("initialize()", "Failed to properly configure. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  auto prettyFuncName = "OriginCorrection::initialize()";

  ToolHandleArray<IJetModifier> modArray;
  ToolHandleArray<IPseudoJetGetter> getterArray;

  /* initialize jet reclustering */
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("InputContainer", m_inContainerName), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("OutputContainer", m_inContainerName+"_PseudoJets"), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("Label", "LCTopo"), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("SkipNegativeEnergy", true), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("GhostScale", 0.0), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->initialize(), "");
  getterArray.push_back( ToolHandle<IPseudoJetGetter>(m_pseudoJetGetterTool.get()) );
  //    - create a Jet builder
  RETURN_CHECK(prettyFuncName, m_jetFromPseudoJetTool->initialize(), "");
  //    - create a ClusterSequence Tool
  RETURN_CHECK(prettyFuncName, m_jetFinderTool->setProperty("JetAlgorithm", m_jet_alg), "");
  RETURN_CHECK(prettyFuncName, m_jetFinderTool->setProperty("JetRadius", m_radius), "");
  RETURN_CHECK(prettyFuncName, m_jetFinderTool->setProperty("PtMin", m_pt_min*1.e3), "");
  RETURN_CHECK(prettyFuncName, m_jetFinderTool->setProperty("GhostArea", 0.0), "");
  RETURN_CHECK(prettyFuncName, m_jetFinderTool->setProperty("RandomOption", 1), "");
  RETURN_CHECK(prettyFuncName, m_jetFinderTool->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(m_jetFromPseudoJetTool.get())), "");
  RETURN_CHECK(prettyFuncName, m_jetFinderTool->initialize(), "");
  //    - create list of modifiers.
  modArray.clear();
  //        and then apply all other modifiers based on the trimmed reclustered jets
  modArray.push_back( ToolHandle<IJetModifier>( m_jetChargeTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_jetPullTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_energyCorrelatorTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_energyCorrelatorRatiosTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_ktSplittingScaleTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_dipolarityTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_centerOfMassShapesTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_jetWidthTool.get() ) );
  //    - create our master reclustering tool
  RETURN_CHECK(prettyFuncName, m_originCorrectionTool->setProperty("OutputContainer", m_outContainerName), "");
  RETURN_CHECK(prettyFuncName, m_originCorrectionTool->setProperty("PseudoJetGetters", getterArray), "");
  RETURN_CHECK(prettyFuncName, m_originCorrectionTool->setProperty("JetFinder", ToolHandle<IJetFinder>(m_jetFinderTool.get())), "");
  RETURN_CHECK(prettyFuncName, m_originCorrectionTool->setProperty("JetModifiers", modArray), "");
  RETURN_CHECK(prettyFuncName, m_originCorrectionTool->initialize(), "");


  return EL::StatusCode::SUCCESS;
}


EL::StatusCode OriginCorrection :: execute ()
{
  if ( m_debug ) { Info("execute()", "Applying Jet Origin Correction... "); }

  // get the collection from TEvent or TStore
  const xAOD::JetContainer* inJets(nullptr);
  RETURN_CHECK("OriginCorrection::execute()", HelperFunctions::retrieve(inJets, m_inContainerName, m_event, m_store, m_verbose) ,"");

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode OriginCorrection :: postExecute ()
{
  if ( m_debug ) { Info("postExecute()", "Calling postExecute"); }
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode OriginCorrection :: finalize ()
{
  Info("finalize()", "Deleting tool instances...");
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode OriginCorrection :: histFinalize ()
{
  Info("histFinalize()", "Calling histFinalize");
  RETURN_CHECK("xAH::Algorithm::algFinalize()", xAH::Algorithm::algFinalize(), "");
  return EL::StatusCode::SUCCESS;
}
