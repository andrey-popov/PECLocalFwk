#include <PECReaderConfig.hpp>

#include <stdexcept>


using namespace std;


PECReaderConfig::PECReaderConfig():
    readHardInteraction(false), readGenJets(false),
    syst()
{}


PECReaderConfig::PECReaderConfig(PECReaderConfig const &src):
    triggerSelection(src.triggerSelection->Clone()),
    eventSelection(src.eventSelection->Clone()),
    bTagger(src.bTagger),
    bTagReweighter((src.bTagReweighter) ? src.bTagReweighter->Clone() : nullptr),
    puReweighter((src.puReweighter) ? src.puReweighter->Clone() : nullptr),
    weightFilesLocation(src.weightFilesLocation),
    readHardInteraction(src.readHardInteraction),
    readGenJets(src.readGenJets),
    syst(src.syst)
{}


PECReaderConfig::PECReaderConfig(PECReaderConfig &&src):
    triggerSelection(move(src.triggerSelection)),
    eventSelection(move(src.eventSelection)),
    bTagger(move(src.bTagger)),
    bTagReweighter(move(src.bTagReweighter)),
    puReweighter(move(src.puReweighter)),
    weightFilesLocation(move(src.weightFilesLocation)),
    readHardInteraction(src.readHardInteraction),
    readGenJets(src.readGenJets),
    syst(src.syst)
{}


void PECReaderConfig::SetTriggerSelection(TriggerSelectionInterface *triggerSelection_)
{
    triggerSelection.reset(triggerSelection_->Clone());
}


void PECReaderConfig::SetModule(TriggerSelectionInterface *triggerSelection)
{
    SetTriggerSelection(triggerSelection);
}


void PECReaderConfig::SetEventSelection(EventSelectionInterface const *eventSelection_)
{
    eventSelection.reset(eventSelection_->Clone());
}


void PECReaderConfig::SetModule(EventSelectionInterface const *eventSelection)
{
    SetEventSelection(eventSelection);
}


void PECReaderConfig::SetBTagger(BTagger const *bTagger_)
{
    bTagger.reset(new BTagger(*bTagger_));
}


void PECReaderConfig::SetModule(BTagger const *bTagger_)
{
    SetBTagger(bTagger_);
}


void PECReaderConfig::SetBTagger(shared_ptr<BTagger const> &bTagger_)
{
    bTagger = bTagger_;
}


void PECReaderConfig::SetModule(shared_ptr<BTagger const> &bTagger_)
{
    SetBTagger(bTagger_);
}


void PECReaderConfig::SetBTagReweighter(WeightBTagInterface const *bTagReweighter_)
{
    bTagReweighter.reset(bTagReweighter_->Clone());
}


void PECReaderConfig::SetModule(WeightBTagInterface const *bTagReweighter_)
{
    SetBTagReweighter(bTagReweighter_);
}


void PECReaderConfig::SetPileUpReweighter(WeightPileUpInterface const *puReweighter_)
{
    puReweighter.reset(puReweighter_->Clone());
}


void PECReaderConfig::SetModule(WeightPileUpInterface const *puReweighter_)
{
    SetPileUpReweighter(puReweighter_);
}


void PECReaderConfig::SetWeightFilesLocation(string const &weightFilesLocation_)
{
    weightFilesLocation = weightFilesLocation_;
    
    // Make sure the last symbol is a slash
    if (weightFilesLocation.back() not_eq '/')
        weightFilesLocation += '/';
}


void PECReaderConfig::SetReadHardInteraction(bool readHardInteraction_)
{
    readHardInteraction = readHardInteraction_;
}


void PECReaderConfig::SetReadGenJets(bool readGenJets_)
{
    readGenJets = readGenJets_;
}


void PECReaderConfig::SetSystematics(SystVariation const &syst_)
{
    syst = syst_;
}


bool PECReaderConfig::IsSetTriggerSelection() const
{
    return bool(triggerSelection);
}


TriggerSelectionInterface *PECReaderConfig::GetTriggerSelection() const
{
    if (not triggerSelection)
        throw logic_error("PECReaderConfig::GetTriggerSelection: Accessing an undefined "
         "configuration parameter.");
    
    return triggerSelection.get();
}


bool PECReaderConfig::IsSetEventSelection() const
{
    return bool(eventSelection);
}


EventSelectionInterface const *PECReaderConfig::GetEventSelection() const
{
    if (not eventSelection)
        throw logic_error("PECReaderConfig::GetEventSelection: Accessing an undefined "
         "configuration parameter.");
    
    return eventSelection.get();
}


bool PECReaderConfig::IsSetBTagger() const
{
    return bool(bTagger);
}


BTagger const *PECReaderConfig::GetBTagger() const
{
    if (not bTagger)
        throw logic_error("PECReaderConfig::GetBTagger: Accessing an undefined "
         "configuration parameter.");
    
    return bTagger.get();
}


bool PECReaderConfig::IsSetBTagReweighter() const
{
    return bool(bTagReweighter);
}


WeightBTagInterface *PECReaderConfig::GetBTagReweighter() const
{
    if (not bTagReweighter)
        throw logic_error("PECReaderConfig::GetBTagReweighter: Accessing an undefined "
         "configuration parameter.");
    
    return bTagReweighter.get();
}


bool PECReaderConfig::IsSetPileUpReweighter() const
{
    return bool(puReweighter);
}


WeightPileUpInterface const *PECReaderConfig::GetPileUpReweighter() const
{
    if (not puReweighter)
        throw logic_error("PECReaderConfig::GetPileUpReweigher: Accessing an undefined "
         "configuration parameter.");
    
    return puReweighter.get();
}


WeightPileUpInterface *PECReaderConfig::GetPileUpReweighter()
{
    if (not puReweighter)
        throw logic_error("PECReaderConfig::GetPileUpReweigher: Accessing an undefined "
         "configuration parameter.");
    
    return puReweighter.get();
}


bool PECReaderConfig::IsSetWeightFilesLocation() const
{
    return (weightFilesLocation.length() not_eq 0);
}


string const &PECReaderConfig::GetWeightFilesLocation() const
{
    if (not IsSetWeightFilesLocation())
        throw logic_error("PECReaderConfig::GetWeightFilesLocation: Accessing an undefined "
         "configuration parameter.");
    
    return weightFilesLocation;
}


bool PECReaderConfig::GetReadHardInteraction() const
{
    return readHardInteraction;
}


bool PECReaderConfig::GetReadGenJets() const
{
    return readGenJets;
}


SystVariation const &PECReaderConfig::GetSystematics() const
{
    return syst;
}
