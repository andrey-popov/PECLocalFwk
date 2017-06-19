#include <mensura/extensions/DatasetSelector.hpp>

#include <mensura/core/Dataset.hpp>


DatasetSelector::DatasetSelector(std::string const &name,
  std::initializer_list<std::string> const &masks, bool inverse /*= false*/):
    AnalysisPlugin(name),
    inversedLogic(inverse), processCurDataset(true)
{
    datasetMasks.reserve(masks.size());
    
    for (auto const &mask: masks)
        datasetMasks.emplace_back(std::regex(mask));
}


DatasetSelector::DatasetSelector(std::initializer_list<std::string> const &masks,
  bool inverse /*= false*/):
    DatasetSelector("DatasetSelector", masks, inverse)
{}


void DatasetSelector::BeginRun(Dataset const &dataset)
{
    std::string const &datasetID = dataset.GetSourceDatasetID();
    bool matchFound = false;
    
    for (auto const &mask: datasetMasks)
    {
        if (std::regex_match(datasetID, mask))
        {
            matchFound = true;
            break;
        }
    }
    
    
    processCurDataset = (matchFound != inversedLogic);
}


Plugin *DatasetSelector::Clone() const
{
    return new DatasetSelector(*this);
}


bool DatasetSelector::ProcessEvent()
{
    return processCurDataset;
}
