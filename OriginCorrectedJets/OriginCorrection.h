#ifndef OriginCorrectedJets_OriginCorrection_H
#define OriginCorrectedJets_OriginCorrection_H

#include <TH1F.h>

// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

class OriginCorrection : public xAH::Algorithm
{
public:
  // configuration variables
  std::string m_inContainerName;
  std::string m_outContainerName;
  bool m_doCorrection;
  bool m_plotCorrectionVariables;
  std::string m_vertexContainerName;

private:

  TH1F* m_primaryVertex_z; //!
  TH1F* m_cluster_centerMag; //!

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

  /// @cond
  // this is needed to distribute the algorithm to the workers
  ClassDef(OriginCorrection, 1);
  /// @endcond

};

#endif
