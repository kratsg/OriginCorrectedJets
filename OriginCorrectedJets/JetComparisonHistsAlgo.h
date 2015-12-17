#ifndef OriginCorrectedJets_JetComparisonHistsAlgo_H
#define OriginCorrectedJets_JetComparisonHistsAlgo_H

#include <OriginCorrectedJets/JetComparisonHists.h>

// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

class JetComparisonHistsAlgo : public xAH::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  // configuration variables
  std::string m_inContainer1Name;
  std::string m_inContainer2Name;
  std::string m_detailStr;
  float m_dR;
  bool m_compareClusters;

private:
  JetComparisonHists* m_plots; //!

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
public:
  // Tree *myTree; //!
  // TH1 *myHist; //!



  // this is a standard constructor
  JetComparisonHistsAlgo (std::string className = "JetComparisonHistsAlgo");

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
  ClassDef(JetComparisonHistsAlgo, 1);
  /// @endcond

};

#endif
