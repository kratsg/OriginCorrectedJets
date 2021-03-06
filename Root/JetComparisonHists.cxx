#include <OriginCorrectedJets/JetComparisonHists.h>
#include <sstream>

#include <FourMomUtils/xAODP4Helpers.h>
#include "xAODAnaHelpers/tools/ReturnCheck.h"

using std::vector;

JetComparisonHists :: JetComparisonHists (std::string name, std::string detailStr) :
  HistogramManager(name, detailStr),
  m_infoSwitch(new HelperClasses::JetInfoSwitch(m_detailStr))
{
  m_debug = false;
}

JetComparisonHists :: ~JetComparisonHists () {
  if(m_infoSwitch) delete m_infoSwitch;
}

StatusCode JetComparisonHists::initialize() {

  // These plots are always made
  m_jetPt          = book(m_name, "jetdPt",  "jet #Delta p_{T} [GeV]", 600, -150,  150.);
  m_jetEta         = book(m_name, "jetdEta", "jet #Delta#eta",         400, -1.0,   1.0);
  m_jetPhi         = book(m_name, "jetdPhi", "jet #Delta#phi",         400, -1.0,   1.0);
  m_jetM           = book(m_name, "jetdMass", "jet #Delta Mass [GeV]", 400, -1000, 1000);
  m_jetDR          = book(m_name, "jetDR", "jet #Delta R",             200,   0.0,  1.0);

  if(m_debug) Info("JetComparisonHists::initialize()", m_name.c_str());
  // details of the jet kinematics
  if( m_infoSwitch->m_kinematic ) {
    if(m_debug) Info("JetComparisonHists::initialize()", "adding kinematic plots");
    m_jetPx     = book(m_name, "jetdPx",     "jet #Delta Px [GeV]",     300, -150, 150);
    m_jetPy     = book(m_name, "jetdPy",     "jet #Delta Py [GeV]",     300, -150, 150);
    m_jetPz     = book(m_name, "jetdPz",     "jet #Delta Pz [GeV]",     300, -150, 150);
  }

  // N leading jets
  if( m_infoSwitch->m_numLeading > 0 ){
    std::stringstream jetNum;
    std::stringstream jetTitle;
    for(int iJet=0; iJet < m_infoSwitch->m_numLeading; ++iJet){
      jetNum << iJet; jetTitle << iJet+1;
      switch(iJet) {
        case 0:
          jetTitle << "^{st}";
          break;
        case 1:
          jetTitle << "^{nd}";
          break;
        case 2:
          jetTitle << "^{rd}";
          break;
        default:
          jetTitle << "^{th}";
          break;
      }

      m_NjetsPt.push_back(       book(m_name, ("jetdPt_jet"+jetNum.str()),       jetTitle.str()+" jet #Delta p_{T} [GeV]" ,300, -150,   150) );
      m_NjetsEta.push_back(      book(m_name, ("jetdEta_jet"+jetNum.str()),      jetTitle.str()+" jet #Delta #eta"        ,200, -1.0,   1.0) );
      m_NjetsPhi.push_back(      book(m_name, ("jetdPhi_jet"+jetNum.str()),      jetTitle.str()+" jet #Delta Phi"         ,200, -1.0,   1.0) );
      m_NjetsM.push_back(        book(m_name, ("jetdMass_jet"+jetNum.str()),     jetTitle.str()+" jet #Delta Mass [GeV]"  ,200, -1000, 1000) );
      m_NjetsDR.push_back(       book(m_name, ("jetDR_jet"+jetNum.str()),        jetTitle.str()+" jet #Delta R"           ,100,     0,    1) );
      jetNum.str("");
      jetTitle.str("");
    }//for iJet
  }


  if( m_infoSwitch->m_substructure ){
    m_tau1                      = book(m_name, "dTau1", "#Delta#Tau_{1}", 200, -1.0, 1.0);
    m_tau2                      = book(m_name, "dTau2", "#Delta#Tau_{2}", 200, -1.0, 1.0);
    m_tau3                      = book(m_name, "dTau3", "#Delta#Tau_{3}", 200, -1.0, 1.0);
    m_tau21                     = book(m_name, "dTau21", "#Delta#Tau_{21}", 200, -1.0, 1.0);
    m_tau32                     = book(m_name, "dTau32", "#Delta#Tau_{32}", 200, -1.0, 1.0);
    m_tau1_wta                  = book(m_name, "dTau1_wta", "#Delta#Tau_{1}^{wta}", 200, -1.0, 1.0);
    m_tau2_wta                  = book(m_name, "dTau2_wta", "#Delta#Tau_{2}^{wta}", 200, -1.0, 1.0);
    m_tau3_wta                  = book(m_name, "dTau3_wta", "#Delta#Tau_{3}^{wta}", 200, -1.0, 1.0);
    m_tau21_wta                 = book(m_name, "dTau21_wta", "#Delta#Tau_{21}^{wta}", 200, -1.0, 1.0);
    m_tau32_wta                 = book(m_name, "dTau32_wta", "#Delta#Tau_{32}^{wta}", 200, -1.0, 1.0);
    m_numConstituents           = book(m_name, "numConstituents", "num. constituents", 1001, -500.5, 500.5);
  }

  return StatusCode::SUCCESS;
}

StatusCode JetComparisonHists::execute( const xAOD::JetContainer* jets1, const xAOD::JetContainer* jets2, float eventWeight ) {
  // apply dR matching here
  for(const auto jet1: *jets1)
    for(const auto jet2: *jets2)
      if(xAOD::P4Helpers::isInDeltaR(*jet1, *jet2, m_dR))
        if(!this->execute( jet1, jet2, eventWeight ).isSuccess())
          return StatusCode::FAILURE;

  if( m_infoSwitch->m_numLeading > 0){
    int numJets = std::min( m_infoSwitch->m_numLeading, (int)jets1->size() );
    for(int iJet=0; iJet < numJets; ++iJet){
      auto jet1 = jets1->at(iJet);
      for(const auto jet2: *jets2){
        if(xAOD::P4Helpers::isInDeltaR(*jet1, *jet2, m_dR)){
          m_NjetsPt.at(iJet)->        Fill( (jet1->pt()-jet2->pt())/1e3,   eventWeight);
          m_NjetsEta.at(iJet)->       Fill( xAOD::P4Helpers::deltaEta(jet1, jet2),      eventWeight);
          m_NjetsPhi.at(iJet)->       Fill( xAOD::P4Helpers::deltaPhi(jet1, jet2),      eventWeight);
          m_NjetsM.at(iJet)->         Fill( (jet1->m()-jet2->m())/1e3,    eventWeight);
          m_NjetsDR.at(iJet)->        Fill( xAOD::P4Helpers::deltaR(jet1, jet2),    eventWeight);
          break;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode JetComparisonHists::execute( const xAOD::Jet* jet1, const xAOD::Jet* jet2, float eventWeight ) {

  if(m_debug) std::cout << "in execute " <<std::endl;

  //basic
  m_jetPt ->        Fill( (jet1->pt()-jet2->pt())/1e3,    eventWeight );
  m_jetEta->        Fill( xAOD::P4Helpers::deltaEta(jet1, jet2),       eventWeight );
  m_jetPhi->        Fill( xAOD::P4Helpers::deltaPhi(jet1, jet2),       eventWeight );
  m_jetM->          Fill( (jet1->m()-jet2->m())/1e3,     eventWeight );
  m_jetDR->         Fill( xAOD::P4Helpers::deltaR(jet1, jet2),     eventWeight );

  // kinematic
  if( m_infoSwitch->m_kinematic ) {
    m_jetPx->  Fill( (jet1->px()-jet2->px())/1e3,  eventWeight );
    m_jetPy->  Fill( (jet1->py()-jet2->py())/1e3,  eventWeight );
    m_jetPz->  Fill( (jet1->pz()-jet2->pz())/1e3,  eventWeight );
  } // fillKinematic

  if( m_infoSwitch->m_substructure ){
    static SG::AuxElement::ConstAccessor<float> Tau1("Tau1");
    static SG::AuxElement::ConstAccessor<float> Tau2("Tau2");
    static SG::AuxElement::ConstAccessor<float> Tau3("Tau3");
    static SG::AuxElement::ConstAccessor<float> Tau1_wta("Tau1_wta");
    static SG::AuxElement::ConstAccessor<float> Tau2_wta("Tau2_wta");
    static SG::AuxElement::ConstAccessor<float> Tau3_wta("Tau3_wta");

    if(Tau1.isAvailable(*jet1) && Tau1.isAvailable(*jet2)) m_tau1->Fill( (Tau1(*jet1)-Tau1(*jet2)), eventWeight );
    if(Tau2.isAvailable(*jet1) && Tau2.isAvailable(*jet2)) m_tau2->Fill( (Tau2(*jet1)-Tau2(*jet2)), eventWeight );
    if(Tau3.isAvailable(*jet1) && Tau3.isAvailable(*jet2)) m_tau3->Fill( (Tau3(*jet1)-Tau3(*jet2)), eventWeight );
    if(Tau1.isAvailable(*jet1) && Tau2.isAvailable(*jet1) && Tau1.isAvailable(*jet2) && Tau2.isAvailable(*jet2)) m_tau21->Fill( (Tau2(*jet1)/Tau1(*jet1) - Tau2(*jet2)/Tau1(*jet2)), eventWeight );
    if(Tau2.isAvailable(*jet1) && Tau3.isAvailable(*jet1) && Tau2.isAvailable(*jet2) && Tau3.isAvailable(*jet2)) m_tau32->Fill( (Tau3(*jet1)/Tau2(*jet1) - Tau3(*jet2)/Tau2(*jet2)), eventWeight );
    if(Tau1_wta.isAvailable(*jet1) && Tau1_wta.isAvailable(*jet2)) m_tau1_wta->Fill( (Tau1_wta(*jet1) - Tau1_wta(*jet2)), eventWeight );
    if(Tau2_wta.isAvailable(*jet1) && Tau2_wta.isAvailable(*jet2)) m_tau2_wta->Fill( (Tau2_wta(*jet1) - Tau2_wta(*jet2)), eventWeight );
    if(Tau3_wta.isAvailable(*jet1) && Tau3_wta.isAvailable(*jet2)) m_tau3_wta->Fill( (Tau3_wta(*jet1) - Tau3_wta(*jet2)), eventWeight );
    if(Tau1_wta.isAvailable(*jet1) && Tau2_wta.isAvailable(*jet1) && Tau1_wta.isAvailable(*jet2) && Tau2_wta.isAvailable(*jet2)) m_tau21_wta->Fill( (Tau2_wta(*jet1)/Tau1_wta(*jet1) - Tau2_wta(*jet2)/Tau1_wta(*jet2)), eventWeight );
    if(Tau2_wta.isAvailable(*jet1) && Tau3_wta.isAvailable(*jet1) && Tau2_wta.isAvailable(*jet2) && Tau3_wta.isAvailable(*jet2)) m_tau32_wta->Fill( (Tau3_wta(*jet1)/Tau2_wta(*jet1) - Tau3_wta(*jet2)/Tau2_wta(*jet2)), eventWeight );

    m_numConstituents->Fill( jet1->numConstituents() - jet2->numConstituents(), eventWeight );

  }

  return StatusCode::SUCCESS;
}

