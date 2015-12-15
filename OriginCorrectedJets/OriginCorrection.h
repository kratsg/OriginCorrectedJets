#ifndef OriginCorrectedJets_OriginCorrection_H
#define OriginCorrectedJets_OriginCorrection_H

// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

class OriginCorrection : public xAH::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  // configuration variables
  std::string m_inContainerName;
  std::string m_outContainerName;

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
