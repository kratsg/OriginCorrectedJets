#ifndef OriginCorrectedJets_OriginCorrection_H
#define OriginCorrectedJets_OriginCorrection_H

// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

#include <fastjet/JetDefinition.hh>
#include <map>
#include <memory>

// all general tools used
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetRec/JetFilterTool.h"
#include "JetRec/JetRecTool.h"
// all jet modifier tools
#include "JetSubStructureMomentTools/JetChargeTool.h"
#include "JetSubStructureMomentTools/JetPullTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
#include "JetSubStructureMomentTools/DipolarityTool.h"
#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"
#include "JetMomentTools/JetWidthTool.h"

class OriginCorrection : public xAH::Algorithm
{
public:
  // configuration variables
  std::string m_inContainerName;
  std::string m_outContainerName;

  /// Kt, CamKt, AntiKt
  std::string m_jet_alg;
  float m_pt_min;
  float m_radius;

private:
  /* all tools we use */
    // this is for clustering
    std::unique_ptr<PseudoJetGetter> m_pseudoJetGetterTool;
    std::unique_ptr<JetFromPseudojet> m_jetFromPseudoJetTool;
    std::unique_ptr<JetFinder> m_jetFinderTool;
    std::unique_ptr<JetRecTool> m_originCorrectionTool;

    // modifier tools for the reclustered jets
    std::unique_ptr<JetChargeTool>              m_jetChargeTool;
    std::unique_ptr<JetPullTool>                m_jetPullTool;
    std::unique_ptr<EnergyCorrelatorTool>       m_energyCorrelatorTool;
    std::unique_ptr<EnergyCorrelatorRatiosTool> m_energyCorrelatorRatiosTool;
    std::unique_ptr<KTSplittingScaleTool>       m_ktSplittingScaleTool;
    std::unique_ptr<DipolarityTool>             m_dipolarityTool;
    std::unique_ptr<CenterOfMassShapesTool>     m_centerOfMassShapesTool;
    std::unique_ptr<JetWidthTool>               m_jetWidthTool;

public:
  // this is a standard constructor
  OriginCorrection (std::string className = "OriginCorrection");

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // these are the functions not inherited from Algorithm
  virtual EL::StatusCode configure ();


  /// @cond
  // this is needed to distribute the algorithm to the workers
  ClassDef(OriginCorrection, 1);
  /// @endcond

};

#endif
