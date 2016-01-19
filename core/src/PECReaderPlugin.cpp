#include <PECReaderPlugin.hpp>

#include <PECReader.hpp>

#include <stdexcept>


using namespace std;


PECReaderPlugin::PECReaderPlugin(unique_ptr<PECReaderConfig> &&config):
    ReaderPlugin("Reader"),
    reader(nullptr), readerConfig(move(config))
{}


PECReaderPlugin::PECReaderPlugin(PECReaderPlugin &&src):
    ReaderPlugin(src),
    reader(src.reader),
    readerConfig(move(src.readerConfig))
{
    // Prevent destructor of the source from deleting moved objects
    src.reader = nullptr;
}


PECReaderPlugin::~PECReaderPlugin()
{
    delete reader;
}


Plugin *PECReaderPlugin::Clone() const
{
    return new PECReaderPlugin(
     unique_ptr<PECReaderConfig>(new PECReaderConfig(*readerConfig.get())));
}


void PECReaderPlugin::BeginRun(Dataset const &dataset)
{
    // Update configuration parameters for the current dataset
    if (readerConfig->IsSetBTagReweighter())
        readerConfig->GetBTagReweighter()->LoadPayload(dataset);
    
    if (readerConfig->IsSetPileUpReweighter())
        readerConfig->GetPileUpReweighter()->SetDataset(dataset);
    
    
    // Create a new instance of PECReader
    reader = new PECReader(dataset, *readerConfig.get());
    
    
    // Open the first file in the dataset
    reader->NextSourceFile();
}


void PECReaderPlugin::EndRun()
{
    // Delete the current instance of PECReader
    delete reader;
    reader = nullptr;
}

PECReader const &PECReaderPlugin::operator*() const
{
    if (not reader)
        throw runtime_error("PECReaderPlugin::operator*: No valid PECReader object is associated "
         "to the plugin.");
    
    return *reader;
}


PECReader const *PECReaderPlugin::operator->() const
{
    return reader;
}


bool PECReaderPlugin::ProcessEvent()
{
    // Try reading the next event
    while (not reader->NextEvent())
    {
        // If there are no more event in the current file, try the next one
        if (not reader->NextSourceFile())
        {
            // There are no more files in the dataset, processing is terminated
            return false;
        }
        //^ Normally the dataset is expected to contain a single file, but the prise of being a
        //little bit more general is zero here
    }
    
    
    // If the control reaches this point, an event has been read successfully
    return true;
}
