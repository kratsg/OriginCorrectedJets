#ifndef OriginCorrectedJets_JetComparisonHists_H
#define OriginCorrectedJets_JetComparisonHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include "xAODAnaHelpers/HelperClasses.h"
#include <xAODJet/JetContainer.h>

class JetComparisonHists : public HistogramManager
{
  public:


    JetComparisonHists(std::string name, std::string detailStr);
    virtual ~JetComparisonHists() ;

    bool m_debug;
    StatusCode initialize();
    StatusCode execute( const xAOD::JetContainer* jets_1, const xAOD::JetContainer* jets_2, float eventWeight);
    StatusCode execute( const xAOD::Jet* jet_1, const xAOD::JetContainer* jet_2, float eventWeight);
    using HistogramManager::book; // make other overloaded version of book() to show up in subclass
    using HistogramManager::execute; // overload

  protected:

    // holds bools that control which histograms are filled
    HelperClasses::JetInfoSwitch* m_infoSwitch;

  private:
    //basic
    TH1F* m_jetPt;                  //!
    TH1F* m_jetEta;                 //!
    TH1F* m_jetPhi;                 //!
    TH1F* m_jetM;                   //!
    TH1F* m_jetDR;                  //!

    // kinematic
    TH1F* m_jetPx;                  //!
    TH1F* m_jetPy;                  //!
    TH1F* m_jetPz;                  //!

    //NLeadingJets
    std::vector< TH1F* > m_NjetsPt;       //!
    std::vector< TH1F* > m_NjetsEta;      //!
    std::vector< TH1F* > m_NjetsPhi;      //!
    std::vector< TH1F* > m_NjetsM;        //!
    std::vector< TH1F* > m_NjetsDR;       //!

    // substructure
    TH1F* m_tau1; //!
    TH1F* m_tau2; //!
    TH1F* m_tau3; //!
    TH1F* m_tau21; //!
    TH1F* m_tau32; //!
    TH1F* m_tau1_wta; //!
    TH1F* m_tau2_wta; //!
    TH1F* m_tau3_wta; //!
    TH1F* m_tau21_wta; //!
    TH1F* m_tau32_wta; //!
};

#endif
