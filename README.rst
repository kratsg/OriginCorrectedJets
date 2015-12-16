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


Physicists
==========

* Giordon Stark
