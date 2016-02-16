#include <PECFwk/PECReader/Candidate.hpp>
#include <PECFwk/PECReader/CandidateWithID.hpp>
#include <PECFwk/PECReader/Lepton.hpp>
#include <PECFwk/PECReader/Muon.hpp>
#include <PECFwk/PECReader/Electron.hpp>
#include <PECFwk/PECReader/Jet.hpp>
#include <PECFwk/PECReader/GenParticle.hpp>
#include <PECFwk/PECReader/GenJet.hpp>
#include <PECFwk/PECReader/ShowerParton.hpp>

#include <PECFwk/PECReader/EventID.hpp>
#include <PECFwk/PECReader/PileUpInfo.hpp>
#include <PECFwk/PECReader/GeneratorInfo.hpp>

#include <vector>


// Instantiate templates
template class std::vector<pec::Candidate>;
template class std::vector<pec::CandidateWithID>;
template class std::vector<pec::Lepton>;
template class std::vector<pec::Muon>;
template class std::vector<pec::Electron>;
template class std::vector<pec::Jet>;
template class std::vector<pec::GenParticle>;
template class std::vector<pec::GenJet>;
template class std::vector<pec::ShowerParton>;
