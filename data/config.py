from xAH_config import xAH_config
c = xAH_config()

c.setalg("OriginCorrection", {"m_inContainerName": "CaloClusters",
                              "m_outContainerName": "OriginCorrectedCaloClusters",
                              "m_debug": True})

c.setalg("JetHistsAlgo", {"m_debug": False,
                          "m_inContainerName": "",
                          "m_detailStr": "kinematic",
                          "m_name": "NoPreSel"
                        })

c.setalg("JetReclusteringAlgo", {"m_debug": false,
                                 "m_inputJetContainer": "AntiKt4EMTopoJets",
                                 "m_outputJetContainer": "OriginCorrectedAntiKt10LCTopoTrimmedJets",
                                 "m_ptMin_input": 0.0,
                                 "m_rc_algName": "antikt_algorithm",
                                 "m_radius": 1.0,
                                 "m_ptMin_rc": 100.0,
                                 "m_ptFrac": 0.05,
                                 "m_subjet_radius": 0.3,
                                 "m_name": "OriginCorrectedJets"
                                })
