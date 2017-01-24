# Data files

This directory contains data files organized into several categories:


## BTag

 * `BTagWP_80Xv2.json` <br />
   Thresholds that define standard working points for b-tagging algorithms for re-reconstruction of 2016 data as given [here](https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80XReReco?rev=3).
 * `BTagWP_80Xv1.json` <br />
   Thresholds that define standard working points for b-tagging algorithms for promt reconstruction of 2016 data as given [here](https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation80X?rev=18).
 * `BTagEff_80Xv1.root` <br />
   Efficiencies of b-tagging with cMVAv2 and CSVv2 algorithms. Measured in POWHEG ttbar sample in `CMSSW_8_0_X`. The event selection targets l+jets signature and asks for at least for jets with pt > 30 GeV and |eta| < 2.4.
 * `BTagSF_cMVAv2_80Xv1.csv` <br />
   Scale factors for b-tagging with cMVAv2 algorithm, as measured in `CMSSW_8_0_X`. The file is copied from [this page](https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation80X?rev=18#Supported_Algorithms_and_Operati).
 * `BTagEff_76X_v1.root` <br />
   Efficiencies of b-tagging with CSVv2 algorithm, which are measured in 76X simulation. The event selection includes standard requirements on leptons, MtW > 50 GeV, and at least four jets with pt > 30 GeV and |eta| < 2.4.
 * `BTagSF_76X_CSVv2.csv` <br />
   Scale factors for b-tagging with CSVv2 algorithm, as measured in `CMSSW_7_6_X`. The file is copied from [this page](https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation76X?rev=24#Supported_Algorithms_and_Operati).


## LeptonSF

 * `ElectronSF_2016BCD_80Xv2.root` <br />
   Scale factors for efficiencies of electron track reconstruction and tight working point of cut-based ID, scale factors and efficiencies of trigger `Ele32_eta2p1_WPTight_Gsf`. Adapted from [here](https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2?rev=34#Efficiencies_and_scale_factors) and [here](https://indico.cern.ch/event/572302/contributions/2316049/). Derived on the ICHEP'16 dataset.
 * `MuonSF_2016BCD_80Xv2.root` <br />
   Scale factors for efficiencies of muon track reconstruction, tight identification, and tight isolation. Also includes efficiency of triggers `IsoMu22 OR IsoTkMu22` in data. Adapted from [here](https://twiki.cern.ch/twiki/bin/view/CMS/MuonWorkInProgressAndPagResults?rev=16). Trigger efficiencies are mixed for the ICHEP'16 dataset. In addition to the central trigger efficiences, scale factors and efficiencies for `IsoMu24 OR IsoTkMu24` reported [here](https://indico.cern.ch/event/572303/contributions/2329366/) are included.
 * `ElectronSF_2015D_76Xv2.root` <br />
   Scale factors for electron reconstruction, tight working point of cut-based ID, and trigger `Ele23_WPLoose_Gsf`. First two are adapted from [here](https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2?rev=16#Electron_efficiencies_and_scale). Trigger scale factors are taken from [this talk](https://indico.cern.ch/event/491510/contributions/2246475/attachments/1310813/1962318/2016.07.18_EGM_Electron-trigger-SF_v2.pdf). The histograms are parameterized with electron transverse momentum and pseudorapidity of the associated supercluster.
 * `MuonSF_2015D_76Xv1.root` <br />
   Scale factors for muon identification and isolation (tight working points in both cases) and triggers `IsoMu20 OR IsoTkMu20`. Adapted from [here](https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceEffsRun2?rev=9#Results_for_CMSSW_7_6_X_dataset). Histogram errors are updated to include the additional uncertainties recommended by the reference. The trigger scale factors derived for Run2015D are combined taking corresponding integrated luminosities into account; Run2015C is ignored. No scale factors for `IsoMu18` are available.


## PileUp

 * `simPUProfiles_Moriond17.root` <br />
   File with simulated pileup profiles for the `RunIISummer16` campaign. Only includes the nominal profile taken from [this configuration](https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_24/SimGeneral/MixingModule/python/mix_2016_25ns_Moriond17MC_PoissonOOTPU_cfi.py).
 * `simPUProfiles_80X.root` <br />
   File with simulated pileup profiles for the `RunIISpring16DR80` campaign. Only includes the nominal profile taken from [this configuration](https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_8/SimGeneral/MixingModule/python/mix_2016_25ns_SpringMC_PUScenarioV1_PoissonOOTPU_cfi.py).
 * `Run2016BCD_SingleElectron_v1_finebin.root`, `Run2016BCD_SingleMuon_v1_finebin.root` <br />
   Pileup profiles in data corresponding to ICHEP'16 dataset. Constructed mostly following [this recipe](https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData?rev=26#Pileup_JSON_Files_For_Run_II), using an effective cross section of 69.2 mb and "golden" certification file.
 * `Run2015D_SingleMuon_v2_finebin.root`, `Run2015D_SingleElectron_v3_finebin.root` <br />
   Profiles of pileup in data collected in Run2015D. They are constructed as described [here](https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData?rev=22#2015_Pileup_JSON_Files), using an effective minimum-bias cross section of 69 mb. The "golden" certification file was used.
 * `simPUProfiles_76X.root` <br />
   File with simulated pile-up profiles for the `RunIIFall15DR76` campaign. Only includes the nominal profile taken from [this configuration](https://github.com/cms-sw/cmssw/blob/CMSSW_7_6_4/SimGeneral/MixingModule/python/mix_2015_25ns_FallMC_matchData_PoissonOOTPU_cfi.py).


## JERC

Text files with jet energy corrections `Fall15_25nsV2` copied from [here](https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECDataMC?rev=112#Jet_Energy_Corrections_in_Run2).
They are mostly used for testing.
