from xAH_config import xAH_config
c = xAH_config()

''' Define all containers we use '''
caloClusters = "CaloCalTopoClusters"
truthJets    = "AntiKt10TruthTrimmedPtFrac5SmallR20Jets"
normalJets   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
pflowJets    = "AntiKt10EMCPFlowTrimmedPtFrac5SmallR20Jets"
''' These are output containers we make, names based on inputs used '''
uncorrectedClusters = "Uncorrected{0:s}".format(caloClusters)
originCorrectedClusters = "OriginCorrection{0:s}".format(caloClusters)
originCorrectedJets     = "OriginCorrection{0:s}".format(normalJets)

''' Set up all the algorithms '''
c.setalg("BasicEventSelection", {"m_name": "BasicEventSelection",
                                 "m_applyGRLCut": False,
                                 "m_cleanPowheg": False,
                                 "m_doPUreweighting": False,
                                 "m_applyPrimaryVertexCut": False,
                                 "m_applyEventCleaningCut": False,
                                 "m_applyCoreFlagsCut": False,
                                 "m_applyTriggerCut": False,
                                 "m_useMetaData": False,
                                 "m_checkDuplicatesData": False,
                                 "m_checkDuplicatesMC": False})

c.setalg("OriginCorrection", {"m_inContainerName": caloClusters,
                              "m_outContainerName": originCorrectedClusters})

c.setalg("OriginCorrection", {"m_inContainerName": caloClusters,
                              "m_outContainerName": uncorrectedClusters,
                              "m_doCorrection": False})

c.setalg("JetReclusteringAlgo", {"m_debug": False,
                                 "m_inputJetContainer": originCorrectedClusters,
                                 "m_outputJetContainer": originCorrectedJets,
                                 "m_ptMin_input": 0.0,
                                 "m_rc_algName": "antikt_algorithm",
                                 "m_radius": 1.0,
                                 "m_ptMin_rc": 25.0,
                                 "m_ptFrac": 0.05,
                                 "m_subjet_radius": 0.2,
                                 "m_name": "OriginCorrectedJets"
                                })

for container in [truthJets, normalJets, pflowJets, originCorrectedJets, uncorrectedClusters, originCorrectedClusters]:
  c.setalg("JetHistsAlgo", {"m_debug": False,
                            "m_inContainerName": container,
                            "m_detailStr": "kinematic substructure 2LeadingJets",
                            "m_name": "kinematics/{0:s}".format(container)
                          })

for left,right,compareClusters in [(originCorrectedClusters, "", True),
                   (normalJets, originCorrectedJets, False),
                   (normalJets, truthJets, False),
                   (normalJets, pflowJets, False),
                   (originCorrectedJets, truthJets, False),
                   (pflowJets, truthJets, False),
                   (pflowJets, originCorrectedJets, False)]:
  c.setalg("JetComparisonHistsAlgo", {"m_debug": False,
                                      "m_inContainer1Name": left,
                                      "m_inContainer2Name": right,
                                      "m_detailStr": "kinematic substructure 2LeadingJets",
                                      "m_name": "comparisons/{0:s}v{1:s}".format(left, right),
                                      "m_compareClusters": compareClusters
                                     })
