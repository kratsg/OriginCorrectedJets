#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include <xAODJet/JetContainer.h>
#include <xAODTracking/VertexContainer.h>
#include <xAODEventInfo/EventInfo.h>
#include <AthContainers/ConstDataVector.h>
#include "xAODCaloEvent/CaloClusterContainer.h"

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
  m_dR                      = 0.3;
  m_compareClusters         = false;

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

EL::StatusCode JetComparisonHistsAlgo :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode JetComparisonHistsAlgo :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode JetComparisonHistsAlgo :: initialize ()
{
  Info("initialize()", m_name.c_str());

  // in case anything was missing or blank...
  if( m_inContainer1Name.empty() || (m_inContainer2Name.empty() && !m_compareClusters) || m_detailStr.empty() ){
    Error("configure()", "One or more required configuration values are empty");
    return EL::StatusCode::FAILURE;
  }

  Info("histInitialize()", "Succesfully configured! ");

  // only running 1 collection
  m_plots = new JetComparisonHists( m_name, m_detailStr ); // add systematic
  m_plots->m_dR = m_dR;
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
  if(!m_compareClusters){
    RETURN_CHECK("JetComparisonHistsAlgo::execute()", HelperFunctions::retrieve(jets2, m_inContainer2Name, m_event, m_store, m_verbose) ,("Failed to get "+m_inContainer2Name).c_str());
    RETURN_CHECK("JetComparisonHistsAlgo::execute()", m_plots->execute( jets1, jets2, eventWeight ), "");
  } else {
    // we are comparing clusters, so we just need to skip the dR matching and go straight to passing in the matched jets
    static SG::AuxElement::Decorator<ElementLink<xAOD::CaloClusterContainer> > parentClusterLink("ParentClusterLink");
    for(const auto jet1: *jets1){
      xAOD::Jet* jet2(new xAOD::Jet);
      jet2->makePrivateStore();
      const xAOD::CaloCluster* cluster = *(parentClusterLink(*jet1));
      xAOD::JetFourMom_t newp4(cluster->pt(), cluster->eta(), cluster->phi(), cluster->m());
      jet2->setJetP4(newp4);
      if(m_debug){
        std::cout << "Cluster Information" << std::endl;
        printf("\tOld ---- Pt: %0.4f\tEta: %0.4f\tPhi: %0.4f\tM: %0.4f\r\n", jet2->pt(), jet2->eta(), jet2->phi(), jet2->m());
        printf("\tNew ---- Pt: %0.4f\tEta: %0.4f\tPhi: %0.4f\tM: %0.4f\r\n", jet1->pt(), jet1->eta(), jet1->phi(), jet1->m());
      }

      RETURN_CHECK("JetComparisonHistsAlgo::execute()", m_plots->execute( jet1, jet2, eventWeight ), "");
      delete jet2;
    }
  }

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
