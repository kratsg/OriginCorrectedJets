#ifndef OriginCorrectedJets_OriginCorrectionVariableHists_H
#define OriginCorrectedJets_OriginCorrectionVariableHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include "xAODAnaHelpers/HelperClasses.h"
#include <xAODJet/JetContainer.h>

class OriginCorrectionVariableHists : public HistogramManager
{
  public:


    OriginCorrectionVariableHists(std::string name, std::string detailStr);
    virtual ~OriginCorrectionVariableHists() ;

    bool m_debug;
    StatusCode initialize();
    StatusCode execute( const xAOD::EventInfo* ei, float eventWeight);
    using HistogramManager::book; // make other overloaded version of book() to show up in subclass
    using HistogramManager::execute; // overload

    float m_dR = 0.3;

  private:
    //basic
    TH1F* m_jetPt;                  //!
    TH1F* m_jetEta;                 //!
    TH1F* m_jetPhi;                 //!
    TH1F* m_jetM;                   //!
    TH1F* m_jetDR;                  //!
};

#endif
