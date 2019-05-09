#include <mensura/PECReader/Candidate.hpp>
#include <mensura/PECReader/CandidateWithID.hpp>
#include <mensura/PECReader/Lepton.hpp>
#include <mensura/PECReader/Muon.hpp>
#include <mensura/PECReader/Electron.hpp>
#include <mensura/PECReader/Jet.hpp>
#include <mensura/PECReader/GenParticle.hpp>
#include <mensura/PECReader/GenJet.hpp>
#include <mensura/PECReader/ShowerParton.hpp>

#include <mensura/PECReader/EventID.hpp>
#include <mensura/PECReader/PileUpInfo.hpp>
#include <mensura/PECReader/GeneratorInfo.hpp>

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
