from xAH_config import xAH_config
c = xAH_config()

''' Define all containers we use '''
caloClusters = "CaloCalTopoClusters"
truthJets    = "AntiKt10TruthTrimmedPtFrac5SmallR20Jets"
normalJets   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
pflowJets    = "AntiKt10EMCPFlowTrimmedPtFrac5SmallR20Jets"
''' These are output containers we make, names based on inputs used '''
originCorrectedClusters = "OriginCorrection{0:s}".format(caloClusters)
originCorrectedJets     = "OriginCorrection{0:s}".format(normalJets)

''' Set up all the algorithms '''
c.setalg("OriginCorrection", {"m_inContainerName": caloClusters,
                              "m_outContainerName": originCorrectedClusters,
                              "m_debug": True})

c.setalg("JetReclusteringAlgo", {"m_debug": false,
                                 "m_inputJetContainer": originCorrectedClusters,
                                 "m_outputJetContainer": originCorrectedJets,
                                 "m_ptMin_input": 0.0,
                                 "m_rc_algName": "antikt_algorithm",
                                 "m_radius": 1.0,
                                 "m_ptMin_rc": 100.0,
                                 "m_ptFrac": 0.05,
                                 "m_subjet_radius": 0.2,
                                 "m_name": "OriginCorrectedJets"
                                })

for container in [truthjets, normalJets, pflowJets, originCorrectedJets]:
  c.setalg("JetHistsAlgo", {"m_debug": False,
                            "m_inContainerName": container,
                            "m_detailStr": "kinematic",
                            "m_name": container
                          })
