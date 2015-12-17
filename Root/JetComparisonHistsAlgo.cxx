#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include <xAODJet/JetContainer.h>
#include <xAODTracking/VertexContainer.h>
#include <xAODEventInfo/EventInfo.h>
#include <AthContainers/ConstDataVector.h>

#include <OriginCorrectedJets/JetComparisonHistsAlgo.h>
#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODAnaHelpers/HelperClasses.h>
#include <xAODAnaHelpers/tools/ReturnCheck.h>

#include "TEnv.h"
#include "TSystem.h"

// this is needed to distribute the algorithm to the workers
ClassImp(JetComparisonHistsAlgo)

JetComparisonHistsAlgo :: JetComparisonHistsAlgo (std::string className) :
    Algorithm(className)
{
  m_inContainer1Name         = "";
  m_inContainer2Name         = "";
  // which plots will be turned on
  m_detailStr               = "";
  m_DR                      = 0.3;

  m_debug                   = false;

}

EL::StatusCode JetComparisonHistsAlgo :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("JetComparisonHistsAlgo").ignore();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetComparisonHistsAlgo :: histInitialize ()
{

  Info("histInitialize()", "%s", m_name.c_str() );
  RETURN_CHECK("xAH::Algorithm::algInitialize()", xAH::Algorithm::algInitialize(), "");
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetComparisonHistsAlgo :: configure ()
{
  if(!getConfig().empty()){
    // the file exists, use TEnv to read it off
    TEnv* config = new TEnv(getConfig(true).c_str());
    // input container to be read from TEvent or TStore
    m_inContainer1Name        = config->GetValue("InputContainer1",  m_inContainer1Name.c_str());
    m_inContainer2Name        = config->GetValue("InputContainer2",  m_inContainer2Name.c_str());
    // which plots will be turned on
    m_detailStr               = config->GetValue("DetailStr",       m_detailStr.c_str());
    m_DR                      = config->GetValue("DeltaR", m_DR);

    m_debug                   = config->GetValue("Debug" ,           m_debug);

    Info("configure()", "Loaded in configuration values");

    // everything seems preliminarily ok, let's print config and say we were successful
    config->Print();
    delete config;
  }

  // in case anything was missing or blank...
  if( m_inContainer1Name.empty() || m_inContainer2Name.empty() || m_detailStr.empty() ){
    Error("configure()", "One or more required configuration values are empty");
    return EL::StatusCode::FAILURE;
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetComparisonHistsAlgo :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode JetComparisonHistsAlgo :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode JetComparisonHistsAlgo :: initialize ()
{
  Info("initialize()", m_name.c_str());

  // needed here and not in initalize since this is called first
  Info("histInitialize()", "Attempting to configure using: %s", m_configName.c_str());
  if ( this->configure() == EL::StatusCode::FAILURE ) {
    Error("histInitialize()", "%s failed to properly configure. Exiting.", m_name.c_str() );
    return EL::StatusCode::FAILURE;
  } else {
    Info("histInitialize()", "Succesfully configured! ");
  }

  // only running 1 collection
  m_plots = new JetComparisonHists( m_name, m_detailStr ); // add systematic
  m_plots->m_DR = m_DR;
  RETURN_CHECK("JetComparisonHistsAlgo::initialize()", m_plots->initialize(), "");
  m_plots->record( wk() );
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetComparisonHistsAlgo :: execute ()
{
  const xAOD::EventInfo* eventInfo(nullptr);
  RETURN_CHECK("JetComparisonHistsAlgo::execute()", HelperFunctions::retrieve(eventInfo, m_eventInfoContainerName, m_event, m_store, m_verbose) ,"");

  float eventWeight(1);
  if( eventInfo->isAvailable< float >( "mcEventWeight" ) ) {
    eventWeight = eventInfo->auxdecor< float >( "mcEventWeight" );
  }

  // this will hold the collection processed
  const xAOD::JetContainer* jets1(nullptr);
  const xAOD::JetContainer* jets2(nullptr);
  RETURN_CHECK("JetComparisonHistsAlgo::execute()", HelperFunctions::retrieve(jets1, m_inContainer1Name, m_event, m_store, m_verbose) ,("Failed to get "+m_inContainer1Name).c_str());
  RETURN_CHECK("JetComparisonHistsAlgo::execute()", HelperFunctions::retrieve(jets2, m_inContainer2Name, m_event, m_store, m_verbose) ,("Failed to get "+m_inContainer2Name).c_str());
  RETURN_CHECK("JetComparisonHistsAlgo::execute()", m_plots->execute( jets1, jets2, eventWeight ), "");

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetComparisonHistsAlgo :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode JetComparisonHistsAlgo :: finalize () {
  Info("finalize()", m_name.c_str());
  if(m_plots) delete m_plots;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetComparisonHistsAlgo :: histFinalize () {
  RETURN_CHECK("xAH::Algorithm::algFinalize()", xAH::Algorithm::algFinalize(), "");
  return EL::StatusCode::SUCCESS;
}
