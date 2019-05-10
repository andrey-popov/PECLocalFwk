#include "Candidate.hpp"
#include "CandidateWithID.hpp"
#include "Lepton.hpp"
#include "Muon.hpp"
#include "Electron.hpp"
#include "Jet.hpp"
#include "GenParticle.hpp"
#include "GenJet.hpp"
#include "ShowerParton.hpp"

#include "EventID.hpp"
#include "PileUpInfo.hpp"
#include "GeneratorInfo.hpp"

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
