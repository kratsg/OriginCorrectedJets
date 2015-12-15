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
#include "xAODAnaHelpers/OriginCorrection.h"
#include <xAODAnaHelpers/tools/ReturnCheck.h>

// ROOT include(s):
#include "TEnv.h"
#include "TSystem.h"


// this is needed to distribute the algorithm to the workers
ClassImp(OriginCorrection)

OriginCorrection :: OriginCorrection (std::string className) :
    Algorithm(className)
{
  Info("OriginCorrection()", "Calling constructor");

  // read debug flag from .config file
  m_debug                   = false;

  // input container to be read from TEvent or TStore
  m_inContainerName         = "";
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

    config->Print();

    delete config; config = nullptr;
  }

  // If there is no InputContainer we must stop
  if ( m_inContainerName.empty() ) {
    Error("configure()", "InputContainer is empty!");
    return EL::StatusCode::FAILURE;
  }

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



EL::StatusCode OriginCorrection :: fileExecute ()
{
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode OriginCorrection :: changeInput (bool /*firstFile*/)
{
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode OriginCorrection :: initialize ()
{
  Info("initialize()", "Initializing OriginCorrection Interface... ");

  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  if ( this->configure() == EL::StatusCode::FAILURE ) {
    Error("initialize()", "Failed to properly configure. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  /* initialize jet reclustering */
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("InputContainer", m_inContainerName), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("OutputContainer", m_inContainerName+"_Clustered"), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("Label", "LCTopo"), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("SkipNegativeEnergy", true), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("GhostScale", 0.0), "");
  RETURN_CHECK(prettyFuncName, m_pseudoJetGetterTool->initialize(), "");
  getterArray.push_back( ToolHandle<IPseudoJetGetter>(m_pseudoJetGetterTool.get()) );


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
