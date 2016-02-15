#include <PECFwk/extensions/StdBTaggerPlugin.hpp>


using namespace std;


StdBTaggerPlugin::StdBTaggerPlugin(string const &name_, shared_ptr<BTagger const> const &bTagger_,
 BTagger::WorkingPoint workingPoint_)
 noexcept:
    AnalysisPlugin(name_),
    //^ Must call explicitly a constructor of a virtual base class
    BTaggerPlugin(name_),
    bTagger(bTagger_), workingPoint(workingPoint_)
{}


StdBTaggerPlugin::StdBTaggerPlugin(string const &name_, BTagger const &bTagger_,
 BTagger::WorkingPoint workingPoint_)
 noexcept:
    AnalysisPlugin(name_),
    //^ Must call explicitly a constructor of a virtual base class
    BTaggerPlugin(name_),
    bTagger(new BTagger(bTagger_)), workingPoint(workingPoint_)
{}


StdBTaggerPlugin::StdBTaggerPlugin(StdBTaggerPlugin const &src) noexcept:
    AnalysisPlugin(src.name),
    //^ Must call explicitly a constructor of a virtual base class
    BTaggerPlugin(src),
    bTagger(src.bTagger), workingPoint(src.workingPoint)
{}


StdBTaggerPlugin::~StdBTaggerPlugin() noexcept
{}


Plugin *StdBTaggerPlugin::Clone() const
{
    return new StdBTaggerPlugin(*this);
}


bool StdBTaggerPlugin::IsTagged(Jet const &jet) const
{
    return bTagger->IsTagged(workingPoint, jet);
}
