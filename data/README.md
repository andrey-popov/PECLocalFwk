# Data files

This directory contains data files organized into several categories:


## BTag

 * `BTagEff_76X_v1.root` <br />
   Efficiencies of b-tagging with CSVv2 algorithm, which are measured in 76X simulation. The event selection includes standard requirements on leptons, MtW > 50 GeV, and at least four jets with pt > 30 GeV and |eta| < 2.4.
 * `BTagSF_76X_CSVv2.csv` <br />
   Scale factors for b-tagging with CSVv2 algorithm, as measured in `CMSSW_7_6_X`. The file is copied from [this page](https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation76X?rev=24#Supported_Algorithms_and_Operati).


## LeptonSF

 * `ElectronSF_2015D_76Xv1.root` <br />
   Scale factors for electron reconstruction and tight working point of cut-based ID. Adapted from [here](https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2?rev=16#Electron_efficiencies_and_scale). The histograms are parameterized with electron transverse momentum and pseudorapidity of the associated supercluster. No appropriate scale factors are available for single-electron triggers.
 * `MuonSF_2015D_76Xv1.root` <br />
   Scale factors for muon identification and isolation (tight working points in both cases) and triggers `IsoMu20 OR IsoTkMu20`. Adapted from [here](https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceEffsRun2?rev=9#Results_for_CMSSW_7_6_X_dataset). Histogram errors are updated to include the additional uncertainties recommended by the reference. The trigger scale factors derived for Run2015D are combined taking corresponding integrated luminosities into account; Run2015C is ignored. No scale factors for `IsoMu18` are available.


## PileUp

 * `Run2015D_SingleMuon_v2_finebin.root`, `Run2015D_SingleElectron_v2_finebin.root` <br />
   Profiles of pile-up in data collected in Run2015D. They are constructed as described [here](https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData?rev=22#2015_Pileup_JSON_Files), using an effective minimum-bias cross section of 69 mb. The "golden" certification file was used. In case of electrons, runs with wrongly reconstructed online beam spot (listed [here](https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/2556.html)) are excluded.
 * `simPUProfiles_76X.root` <br />
   File with simulated pile-up profiles for the `RunIIFall15DR76` campaign. Only includes the nominal profile taken from [this configuration](https://github.com/cms-sw/cmssw/blob/CMSSW_7_6_4/SimGeneral/MixingModule/python/mix_2015_25ns_FallMC_matchData_PoissonOOTPU_cfi.py).


## JERC

Text files with jet energy corrections `Fall15_25nsV2` copied from [here](https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECDataMC?rev=112#Jet_Energy_Corrections_in_Run2).
They are mostly used for testing.
