# Data files

This directory contains data files organized into several categories:


## BTag

 * `BTagEff_74X_v1.0.root` <br />
   Efficiencies of b-tagging with CSVv2 algorithm, which are measured in 74X simulation. The event selection includes standard requirements on leptons, MtW > 40 GeV, and at least four jets with pt > 30 GeV and |eta| < 2.4.
 * `BTagSF_74X_CSVv2.csv` <br />
   Scale factors for b-tagging with CSVv2 algorithm, as measured in `CMSSW_7_4_X`. The file is copied from [this page](https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation74X?rev=12#Supported_Algorithms_and_Operati).
 * `CSVWeights_74X_v1.0.root` <br />
   Scale factors for CSV reweighting derived in the search for ttH (AN-15-216). Histograms contained in this file are copied from files `csv_rwt_fit_*_2015_11_20.root` taken from [here](https://github.com/cms-ttH/MiniAOD/tree/5f72d163c5ab3a5f2eb79bda324fadce72e54460/MiniAODHelper/data). For convenience, groups of 1D histograms in CSV for various (pt, |eta|) bins have been converted into 3D histograms. Under- and overflow bins are set appropriately (most notably, overflow in pt and underflow in CSV).


## PileUp

 * `Run2015D_SingleMuon_v1.0_finebin.root`, `Run2015D_SingleElectron_v1.0_finebin.root` <br />
   Profiles of pile-up in data collected in Run2015D. They are constructed as described [here](https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData?rev=22#2015_Pileup_JSON_Files), using an effective minimum-bias cross section of 69 mb. The "golden" certification file was used. In case of electrons, runs with wrongly reconstructed online beam spot (listed [here](https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/2556.html)) are excluded.
