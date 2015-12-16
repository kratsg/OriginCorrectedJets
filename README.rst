Origin-Corrected Topoclusters
=============================

Question: what are the performance differences between

* "normal", topocluster jets (Anti-Kt R=1.0 LC Topo Jets)
* p-flow jets
* origin-corrected jets

as compared to the truth jets? How does substructure change? How does the behavior of substructure top-tagging perform across the board?

This is for `HFSF2015 <https://github.com/US-ATLAS-HFSF/HFSF2015>`_.

Dependencies
------------

This depends on two packages: ``xAODAnaHelpers`` and ``xAODJetReclustering``. xAH is used to help with ntuple dumping and making plots, while the xAODJetReclustering package is used to help form appropriate origin-corrected jets by doing the clustering for us. This package will produce ``xAOD::Jet`` objects from topoclusters that have been origin-corrected. These jets will be reclustered into large-R jets (which is effectively the same idea as clustering, a hack if you will)::

  git clone git@github.com:UCATLAS/xAODAnaHelpers
  git clone git@github.com:kratsg/xAODJetReclustering
  cd xAODJetReclustering && git checkout useJetRecTrimmer && cd ../
  python xAODAnaHelpers/scripts/checkoutASGtags.py 2.3.38


Installing
----------

To install, simply run::

  rc clean
  rc find_packages
  rc compile

and we are good to go.

Files
=====

Test Files
----------

A test file has been placed on UCTier3 at::

  /share/t3data3/kratsg/xAODs/HFSF/pflowTest.root

Datasets
--------

We need datasets that have the pflow objects -- see `this bigpanda page (12/15/2015) <http://bigpanda.cern.ch/tasks/?workinggroup=perf-jets>`_ for a list of datasets at the time of writing. We reiterate the list below::

  group.perf-jets.mc15_13TeV.361027.Py8EG_A14NNPDF23LO_jetjet_JZ7W.e3668_s2608_s2183_r6765_r6282_p2471.DAOD_JETM8.131215.v1_EXT0/
  group.perf-jets.mc15_13TeV.361026.Py8EG_A14NNPDF23LO_jetjet_JZ6W.e3569_s2608_s2183_r6765_r6282_p2471.DAOD_JETM8.131215.v1_EXT0/
  group.perf-jets.mc15_13TeV.361028.Py8EG_A14NNPDF23LO_jetjet_JZ8W.e3569_s2576_s2132_r6765_r6282_p2471.DAOD_JETM8.111215.v1_EXT0/
  group.perf-jets.mc15_13TeV.410000.PhPyEG_P2012_ttbar_hdamp172p5_nonAH.e3698_s2608_s2183_r6765_r6282_p2471.DAOD_JETM8.111215.v2_EXT0/
  group.perf-jets.mc15_13TeV.301332.Py8EG_A14NNPDF23LO_zprime2750_tt.e4061_s2608_s2183_r6869_r6282_p2471.DAOD_JETM8.111215.v1_EXT0/
  group.perf-jets.mc15_13TeV.301330.Py8EG_A14NNPDF23LO_zprime2250_tt.e4061_s2608_s2183_r6869_r6282_p2471.DAOD_JETM8.111215.v1_EXT0/
  group.perf-jets.mc15_13TeV.361025.Py8EG_A14NNPDF23LO_jetjet_JZ5W.e3668_s2576_s2132_r6765_r6282_p2471.DAOD_JETM8.111215.v1_EXT0/
  group.perf-jets.mc15_13TeV.361024.Py8EG_A14NNPDF23LO_jetjet_JZ4W.e3668_s2576_s2132_r6765_r6282_p2471.DAOD_JETM8.111215.v1_EXT0/

Physicists
==========

* Giordon Stark
