#include <BTagDatabase.hpp>

#include <ROOTLock.hpp>
#include <FileInPath.hpp>

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
    FileInPath pathResolver;
    
    effFile.reset(new TFile((pathResolver.Resolve("BTag/", string("eff_") + bTagger.GetTextCode() +
     ".root")).c_str()));
    
    
    // The code below was generated automatically
    BTagger::Algorithm const algo = bTagger.GetAlgorithm();
    BTagger::WorkingPoint const wp = bTagger.GetWorkingPoint();
    
    if (algo == BTagger::Algorithm::TCHP and wp == BTagger::WorkingPoint::Tight)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagTCHPT;
        tagUncertainties.assign({0.0624031, 0.034023, 0.0362764, 0.0341996, 0.031248, 0.0281222, 0.0316684, 0.0276272, 0.0208828, 0.0223511, 0.0224121, 0.0261939, 0.0268247, 0.0421413, 0.0532897, 0.0506714});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagTCHPTMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagTCHPTMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagTCHPTMax;
        mistagOuterRegion = 2.4;
    }
    else if (algo == BTagger::Algorithm::CSV and wp == BTagger::WorkingPoint::Loose)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVL;
        tagUncertainties.assign({0.033299, 0.0146768, 0.013803, 0.0170145, 0.0166976, 0.0137879, 0.0149072, 0.0153068, 0.0133077, 0.0123737, 0.0157152, 0.0175161, 0.0209241, 0.0278605, 0.0346928, 0.0350099});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVLMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVLMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVLMax;
        mistagOuterRegion = 1.5;
    }
    else if (algo == BTagger::Algorithm::CSV and wp == BTagger::WorkingPoint::Medium)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVM;
        tagUncertainties.assign({0.0415707, 0.0204209, 0.0223227, 0.0206655, 0.0199325, 0.0174121, 0.0202332, 0.0182446, 0.0159777, 0.0218531, 0.0204688, 0.0265191, 0.0313175, 0.0415417, 0.0740446, 0.0596716});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVMMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVMMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVMMax;
        mistagOuterRegion = 1.6;
    }
    else if (algo == BTagger::Algorithm::CSV and wp == BTagger::WorkingPoint::Tight)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVT;
        tagUncertainties.assign({0.0515703, 0.0264008, 0.0272757, 0.0275565, 0.0248745, 0.0218456, 0.0253845, 0.0239588, 0.0271791, 0.0273912, 0.0379822, 0.0411624, 0.0786307, 0.0866832, 0.0942053, 0.102403});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVTMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVTMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVTMax;
        mistagOuterRegion = 2.4;
    }
    else if (algo == BTagger::Algorithm::CSVV1 and wp == BTagger::WorkingPoint::Loose)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVV1L;
        tagUncertainties.assign({0.0345802, 0.0152688, 0.0149101, 0.0167145, 0.0167098, 0.013472, 0.0146024, 0.0156735, 0.0142592, 0.0147227, 0.0167101, 0.0191159, 0.0360389, 0.0331342, 0.0336916, 0.0298064});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVV1LMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVV1LMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVV1LMax;
        mistagOuterRegion = 1.5;
    }
    else if (algo == BTagger::Algorithm::CSVV1 and wp == BTagger::WorkingPoint::Medium)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVV1M;
        tagUncertainties.assign({0.0376303, 0.0187774, 0.019884, 0.0215849, 0.0207925, 0.0180289, 0.0178674, 0.0159339, 0.019042, 0.020975, 0.0189178, 0.0246477, 0.0291784, 0.0428437, 0.0674624, 0.0479834});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVV1MMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVV1MMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVV1MMax;
        mistagOuterRegion = 1.6;
    }
    else if (algo == BTagger::Algorithm::CSVV1 and wp == BTagger::WorkingPoint::Tight)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVV1T;
        tagUncertainties.assign({0.0564014, 0.0293159, 0.0315288, 0.0301526, 0.0266047, 0.0240973, 0.0254404, 0.0241548, 0.0233434, 0.0303961, 0.040912, 0.042942, 0.0440911, 0.0555312, 0.105762, 0.0886457});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVV1TMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVV1TMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVV1TMax;
        mistagOuterRegion = 2.4;
    }
    else if (algo == BTagger::Algorithm::CSVSLV1 and wp == BTagger::WorkingPoint::Loose)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVSLV1L;
        tagUncertainties.assign({0.135344, 0.0288656, 0.0259088, 0.0199242, 0.0189792, 0.0178341, 0.0187104, 0.0239028, 0.0211104, 0.017689, 0.02823, 0.0259654, 0.0614497});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVSLV1LMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVSLV1LMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVSLV1LMax;
        mistagOuterRegion = 1.5;
    }
    else if (algo == BTagger::Algorithm::CSVSLV1 and wp == BTagger::WorkingPoint::Medium)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVSLV1M;
        tagUncertainties.assign({0.0918443, 0.0282557, 0.0264246, 0.0242536, 0.0218046, 0.0207568, 0.0207962, 0.0208919, 0.0200894, 0.0258879, 0.0270699, 0.0256006, 0.0438219});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVSLV1MMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVSLV1MMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVSLV1MMax;
        mistagOuterRegion = 1.6;
    }
    else if (algo == BTagger::Algorithm::CSVSLV1 and wp == BTagger::WorkingPoint::Tight)
    {
        tagScaleFactorMethod = &BTagDatabase::GetSFTagCSVSLV1T;
        tagUncertainties.assign({0.10761, 0.0333696, 0.0339123, 0.0302699, 0.0261626, 0.0274243, 0.0224287, 0.0239842, 0.0267866, 0.0254787, 0.0317589, 0.0365968, 0.0481259});
        mistagScaleFactorMethodMean = &BTagDatabase::GetSFMistagCSVSLV1TMean;
        mistagScaleFactorMethodMin = &BTagDatabase::GetSFMistagCSVSLV1TMin;
        mistagScaleFactorMethodMax = &BTagDatabase::GetSFMistagCSVSLV1TMax;
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
double BTagDatabase::GetSFTagTCHPT(double pt) const
{
    return 0.703389*((1.+(0.088358*pt))/(1.+(0.0660291*pt)));
}

double BTagDatabase::GetSFTagCSVL(double pt) const
{
    return 0.997942*((1.+(0.00923753*pt))/(1.+(0.0096119*pt)));
}

double BTagDatabase::GetSFTagCSVM(double pt) const
{
    return (0.938887+(0.00017124*pt))+(-2.76366e-07*(pt*pt));
}

double BTagDatabase::GetSFTagCSVT(double pt) const
{
    return (0.927563+(1.55479e-05*pt))+(-1.90666e-07*(pt*pt));
}

double BTagDatabase::GetSFTagCSVV1L(double pt) const
{
    return 1.7586*((1.+(0.799078*pt))/(1.+(1.44245*pt)));
}

double BTagDatabase::GetSFTagCSVV1M(double pt) const
{
    return 0.952067+(-2.00037e-05*pt);
}

double BTagDatabase::GetSFTagCSVV1T(double pt) const
{
    return (0.912578+(0.000115164*pt))+(-2.24429e-07*(pt*pt));
}

double BTagDatabase::GetSFTagCSVSLV1L(double pt) const
{
    return 0.970168*((1.+(0.00266812*pt))/(1.+(0.00250852*pt)));
}

double BTagDatabase::GetSFTagCSVSLV1M(double pt) const
{
    return ((0.939238+(0.000278928*pt))+(-7.49693e-07*(pt*pt)))+(2.04822e-10*(pt*(pt*pt)));
}

double BTagDatabase::GetSFTagCSVSLV1T(double pt) const
{
    return (0.928257+(9.3526e-05*pt))+(-4.1568e-07*(pt*pt));
}

double BTagDatabase::GetSFMistagTCHPTMean(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.20175+(0.000858187*pt))+(-1.98726e-06*(pt*pt)))+(1.31057e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagTCHPTMin(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((0.968557+(0.000586877*pt))+(-1.34624e-06*(pt*pt)))+(9.09724e-10*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagTCHPTMax(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.43508+(0.00112666*pt))+(-2.62078e-06*(pt*pt)))+(1.70697e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVLMean(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.01177+(0.0023066*pt))+(-4.56052e-06*(pt*pt)))+(2.57917e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((0.975966+(0.00196354*pt))+(-3.83768e-06*(pt*pt)))+(2.17466e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.93821+(0.00180935*pt))+(-3.86937e-06*(pt*pt)))+(2.43222e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.00022+(0.0010998*pt))+(-3.10672e-06*(pt*pt)))+(2.35006e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVLMin(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((0.977761+(0.00170704*pt))+(-3.2197e-06*(pt*pt)))+(1.78139e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((0.945135+(0.00146006*pt))+(-2.70048e-06*(pt*pt)))+(1.4883e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.911657+(0.00142008*pt))+(-2.87569e-06*(pt*pt)))+(1.76619e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.970045+(0.000862284*pt))+(-2.31714e-06*(pt*pt)))+(1.68866e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVLMax(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.04582+(0.00290226*pt))+(-5.89124e-06*(pt*pt)))+(3.37128e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((1.00683+(0.00246404*pt))+(-4.96729e-06*(pt*pt)))+(2.85697e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.964787+(0.00219574*pt))+(-4.85552e-06*(pt*pt)))+(3.09457e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.03039+(0.0013358*pt))+(-3.89284e-06*(pt*pt)))+(3.01155e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVMMean(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((1.07541+(0.00231827*pt))+(-4.74249e-06*(pt*pt)))+(2.70862e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.05613+(0.00114031*pt))+(-2.56066e-06*(pt*pt)))+(1.67792e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.05625+(0.000487231*pt))+(-2.22792e-06*(pt*pt)))+(1.70262e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVMMin(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((0.964527+(0.00149055*pt))+(-2.78338e-06*(pt*pt)))+(1.51771e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((0.946051+(0.000759584*pt))+(-1.52491e-06*(pt*pt)))+(9.65822e-10*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.956736+(0.000280197*pt))+(-1.42739e-06*(pt*pt)))+(1.0085e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVMMax(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((1.18638+(0.00314148*pt))+(-6.68993e-06*(pt*pt)))+(3.89288e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.16624+(0.00151884*pt))+(-3.59041e-06*(pt*pt)))+(2.38681e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.15575+(0.000693344*pt))+(-3.02661e-06*(pt*pt)))+(2.39752e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVTMean(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.00462+(0.00325971*pt))+(-7.79184e-06*(pt*pt)))+(5.22506e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVTMin(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((0.845757+(0.00186422*pt))+(-4.6133e-06*(pt*pt)))+(3.21723e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVTMax(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.16361+(0.00464695*pt))+(-1.09467e-05*(pt*pt)))+(7.21896e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVV1LMean(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.03599+(0.00187708*pt))+(-3.73001e-06*(pt*pt)))+(2.09649e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((0.987393+(0.00162718*pt))+(-3.21869e-06*(pt*pt)))+(1.84615e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.950146+(0.00150932*pt))+(-3.28136e-06*(pt*pt)))+(2.06196e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.01923+(0.000898874*pt))+(-2.57986e-06*(pt*pt)))+(1.8149e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVV1LMin(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((0.995735+(0.00146811*pt))+(-2.83906e-06*(pt*pt)))+(1.5717e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((0.947416+(0.00130297*pt))+(-2.50427e-06*(pt*pt)))+(1.41682e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.91407+(0.00123525*pt))+(-2.61966e-06*(pt*pt)))+(1.63016e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.979782+(0.000743807*pt))+(-2.14927e-06*(pt*pt)))+(1.49486e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVV1LMax(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.0763+(0.00228243*pt))+(-4.61169e-06*(pt*pt)))+(2.61601e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((1.02741+(0.00194855*pt))+(-3.92587e-06*(pt*pt)))+(2.27149e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.986259+(0.00178067*pt))+(-3.93596e-06*(pt*pt)))+(2.49014e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.05868+(0.00105264*pt))+(-3.00767e-06*(pt*pt)))+(2.13498e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVV1MMean(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((1.06383+(0.00279657*pt))+(-5.75405e-06*(pt*pt)))+(3.4302e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.03709+(0.00169762*pt))+(-3.52511e-06*(pt*pt)))+(2.25975e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.01679+(0.00211998*pt))+(-6.26097e-06*(pt*pt)))+(4.53843e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVV1MMin(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((0.971686+(0.00195242*pt))+(-3.98756e-06*(pt*pt)))+(2.38991e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((0.947328+(0.00117422*pt))+(-2.32363e-06*(pt*pt)))+(1.46136e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.922527+(0.00176245*pt))+(-5.14169e-06*(pt*pt)))+(3.61532e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVV1MMax(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((1.15605+(0.00363538*pt))+(-7.50634e-06*(pt*pt)))+(4.4624e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.12687+(0.00221834*pt))+(-4.71949e-06*(pt*pt)))+(3.05456e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.11102+(0.00247531*pt))+(-7.37745e-06*(pt*pt)))+(5.46589e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVV1TMean(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.15047+(0.00220948*pt))+(-5.17912e-06*(pt*pt)))+(3.39216e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVV1TMin(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((0.936862+(0.00149618*pt))+(-3.64924e-06*(pt*pt)))+(2.43883e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVV1TMax(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.36418+(0.00291794*pt))+(-6.6956e-06*(pt*pt)))+(4.33793e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVSLV1LMean(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.06344+(0.0014539*pt))+(-2.72328e-06*(pt*pt)))+(1.47643e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((1.0123+(0.00151734*pt))+(-2.99087e-06*(pt*pt)))+(1.73428e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.975277+(0.00146932*pt))+(-3.17563e-06*(pt*pt)))+(2.03698e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.04201+(0.000827388*pt))+(-2.31261e-06*(pt*pt)))+(1.62629e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVSLV1LMin(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.01168+(0.000950951*pt))+(-1.58947e-06*(pt*pt)))+(7.96543e-10*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((0.960377+(0.00109821*pt))+(-2.01652e-06*(pt*pt)))+(1.13076e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((0.931687+(0.00110971*pt))+(-2.29681e-06*(pt*pt)))+(1.45867e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.992838+(0.000660673*pt))+(-1.84971e-06*(pt*pt)))+(1.2758e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVSLV1LMax(double pt, double absEta) const
{
    if (absEta < 0.5)
        return ((1.11523+(0.00195443*pt))+(-3.85115e-06*(pt*pt)))+(2.15307e-09*(pt*(pt*pt)));
    else if (absEta < 1.0)
        return ((1.06426+(0.0019339*pt))+(-3.95863e-06*(pt*pt)))+(2.3342e-09*(pt*(pt*pt)));
    else if (absEta < 1.5)
        return ((1.0189+(0.00182641*pt))+(-4.04782e-06*(pt*pt)))+(2.61199e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.09118+(0.000992959*pt))+(-2.77313e-06*(pt*pt)))+(1.9769e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVSLV1MMean(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((1.06212+(0.00223614*pt))+(-4.25167e-06*(pt*pt)))+(2.42728e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.04547+(0.00216995*pt))+(-4.579e-06*(pt*pt)))+(2.91791e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.991865+(0.00324957*pt))+(-9.65897e-06*(pt*pt)))+(7.13694e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVSLV1MMin(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((0.903956+(0.00121678*pt))+(-2.04383e-06*(pt*pt)))+(1.10727e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((0.900637+(0.00120088*pt))+(-2.27069e-06*(pt*pt)))+(1.40609e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.868875+(0.00222761*pt))+(-6.44897e-06*(pt*pt)))+(4.53261e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVSLV1MMax(double pt, double absEta) const
{
    if (absEta < 0.8)
        return ((1.22035+(0.00325183*pt))+(-6.45023e-06*(pt*pt)))+(3.74225e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.19034+(0.00313562*pt))+(-6.87854e-06*(pt*pt)))+(4.42546e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.11481+(0.00426745*pt))+(-1.28612e-05*(pt*pt)))+(9.74425e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVSLV1TMean(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.09494+(0.00193966*pt))+(-4.35021e-06*(pt*pt)))+(2.8973e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVSLV1TMin(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((0.813331+(0.00139561*pt))+(-3.15313e-06*(pt*pt)))+(2.12173e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}

double BTagDatabase::GetSFMistagCSVSLV1TMax(double pt, double absEta) const
{
    if (absEta < 2.4)
        return ((1.37663+(0.00247963*pt))+(-5.53583e-06*(pt*pt)))+(3.66635e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Eta out of range.");
        return -100.;
    }
}
// End of automatically generated code
