#include <mensura/extensions/GenWeightSyst.hpp>

#include <mensura/core/GeneratorReader.hpp>


GenWeightSyst::GenWeightSyst(std::string const name,
  std::initializer_list<std::pair<unsigned, unsigned>> const &systWeightsIndices_):
    EventWeightPlugin(name),
    generatorReaderName("Generator"), generatorReader(nullptr),
    referenceWeightIndex(0),
    systWeightsIndices(systWeightsIndices_)
{
    EventWeightPlugin::weights.resize(1 + 2 * systWeightsIndices.size(), 1.);
}


GenWeightSyst::GenWeightSyst(
  std::initializer_list<std::pair<unsigned, unsigned>> const &systWeightsIndices):
    GenWeightSyst("GenWeightSyst", systWeightsIndices)
{}


GenWeightSyst::GenWeightSyst(GenWeightSyst const &src):
    EventWeightPlugin(src),
    generatorReaderName(src.generatorReaderName),
    generatorReader(nullptr),
    referenceWeightIndex(src.referenceWeightIndex),
    systWeightsIndices(src.systWeightsIndices)
{}


void GenWeightSyst::BeginRun(Dataset const &)
{
    // Save pointer to plugin that provides generator-level weights
    generatorReader =
      dynamic_cast<GeneratorReader const *>(GetDependencyPlugin(generatorReaderName));
}


Plugin *GenWeightSyst::Clone() const
{
    return new GenWeightSyst(*this);
}


bool GenWeightSyst::ProcessEvent()
{
    // Compute systematic weights. Nominal weight is always 1.
    double const referenceWeight = generatorReader->GetAltWeight(referenceWeightIndex);
    
    for (unsigned iVar = 0; iVar < systWeightsIndices.size(); ++iVar)
    {
        auto const &indices = systWeightsIndices[iVar];
        weights.at(1 + 2 * iVar) = generatorReader->GetAltWeight(indices.first) / referenceWeight;
        weights.at(2 + 2 * iVar) = generatorReader->GetAltWeight(indices.second) / referenceWeight;
    }
    
    
    return true;
}
