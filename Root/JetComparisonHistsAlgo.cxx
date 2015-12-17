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
  m_inContainerName         = "";
  // which plots will be turned on
  m_detailStr               = "";

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

EL::StatusCode JetComparisonHistsAlgo::AddHists( std::string name ) {

  std::string fullname(m_name);
  fullname += name; // add systematic
  JetComparisonHists* jetHists = new JetComparisonHists( fullname, m_detailStr ); // add systematic
  RETURN_CHECK("JetComparisonHistsAlgo::AddHists", jetHists->initialize(), "");
  jetHists->record( wk() );
  m_plots[name] = jetHists;

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetComparisonHistsAlgo :: configure ()
{
  if(!getConfig().empty()){
    // the file exists, use TEnv to read it off
    TEnv* config = new TEnv(getConfig(true).c_str());
    // input container to be read from TEvent or TStore
    m_inContainerName         = config->GetValue("InputContainer",  m_inContainerName.c_str());
    // which plots will be turned on
    m_detailStr               = config->GetValue("DetailStr",       m_detailStr.c_str());

    m_debug                   = config->GetValue("Debug" ,           m_debug);

    Info("configure()", "Loaded in configuration values");

    // everything seems preliminarily ok, let's print config and say we were successful
    config->Print();
    delete config;
  }

  // in case anything was missing or blank...
  if( m_inContainerName.empty() || m_detailStr.empty() ){
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
  AddHists("");
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
  const xAOD::JetContainer* inJets = 0;
  RETURN_CHECK("JetComparisonHistsAlgo::execute()", HelperFunctions::retrieve(inJets, m_inContainerName, m_event, m_store, m_verbose) ,("Failed to get "+m_inContainerName).c_str());
  RETURN_CHECK("JetComparisonHistsAlgo::execute()", m_plots[""]->execute( inJets, eventWeight ), "");

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetComparisonHistsAlgo :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode JetComparisonHistsAlgo :: finalize () {
  Info("finalize()", m_name.c_str());
  if(!m_plots.empty()){
    for( auto plots : m_plots ) {
      if(plots.second) delete plots.second;
    }
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetComparisonHistsAlgo :: histFinalize () {
  RETURN_CHECK("xAH::Algorithm::algFinalize()", xAH::Algorithm::algFinalize(), "");
  return EL::StatusCode::SUCCESS;
}
