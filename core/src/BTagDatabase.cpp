#include <BTagDatabase.hpp>

#include <ROOTLock.hpp>

#include <stdexcept>
#include <cstdlib>


using namespace std;


BTagDatabase::BTagDatabase(BTagger const &bTagger,
 BJetPayload bJetPayload_ /*= BJetPayload::MuJetTTBar*/):
    algo(bTagger.GetAlgorithm()), workingPoint(bTagger.GetWorkingPoint()),
    bJetPayload(bJetPayload_),
    histEffB(nullptr), histEffC(nullptr), histEffL(nullptr), histEffG(nullptr)
{
    if (bJetPayload != BJetPayload::MuJetTTBar)
        throw runtime_error("BTagDatabase::BTagDatabase: Only MuJetsTTBar payload is supported at "
         "the moment.");
    
    
    // Open file with b-tagging efficiencies
    char const *installPath = getenv("PEC_FWK_INSTALL");
    
    if (not installPath)
        throw runtime_error("BTagDatabase::BTagDatabase: Mandatory environment variable "
         "PEC_FWK_INSTALL is not defined.");
    
    effFile.reset(new TFile((string(installPath) + "/data/BTag/eff_" + bTagger.GetTextCode() +
     ".root").c_str()));
    
    
    // The code below was generated automatically
    BTagger::Algorithm const algo = bTagger.GetAlgorithm();
    BTagger::WorkingPoint const wp = bTagger.GetWorkingPoint();

    if (algo == BTagger::Algorithm::CSV and wp == BTagger::WorkingPoint::Loose)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVL;
        tagUncertainties.assign({0.0484285, 0.0126178, 0.0120027, 0.0141137, 0.0145441, 0.0131145, 0.0168479, 0.0160836, 0.0126209, 0.0136017, 0.019182, 0.0198805, 0.0386531, 0.0392831, 0.0481008, 0.0474291});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVLMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVLMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVLMax;
        mistagOuterRegion = 1.5;
    }
    else if (algo == BTagger::Algorithm::CSV and wp == BTagger::WorkingPoint::Medium)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVM;
        tagUncertainties.assign({0.0554504, 0.0209663, 0.0207019, 0.0230073, 0.0208719, 0.0200453, 0.0264232, 0.0240102, 0.0229375, 0.0184615, 0.0216242, 0.0248119, 0.0465748, 0.0474666, 0.0718173, 0.0717567});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVMMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVMMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVMMax;
        mistagOuterRegion = 1.6;
    }
    else if (algo == BTagger::Algorithm::CSV and wp == BTagger::WorkingPoint::Tight)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVT;
        tagUncertainties.assign({0.0567059, 0.0266907, 0.0263491, 0.0342831, 0.0303327, 0.024608, 0.0333786, 0.0317642, 0.031102, 0.0295603, 0.0474663, 0.0503182, 0.0580424, 0.0575776, 0.0769779, 0.0898199});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVTMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVTMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVTMax;
        mistagOuterRegion = 2.4;
    }
    else if (algo == BTagger::Algorithm::TCHP and wp == BTagger::WorkingPoint::Tight)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagTCHPT;
        tagUncertainties.assign({0.0725549, 0.0275189, 0.0279695, 0.028065, 0.0270752, 0.0254934, 0.0262087, 0.0230919, 0.0294829, 0.0226487, 0.0272755, 0.0303747, 0.051223, 0.0542895, 0.0589887, 0.0584216});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagTCHPTMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagTCHPTMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagTCHPTMax;
        mistagOuterRegion = 2.4;
    }
    else if (algo == BTagger::Algorithm::JP and wp == BTagger::WorkingPoint::Loose)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagJPL;
        tagUncertainties.assign({0.0456879, 0.0229755, 0.0229115, 0.0219184, 0.0222935, 0.0189195, 0.0237255, 0.0236069, 0.0159177, 0.0196792, 0.0168556, 0.0168882, 0.0348084, 0.0355933, 0.0476836, 0.0500367});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagJPLMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagJPLMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagJPLMax;
        mistagOuterRegion = 1.5;
    }
    else if (algo == BTagger::Algorithm::JP and wp == BTagger::WorkingPoint::Medium)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagJPM;
        tagUncertainties.assign({0.0584144, 0.0304763, 0.0311788, 0.0339226, 0.0343223, 0.0303401, 0.0329372, 0.0339472, 0.0368516, 0.0319189, 0.0354756, 0.0347098, 0.0408868, 0.0415471, 0.0567743, 0.0605397});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagJPMMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagJPMMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagJPMMax;
        mistagOuterRegion = 1.6;
    }
    else if (algo == BTagger::Algorithm::JP and wp == BTagger::WorkingPoint::Tight)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagJPT;
        tagUncertainties.assign({0.0673183, 0.0368276, 0.037958, 0.0418136, 0.0463115, 0.0409334, 0.0436405, 0.0419725, 0.0451182, 0.0394386, 0.0423327, 0.0393015, 0.0499883, 0.0509444, 0.0780023, 0.0856582});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagJPTMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagJPTMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagJPTMax;
        mistagOuterRegion = 2.4;
    }
    // End of automatically generated code
    else
        throw runtime_error("BTagDatabase::BTagDatabase: Requested combination of b-tagging algorithm "
         "and working point is not supported.");
}


BTagDatabase::BTagDatabase(BTagDatabase const &src):
    algo(src.algo), workingPoint(src.workingPoint), bJetPayload(src.bJetPayload),
    tagScaleFactorMethod(src.tagScaleFactorMethod),
    tagUncertainties(src.tagUncertainties),
    mistagScaleFactorMethodMean(src.mistagScaleFactorMethodMean),
    mistagScaleFactorMethodMin(src.mistagScaleFactorMethodMin),
    mistagScaleFactorMethodMax(src.mistagScaleFactorMethodMax),
    mistagOuterRegion(src.mistagOuterRegion),
    effFile(src.effFile),
    histEffB(nullptr), histEffC(nullptr), histEffL(nullptr), histEffG(nullptr)
{}


BTagDatabase::~BTagDatabase()
{
    delete histEffB;
    delete histEffC;
    delete histEffG;
    delete histEffL;
}


void BTagDatabase::SetDataset(Dataset const &)
{
    SetEfficiencies();
}


double BTagDatabase::GetEfficiency(Jet const &jet) const
{
    // Check the pseudorapidity range
    if (fabs(jet.Eta()) >= 2.4)
        return 0.;
    
    
    // Choose the appropriate histogram
    TH2 *hist = nullptr;
    
    switch (abs(jet.GetParentID()))
    {
        case 5:
            hist = histEffB;
            break;
        
        case 4:
            hist = histEffC;
            break;
        
        case 21:
            hist = histEffG;
            break;
        
        default:
            hist = histEffL;
            break;
    }
    
    return hist->GetBinContent(hist->FindFixBin(min(399., jet.Pt()), jet.Eta()));
    //^ Here is a dirty hack for the overflow bin}
}


double BTagDatabase::GetScaleFactor(Jet const &jet, SFVar var /*= SFVar::Central*/) const
{
    // Check the pseudorapidity range
    if (fabs(jet.Eta()) >= 2.4)
        return 0.;
    
    
    // Switch over the jet flavour
    unsigned const absFlavour = abs(jet.GetParentID());
    
    if (absFlavour == 4 or absFlavour == 5)  // b- or c-jets
    {
        // Constrain the momentum to the range
        double pt = jet.Pt();
        double uncFactor = 1.;  // a factor to increase the uncertainty
        
        if (pt < 20.)
        {
            pt = 20.;
            uncFactor = 2.;
        }
        else if (pt > 800)
        {
            pt = 800;
            uncFactor = 2.;
        }
        
        
        // Get the central scale factor with the booked method
        double const centralSF = (this->*tagScaleFactorMethod)(pt);
        
        if (var == SFVar::Central)
            return centralSF;
        
        
        // If this point is reached, a variated scale factor has been requested
        // Find the corresponding pt bin (the boundaries are the same for all the cases)
        static double const ptMax[16] = {30., 40., 50., 60., 70., 80., 100., 120., 160., 210., 260.,
         320., 400., 500., 600., 800.};
        unsigned bin = 0;
        
        while (bin < 15 and ptMax[bin] < pt)
            ++bin;
        
        
        // Double the uncertainty for a c-jet
        if (absFlavour == 4)
            uncFactor *= 2;
        
        
        // Calculate the final result
        if (var == SFVar::Up)
            return centralSF + uncFactor * tagUncertainties[bin];
        else  // i.e. down variation
            return centralSF - uncFactor * tagUncertainties[bin];
    }
    else  // light-flavour or gluon jets
    {
        double pt = jet.Pt();
        double const absEta = fabs(jet.Eta());
        
        if (pt > 800. or (pt > 700. and absEta > mistagOuterRegion))
        //^ The momentum is outside supported range; uncertainty should be doubled
        {
            // Force the momentum to the range
            if (absEta > mistagOuterRegion)
                pt = 700.;
            else
                pt = 800.;
            
            
            // Get the central scale factor
            double const centralSF = (this->*mistagScaleFactorMethodMean)(pt, absEta);
            
            // Calculate and return the scale factor
            switch (var)
            {
                case SFVar::Central:
                    return centralSF;
                
                case SFVar::Up:
                    return 2 * (this->*mistagScaleFactorMethodMax)(pt, absEta) + centralSF;
                
                case SFVar::Down:
                    return 2 * (this->*mistagScaleFactorMethodMin)(pt, absEta) - centralSF;
            }
        }
        else
        //^ The momentum is in the allowed range
        {
            // Just return the scale factor
            switch (var)
            {
                case SFVar::Central:
                    return (this->*mistagScaleFactorMethodMean)(pt, absEta);
            
                case SFVar::Up:
                    return (this->*mistagScaleFactorMethodMax)(pt, absEta);
            
                case SFVar::Down:
                    return (this->*mistagScaleFactorMethodMin)(pt, absEta);
            }
        }
    }
    
    throw runtime_error("BTagDatabase::GetScaleFactor: Control is not supposed to reach this point.");
    return -100.;
}


void BTagDatabase::SetEfficiencies()
{
    // Reset of histograms is thread-unsafe and must be protected
    ROOTLock::Lock();
    
    // Delete the previous set of efficiencies
    delete histEffB;
    delete histEffC;
    delete histEffL;
    delete histEffG;
    
    
    // Read new efficiencies for the current process (for the time being they are the same)
    histEffB = dynamic_cast<TH2 *>(effFile->Get("hist_eff_b"));
    histEffC = dynamic_cast<TH2 *>(effFile->Get("hist_eff_c"));
    histEffL = dynamic_cast<TH2 *>(effFile->Get("hist_eff_uds"));
    histEffG = dynamic_cast<TH2 *>(effFile->Get("hist_eff_g"));
    
    // Make sure the histograms will not be deleted when some random ROOT file is closed
    histEffB->SetDirectory(nullptr);
    histEffC->SetDirectory(nullptr);
    histEffG->SetDirectory(nullptr);
    histEffL->SetDirectory(nullptr);
    
    // Release the ROOT lock
    ROOTLock::Unlock();
}


// The code below was generated automatically
double BTagDatabase::GetSFTagCSVL(double pt) const
{
    return 0.981149*((1.+(-0.000713295*pt))/(1.+(-0.000703264*pt)));
}

double BTagDatabase::GetSFTagCSVM(double pt) const
{
    return 0.726981*((1.+(0.253238*pt))/(1.+(0.188389*pt)));
}

double BTagDatabase::GetSFTagCSVT(double pt) const
{
    return 0.869965*((1.+(0.0335062*pt))/(1.+(0.0304598*pt)));
}

double BTagDatabase::GetSFTagTCHPT(double pt) const
{
    return 0.305208*((1.+(0.595166*pt))/(1.+(0.186968*pt)));
}

double BTagDatabase::GetSFTagJPL(double pt) const
{
    return 0.977721*((1.+(-1.02685e-06*pt))/(1.+(-2.56586e-07*pt)));
}

double BTagDatabase::GetSFTagJPM(double pt) const
{
    return 0.87887*((1.+(0.0393348*pt))/(1.+(0.0354499*pt)));
}

double BTagDatabase::GetSFTagJPT(double pt) const
{
    return 0.802097*((1.+(0.013219*pt))/(1.+(0.0107842*pt)));
}

double BTagDatabase::GetSFMistagCSVLMean(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.04901+(0.00152181*pt))+(-3.43568e-06*(pt*pt)))+(2.17219e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((0.991915+(0.00172552*pt))+(-3.92652e-06*(pt*pt)))+(2.56816e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.962127+(0.00192796*pt))+(-4.53385e-06*(pt*pt)))+(3.0605e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.06121+(0.000332747*pt))+(-8.81201e-07*(pt*pt)))+(7.43896e-10*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVLMin(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((0.973773+(0.00103049*pt))+(-2.2277e-06*(pt*pt)))+(1.37208e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((0.921518+(0.00129098*pt))+(-2.86488e-06*(pt*pt)))+(1.86022e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.895419+(0.00153387*pt))+(-3.48409e-06*(pt*pt)))+(2.30899e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.983607+(0.000196747*pt))+(-3.98327e-07*(pt*pt)))+(2.95764e-10*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVLMax(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.12424+(0.00201136*pt))+(-4.64021e-06*(pt*pt)))+(2.97219e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((1.06231+(0.00215815*pt))+(-4.9844e-06*(pt*pt)))+(3.27623e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((1.02883+(0.00231985*pt))+(-5.57924e-06*(pt*pt)))+(3.81235e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.1388+(0.000468418*pt))+(-1.36341e-06*(pt*pt)))+(1.19256e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVMMean(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((1.06238+(0.00198635*pt))+(-4.89082e-06*(pt*pt)))+(3.29312e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.08048+(0.00110831*pt))+(-2.96189e-06*(pt*pt)))+(2.16266e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.09145+(0.000687171*pt))+(-2.45054e-06*(pt*pt)))+(1.7844e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVMMin(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((0.972746+(0.00104424*pt))+(-2.36081e-06*(pt*pt)))+(1.53438e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((0.9836+(0.000649761*pt))+(-1.59773e-06*(pt*pt)))+(1.14324e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.00616+(0.000358884*pt))+(-1.23768e-06*(pt*pt)))+(6.86678e-10*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVMMax(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((1.15201+(0.00292575*pt))+(-7.41497e-06*(pt*pt)))+(5.0512e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.17735+(0.00156533*pt))+(-4.32257e-06*(pt*pt)))+(3.18197e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.17671+(0.0010147*pt))+(-3.66269e-06*(pt*pt)))+(2.88425e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVTMean(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.01739+(0.00283619*pt))+(-7.93013e-06*(pt*pt)))+(5.97491e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVTMin(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((0.953587+(0.00124872*pt))+(-3.97277e-06*(pt*pt)))+(3.23466e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVTMax(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.08119+(0.00441909*pt))+(-1.18764e-05*(pt*pt)))+(8.71372e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagTCHPTMean(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.1676+(0.00136673*pt))+(-3.51053e-06*(pt*pt)))+(2.4966e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagTCHPTMin(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((0.988346+(0.000914722*pt))+(-2.37077e-06*(pt*pt)))+(1.72082e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagTCHPTMax(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.34691+(0.00181637*pt))+(-4.64484e-06*(pt*pt)))+(3.27122e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagJPLMean(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.05617+(0.000986016*pt))+(-2.05398e-06*(pt*pt)))+(1.25408e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((1.02884+(0.000471854*pt))+(-1.15441e-06*(pt*pt)))+(7.83716e-10*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((1.02463+(0.000907924*pt))+(-2.07133e-06*(pt*pt)))+(1.37083e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.05387+(0.000951237*pt))+(-2.35437e-06*(pt*pt)))+(1.66123e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagJPLMin(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((0.918762+(0.000749113*pt))+(-1.48511e-06*(pt*pt)))+(8.78559e-10*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((0.893017+(0.000369124*pt))+(-8.68577e-07*(pt*pt)))+(5.79006e-10*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.89415+(0.000712877*pt))+(-1.57703e-06*(pt*pt)))+(1.02034e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.918611+(0.000781707*pt))+(-1.8923e-06*(pt*pt)))+(1.312e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagJPLMax(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.19358+(0.00122182*pt))+(-2.62078e-06*(pt*pt)))+(1.62951e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((1.16466+(0.000573985*pt))+(-1.43899e-06*(pt*pt)))+(9.88387e-10*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((1.15511+(0.00110197*pt))+(-2.56374e-06*(pt*pt)))+(1.72152e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.1891+(0.00112006*pt))+(-2.81586e-06*(pt*pt)))+(2.01249e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagJPMMean(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((0.980407+(0.00190765*pt))+(-4.49633e-06*(pt*pt)))+(3.02664e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.01783+(0.00183763*pt))+(-4.64972e-06*(pt*pt)))+(3.34342e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.866685+(0.00396887*pt))+(-1.11342e-05*(pt*pt)))+(8.84085e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagJPMMin(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((0.813164+(0.00127951*pt))+(-2.74274e-06*(pt*pt)))+(1.78799e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((0.860873+(0.00110031*pt))+(-2.48023e-06*(pt*pt)))+(1.73776e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.740983+(0.00302736*pt))+(-8.12284e-06*(pt*pt)))+(6.281e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagJPMMax(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((1.14766+(0.00253327*pt))+(-6.24447e-06*(pt*pt)))+(4.26468e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.17479+(0.00257252*pt))+(-6.81377e-06*(pt*pt)))+(4.94891e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.992297+(0.00490671*pt))+(-1.41403e-05*(pt*pt)))+(1.14097e-08*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagJPTMean(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((0.89627+(0.00328988*pt))+(-8.76392e-06*(pt*pt)))+(6.4662e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagJPTMin(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((0.666092+(0.00262465*pt))+(-6.5345e-06*(pt*pt)))+(4.73926e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagJPTMax(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.12648+(0.00394995*pt))+(-1.0981e-05*(pt*pt)))+(8.19134e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}
// End of automatically generated code