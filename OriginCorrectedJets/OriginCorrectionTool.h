#ifndef OriginCorrectedJets_OriginCorrectionTool_H
#define OriginCorrectedJets_OriginCorrectionTool_H

/// Giordon Stark \n
/// May 2015
///
/// Tool to trim reclustered jets

#include "JetRec/JetModifierBase.h"

class OriginCorrectionTool : public JetModifierBase {
  ASG_TOOL_CLASS(OriginCorrectionTool, IJetModifier)

  public:
    // Constructor from tool name.
    OriginCorrectionTool(std::string myname);

    // Inherited method to modify a jet.
    // Calls width and puts the result on the jet.
    virtual int modifyJet(xAOD::Jet& jet) const;

  private:
    float m_ptFrac;
};

#endif
