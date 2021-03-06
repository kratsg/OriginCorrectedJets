// EL include(s):
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

// package include(s):
#include "xAODAnaHelpers/HelperFunctions.h"
#include "OriginCorrectedJets/OriginCorrection.h"
#include <xAODAnaHelpers/tools/ReturnCheck.h>

// EDM includes
#include <xAODEventInfo/EventInfo.h>
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// ROOT include(s):
#include "TEnv.h"
#include "TSystem.h"

// C++ includes
#include <cmath>

// this is needed to distribute the algorithm to the workers
ClassImp(OriginCorrection)

OriginCorrection :: OriginCorrection (std::string className) :
    Algorithm(className)
{
  Info("OriginCorrection()", "Calling constructor");

  // read debug flag from .config file
  m_debug                   = false;

  m_inContainerName         = "";
  m_outContainerName        = "";
  m_doCorrection            = true;
  m_plotCorrectionVariables = true;
  m_vertexContainerName     = "PrimaryVertices";
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

  // If there is no InputContainer we must stop
  if ( m_inContainerName.empty() ) {
    Error("initialize()", "InputContainer is empty!");
    return EL::StatusCode::FAILURE;
  }

  if(m_outContainerName.empty()) m_outContainerName = "OriginCorrected"+m_inContainerName;

  Info("initialize()", "OriginCorrection Interface succesfully configured! ");

  if(m_plotCorrectionVariables){
    m_primaryVertex_z = new TH1F( "originCorrection/vxp_z", "vxp_z", 1000, -500, 500 );
    m_primaryVertex_z->GetXaxis()->SetTitle("primary vertex z_{0}");
    m_primaryVertex_z->Sumw2(true);
    wk()->addOutput(m_primaryVertex_z);

    m_cluster_centerMag = new TH1F( "originCorrection/cluster_centerMag", "cluster_centerMag", 1000, 0, 10000 );
    m_cluster_centerMag->GetXaxis()->SetTitle("cluster CENTER_MAG");
    m_cluster_centerMag->Sumw2(true);
    wk()->addOutput(m_cluster_centerMag);
  }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode OriginCorrection :: execute ()
{
  if ( m_debug ) { Info("execute()", "Applying Jet Origin Correction... "); }

  const xAOD::EventInfo* eventInfo(nullptr);
  RETURN_CHECK("JetComparisonHistsAlgo::execute()", HelperFunctions::retrieve(eventInfo, m_eventInfoContainerName, m_event, m_store, m_verbose) ,"");

  float eventWeight(1);
  if( eventInfo->isAvailable< float >( "mcEventWeight" ) ) {
    eventWeight = eventInfo->auxdecor< float >( "mcEventWeight" );
  }

  // get the calorimeter clusters
  const xAOD::CaloClusterContainer* inClusters(nullptr);
  RETURN_CHECK("OriginCorrection::execute()", HelperFunctions::retrieve(inClusters, m_inContainerName, m_event, m_store, m_verbose) ,"");

  // get the primary vertex's z-coordinate
  const xAOD::VertexContainer* vertices(nullptr);
  RETURN_CHECK("OriginCorrection::execute()", HelperFunctions::retrieve(vertices, m_vertexContainerName, m_event, m_store, m_verbose), "");
  const xAOD::Vertex* primaryVertex = vertices->at(HelperFunctions::getPrimaryVertexLocation(vertices));

  if(m_plotCorrectionVariables) m_primaryVertex_z->Fill( primaryVertex->z(), eventWeight );

  xAOD::JetContainer* outJets(new xAOD::JetContainer);
  xAOD::JetAuxContainer* outJetsAux(new xAOD::JetAuxContainer);
  outJets->setStore(outJetsAux);

  RETURN_CHECK("OriginCorrection::execute()", m_store->record(outJets, m_outContainerName), "Could not record output container to TStore.");
  RETURN_CHECK("OriginCorrection::execute()", m_store->record(outJetsAux, m_outContainerName+"Aux."), "Could not record output aux container to TStore.");

  static SG::AuxElement::Decorator<ElementLink<xAOD::CaloClusterContainer> > parentClusterLink("ParentClusterLink");
  for(const auto cluster: *inClusters){
    // first, retrieve center mag: Cluster Centroid (\f$\sqrt(x^2+y^2+z^2)\f$)
    //      if it is somehow missing, we will just error out because it should not be missing
    double center_mag(-999.0);
    if(!cluster->retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_MAG, center_mag)) return EL::StatusCode::FAILURE;

    if(m_plotCorrectionVariables) m_cluster_centerMag->Fill( center_mag, eventWeight );

    // create and output jets
    xAOD::Jet* outJet(new xAOD::Jet);
    outJets->push_back(outJet);
    // build element links
    ElementLink<xAOD::CaloClusterContainer> el_cluster( *inClusters, cluster->index() );
    parentClusterLink(*outJet) = el_cluster;
    // calculate the correction and set the jet's 4-vector
    float new_eta = cluster->eta();
    float new_pt = cluster->pt();
    if(m_doCorrection){
      float radius = center_mag/std::cosh(cluster->eta());
      new_eta = std::asinh(std::sinh(cluster->eta())-primaryVertex->z()/radius);
      new_pt = cluster->pt() * std::cosh(cluster->eta())/std::cosh(new_eta);
    }
    float new_phi = cluster->phi();
    float new_m = cluster->m();
    if(m_debug){
      std::cout << "Cluster Information" << std::endl;
      printf("\tOld ---- Pt: %0.4f\tEta: %0.4f\tPhi: %0.4f\tM: %0.4f\r\n", cluster->pt(), cluster->eta(), cluster->phi(), cluster->m());
      printf("\tNew ---- Pt: %0.4f\tEta: %0.4f\tPhi: %0.4f\tM: %0.4f\r\n", new_pt, new_eta, new_phi, new_m);
    }
    xAOD::JetFourMom_t newp4(new_pt, new_eta, new_phi, new_m);
    outJet->setJetP4(newp4);
  }

  /* ***FROM PROOFANA***
      TLorentzVector part = ((Particle* ) theJet->Obj(ConstType,iParticle))->p;

      if (trackaxis==2){
          float radius = ((Particle* ) theJet->Obj(ConstType,iParticle))->Float("centermag")/cosh(part.Eta());
          float etaCorr2=part.Eta()-Float("vxp_z_at_0")/(radius*cosh(part.Eta()));
          float etaCorr=TMath::ASinH(sinh(part.Eta())-Float("vxp_z_at_0")/radius);
          part.SetPtEtaPhiM(part.Pt()*cosh(part.Eta())/cosh(etaCorr),etaCorr,part.Phi(),0.0);

      }

  something like this. the "centermag" moment is what you want.
  */


  return EL::StatusCode::SUCCESS;
}



EL::StatusCode OriginCorrection :: postExecute ()
{
  if ( m_debug ) { Info("postExecute()", "Calling postExecute"); }
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode OriginCorrection :: finalize () { return EL::StatusCode::SUCCESS; }

EL::StatusCode OriginCorrection :: histFinalize ()
{
  Info("histFinalize()", "Calling histFinalize");
  RETURN_CHECK("xAH::Algorithm::algFinalize()", xAH::Algorithm::algFinalize(), "");
  return EL::StatusCode::SUCCESS;
}
