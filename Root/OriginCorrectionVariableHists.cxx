#include <OriginCorrectedJets/OriginCorrectionVariableHists.h>

#include "xAODAnaHelpers/tools/ReturnCheck.h"

using std::vector;

OriginCorrectionVariableHists :: OriginCorrectionVariableHists (std::string name, std::string detailStr) :
  HistogramManager(name, detailStr)
{
  m_debug = false;
}

OriginCorrectionVariableHists :: ~OriginCorrectionVariableHists () {}

StatusCode OriginCorrectionVariableHists::initialize() {

  // These plots are always made
  m_jetPt          = book(m_name, "jetdPt",  "jet #Delta p_{T} [GeV]", 300, -150, 150.);
  m_jetEta         = book(m_name, "jetdEta", "jet #Delta#eta",         200, -1.0, 1.0);
  m_jetPhi         = book(m_name, "jetdPhi", "jet #Delta#phi",200, -1.0, 1.0 );
  m_jetM           = book(m_name, "jetdMass", "jet #Delta Mass [GeV]", 200, -1000, 1000);
  m_jetDR          = book(m_name, "jetDR", "jet #Delta R", 100, 0.0, 1.0);

  if(m_debug) Info("OriginCorrectionVariableHists::initialize()", m_name.c_str());
  return StatusCode::SUCCESS;
}

StatusCode OriginCorrectionVariableHists::execute( const xAOD::EventInfo* ei, float eventWeight ) {
  if(m_debug) std::cout << "in execute " <<std::endl;

  //basic
  m_jetPt ->        Fill( (jet1->pt()-jet2->pt())/1e3,    eventWeight );
  m_jetEta->        Fill( xAOD::P4Helpers::deltaEta(jet1, jet2),       eventWeight );
  m_jetPhi->        Fill( xAOD::P4Helpers::deltaPhi(jet1, jet2),       eventWeight );
  m_jetM->          Fill( (jet1->m()-jet2->m())/1e3,     eventWeight );
  m_jetDR->         Fill( xAOD::P4Helpers::deltaR(jet1, jet2),     eventWeight );

  return StatusCode::SUCCESS;
}

