# JERC factors

This subpackage provides means to compute corrections to jet energy and jet energy resolution, as well as the corresponding uncertainties.
Inputs are described in the form of standard text files used in JetMET POG (file format for [JEC](https://hypernews.cern.ch/HyperNews/CMS/get/jes/423/1.html) and [JER factors](https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyResolution?rev=14#Scale_factors_file_format)).


## Jet energy corrections

Source files related to JEC have been copied from [this directory](https://github.com/miquork/jecsys/tree/master/CondFormats/JetMETObjects), with some minor modifications.
They correspond to the state of the remote repository as of commit 194510cedf65259bc4b58092120df2b87e6a3b24, done on 15.04.2015 (direct [link](https://github.com/miquork/jecsys/commits/master/CondFormats/JetMETObjects) to the history of the remote directory).


## Jet energy resolution

Files with code for accessing JER factors have been copied from `CMSSW_8_0_8`, packages `CondFormats/JetMETObjects` and `JetMETCorrections/Modules`.
