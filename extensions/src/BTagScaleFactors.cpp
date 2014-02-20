#include <BTagScaleFactors.hpp>

#include <stdexcept>
#include <sstream>


using namespace std;


BTagScaleFactors::BTagScaleFactors(BTagger::Algorithm algo):
    BTagSFInterface()
{
    // For the given b-tagging algorithm fill the map from a working point to a set of correspoing
    //scale factor methods
    RawSFSingleWP *group;
    
    switch (algo)
    {
        case BTagger::Algorithm::TCHP:
            group = &rawScaleFactors[BTagger::WorkingPoint::Tight];
            group->tagSF = &BTagScaleFactors::GetSFTagTCHPT;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncTCHPT;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagTCHPTMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagTCHPTMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagTCHPTMax;
            break;
        
        case BTagger::Algorithm::CSV:
            group = &rawScaleFactors[BTagger::WorkingPoint::Tight];
            group->tagSF = &BTagScaleFactors::GetSFTagCSVT;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncCSVT;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagCSVTMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagCSVTMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagCSVTMax;
            
            group = &rawScaleFactors[BTagger::WorkingPoint::Medium];
            group->tagSF = &BTagScaleFactors::GetSFTagCSVM;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncCSVM;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagCSVMMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagCSVMMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagCSVMMax;
            
            group = &rawScaleFactors[BTagger::WorkingPoint::Loose];
            group->tagSF = &BTagScaleFactors::GetSFTagCSVL;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncCSVL;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagCSVLMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagCSVLMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagCSVLMax;
            break;
        
        case BTagger::Algorithm::CSVV1:
            group = &rawScaleFactors[BTagger::WorkingPoint::Tight];
            group->tagSF = &BTagScaleFactors::GetSFTagCSVV1T;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncCSVV1T;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagCSVV1TMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagCSVV1TMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagCSVV1TMax;
            
            group = &rawScaleFactors[BTagger::WorkingPoint::Medium];
            group->tagSF = &BTagScaleFactors::GetSFTagCSVV1M;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncCSVV1M;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagCSVV1MMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagCSVV1MMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagCSVV1MMax;
            
            group = &rawScaleFactors[BTagger::WorkingPoint::Loose];
            group->tagSF = &BTagScaleFactors::GetSFTagCSVV1L;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncCSVV1L;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagCSVV1LMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagCSVV1LMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagCSVV1LMax;
            break;
        
        case BTagger::Algorithm::CSVSLV1:
            group = &rawScaleFactors[BTagger::WorkingPoint::Tight];
            group->tagSF = &BTagScaleFactors::GetSFTagCSVSLV1T;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncCSVSLV1T;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagCSVSLV1TMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagCSVSLV1TMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagCSVSLV1TMax;
            
            group = &rawScaleFactors[BTagger::WorkingPoint::Medium];
            group->tagSF = &BTagScaleFactors::GetSFTagCSVSLV1M;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncCSVSLV1M;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagCSVSLV1MMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagCSVSLV1MMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagCSVSLV1MMax;
            
            group = &rawScaleFactors[BTagger::WorkingPoint::Loose];
            group->tagSF = &BTagScaleFactors::GetSFTagCSVSLV1L;
            group->tagSFUnc = &BTagScaleFactors::GetSFTagUncCSVSLV1L;
            group->mistagSFMean = &BTagScaleFactors::GetSFMistagCSVSLV1LMean;
            group->mistagSFMin = &BTagScaleFactors::GetSFMistagCSVSLV1LMin;
            group->mistagSFMax = &BTagScaleFactors::GetSFMistagCSVSLV1LMax;
            break;
        
        default:
        {
            ostringstream ost;
            ost << "BTagScaleFactors::BTagScaleFactors: Algorithm of b-tagging " << int(algo) <<
             " is not supported.";
            
            throw runtime_error(ost.str());
        }
    }
    
    
    // Set maximal supported pt for b- and c-quark jets
    if (algo == BTagger::Algorithm::CSVSLV1)
        ptMaxTag = 400.;
    else
        ptMaxTag = 800.;
}


BTagScaleFactors::BTagScaleFactors(BTagScaleFactors const &src):
    BTagSFInterface(src),
    rawScaleFactors(src.rawScaleFactors),
    ptMaxTag(src.ptMaxTag)
{}


BTagScaleFactors::BTagScaleFactors(BTagScaleFactors &&src):
    BTagSFInterface(move(src)),
    rawScaleFactors(move(src.rawScaleFactors)),
    ptMaxTag(src.ptMaxTag)
{}


BTagScaleFactors &BTagScaleFactors::operator=(BTagScaleFactors const &rhs)
{
    BTagSFInterface::operator=(rhs);
    
    rawScaleFactors = rhs.rawScaleFactors;
    ptMaxTag = rhs.ptMaxTag;
    
    return *this;
}


BTagSFInterface *BTagScaleFactors::Clone() const
{
    return new BTagScaleFactors(*this);
}


double BTagScaleFactors::GetScaleFactor(BTagger::WorkingPoint wp, Jet const &jet,
 Variation var /*= Variation::Nominal*/) const
{
    // Get pointers to the scale factor methods for the given working point
    auto sfGroupIt = rawScaleFactors.find(wp);
    
    if (sfGroupIt == rawScaleFactors.end())
    {
        ostringstream ost;
        ost << "BTagScaleFactors::GetScaleFactor: No b-tagging scale factors are available for " <<
         "working point " << int(wp) << ".";
        
        throw runtime_error(ost.str());
    }
    
    auto const &sfGroup = sfGroupIt->second;
    
    
    // A scale factor to increase the uncertainty if needed
    double uncFactor = 1.;
    
    
    // Switch between heavy-flavour and light-flavour jets
    unsigned const absFlavour = abs(jet.GetParentID());
    
    if (absFlavour == 4 or absFlavour == 5)  // b- or c-quark jets
    {
        // Constrain jet momentum to the supported range
        double pt = jet.Pt();
        
        if (pt < 20.)
        {
            pt = 20.;
            uncFactor *= 2;
        }
        else if (pt > ptMaxTag)
        {
            pt = ptMaxTag;
            uncFactor *= 2;
        }
        
        
        // Calculate the nominal scale factor
        double const nominalSF = (*sfGroup.tagSF)(pt);
        
        if (var == Variation::Nominal)
            return nominalSF;
        
        
        // The workflow continues if only a systematic variation has been requested
        // Increase uncertainty factor for c-quark jets
        if (absFlavour == 4)
            uncFactor *= 2;
        
        // Calculate the uncertainty
        double const uncertainty = (*sfGroup.tagSFUnc)(pt);
        
        if (var == Variation::Up)
            return nominalSF + uncertainty * uncFactor;
        else
            return nominalSF - uncertainty * uncFactor;
    }
    else  // light-flavour or unidentified jets
    {
        // Constrain jet momentum to the supported range
        double pt = jet.Pt();
        double absEta = fabs(jet.Eta());
        
        if (pt < 20.)
        {
            pt = 20.;
            uncFactor *= 2;
        }
        else
        {
            if (pt > 850. and ((wp == BTagger::WorkingPoint::Loose and absEta > 1.5) or
             (wp == BTagger::WorkingPoint::Medium and absEta > 1.6)))
            {
                pt = 850.;
                uncFactor *= 2;
            }
            else if (pt > 1000.)
            {
                pt = 1000.;
                uncFactor *= 2;
            }
        }
        
        
        // Calculate the nominal scale factor
        double const nominalSF = (*sfGroup.mistagSFMean)(pt, absEta);
        
        if (var == Variation::Nominal)
            return nominalSF;
        
        
        // The workflow continues if only a systematic variation has been requested
        if (var == Variation::Up)
        {
            double const upSF = (*sfGroup.mistagSFMax)(pt, absEta);
            return nominalSF + (upSF - nominalSF) * uncFactor;
        }
        else
        {
            double const downSF = (*sfGroup.mistagSFMin)(pt, absEta);
            return nominalSF - (nominalSF - downSF) * uncFactor;
        }
    }
}


// The code below was generated automatically
double BTagScaleFactors::GetSFTagTCHPT(double pt)
{
    return 0.703389*((1.+(0.088358*pt))/(1.+(0.0660291*pt)));
}

double BTagScaleFactors::GetSFTagCSVL(double pt)
{
    return 0.997942*((1.+(0.00923753*pt))/(1.+(0.0096119*pt)));
}

double BTagScaleFactors::GetSFTagCSVM(double pt)
{
    return (0.938887+(0.00017124*pt))+(-2.76366e-07*(pt*pt));
}

double BTagScaleFactors::GetSFTagCSVT(double pt)
{
    return (0.927563+(1.55479e-05*pt))+(-1.90666e-07*(pt*pt));
}

double BTagScaleFactors::GetSFTagCSVV1L(double pt)
{
    return 1.7586*((1.+(0.799078*pt))/(1.+(1.44245*pt)));
}

double BTagScaleFactors::GetSFTagCSVV1M(double pt)
{
    return 0.952067+(-2.00037e-05*pt);
}

double BTagScaleFactors::GetSFTagCSVV1T(double pt)
{
    return (0.912578+(0.000115164*pt))+(-2.24429e-07*(pt*pt));
}

double BTagScaleFactors::GetSFTagCSVSLV1L(double pt)
{
    return 0.970168*((1.+(0.00266812*pt))/(1.+(0.00250852*pt)));
}

double BTagScaleFactors::GetSFTagCSVSLV1M(double pt)
{
    return ((0.939238+(0.000278928*pt))+(-7.49693e-07*(pt*pt)))+(2.04822e-10*(pt*(pt*pt)));
}

double BTagScaleFactors::GetSFTagCSVSLV1T(double pt)
{
    return (0.928257+(9.3526e-05*pt))+(-4.1568e-07*(pt*pt));
}

double BTagScaleFactors::GetSFTagUncTCHPT(double pt)
{
    static double const unc[16] = {0.0624031, 0.034023, 0.0362764, 0.0341996, 0.031248, 0.0281222, 0.0316684, 0.0276272, 0.0208828, 0.0223511, 0.0224121, 0.0261939, 0.0268247, 0.0421413, 0.0532897, 0.0506714};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::GetSFTagUncCSVL(double pt)
{
    static double const unc[16] = {0.033299, 0.0146768, 0.013803, 0.0170145, 0.0166976, 0.0137879, 0.0149072, 0.0153068, 0.0133077, 0.0123737, 0.0157152, 0.0175161, 0.0209241, 0.0278605, 0.0346928, 0.0350099};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::GetSFTagUncCSVM(double pt)
{
    static double const unc[16] = {0.0415707, 0.0204209, 0.0223227, 0.0206655, 0.0199325, 0.0174121, 0.0202332, 0.0182446, 0.0159777, 0.0218531, 0.0204688, 0.0265191, 0.0313175, 0.0415417, 0.0740446, 0.0596716};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::GetSFTagUncCSVT(double pt)
{
    static double const unc[16] = {0.0515703, 0.0264008, 0.0272757, 0.0275565, 0.0248745, 0.0218456, 0.0253845, 0.0239588, 0.0271791, 0.0273912, 0.0379822, 0.0411624, 0.0786307, 0.0866832, 0.0942053, 0.102403};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::GetSFTagUncCSVV1L(double pt)
{
    static double const unc[16] = {0.0345802, 0.0152688, 0.0149101, 0.0167145, 0.0167098, 0.013472, 0.0146024, 0.0156735, 0.0142592, 0.0147227, 0.0167101, 0.0191159, 0.0360389, 0.0331342, 0.0336916, 0.0298064};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::GetSFTagUncCSVV1M(double pt)
{
    static double const unc[16] = {0.0376303, 0.0187774, 0.019884, 0.0215849, 0.0207925, 0.0180289, 0.0178674, 0.0159339, 0.019042, 0.020975, 0.0189178, 0.0246477, 0.0291784, 0.0428437, 0.0674624, 0.0479834};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::GetSFTagUncCSVV1T(double pt)
{
    static double const unc[16] = {0.0564014, 0.0293159, 0.0315288, 0.0301526, 0.0266047, 0.0240973, 0.0254404, 0.0241548, 0.0233434, 0.0303961, 0.040912, 0.042942, 0.0440911, 0.0555312, 0.105762, 0.0886457};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::GetSFTagUncCSVSLV1L(double pt)
{
    static double const unc[13] = {0.135344, 0.0288656, 0.0259088, 0.0199242, 0.0189792, 0.0178341, 0.0187104, 0.0239028, 0.0211104, 0.017689, 0.02823, 0.0259654, 0.0614497};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::GetSFTagUncCSVSLV1M(double pt)
{
    static double const unc[13] = {0.0918443, 0.0282557, 0.0264246, 0.0242536, 0.0218046, 0.0207568, 0.0207962, 0.0208919, 0.0200894, 0.0258879, 0.0270699, 0.0256006, 0.0438219};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::GetSFTagUncCSVSLV1T(double pt)
{
    static double const unc[13] = {0.10761, 0.0333696, 0.0339123, 0.0302699, 0.0261626, 0.0274243, 0.0224287, 0.0239842, 0.0267866, 0.0254787, 0.0317589, 0.0365968, 0.0481259};
    
    return CalcSFTagUnc(pt, unc);
}

double BTagScaleFactors::CalcSFTagUnc(double pt, double const *unc)
{
    static double const ptMax[16] = {30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 600, 800};
    unsigned bin = 0;
    
    while (pt > ptMax[bin])
        ++bin;
    
    return unc[bin];
}

double BTagScaleFactors::GetSFMistagTCHPTMean(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((1.20175+(0.000858187*pt))+(-1.98726e-06*(pt*pt)))+(1.31057e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagTCHPTMin(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((0.968557+(0.000586877*pt))+(-1.34624e-06*(pt*pt)))+(9.09724e-10*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagTCHPTMax(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((1.43508+(0.00112666*pt))+(-2.62078e-06*(pt*pt)))+(1.70697e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVLMean(double pt, double absEta)
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
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVLMin(double pt, double absEta)
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
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVLMax(double pt, double absEta)
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
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVMMean(double pt, double absEta)
{
    if (absEta < 0.8)
        return ((1.07541+(0.00231827*pt))+(-4.74249e-06*(pt*pt)))+(2.70862e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.05613+(0.00114031*pt))+(-2.56066e-06*(pt*pt)))+(1.67792e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.05625+(0.000487231*pt))+(-2.22792e-06*(pt*pt)))+(1.70262e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVMMin(double pt, double absEta)
{
    if (absEta < 0.8)
        return ((0.964527+(0.00149055*pt))+(-2.78338e-06*(pt*pt)))+(1.51771e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((0.946051+(0.000759584*pt))+(-1.52491e-06*(pt*pt)))+(9.65822e-10*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.956736+(0.000280197*pt))+(-1.42739e-06*(pt*pt)))+(1.0085e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVMMax(double pt, double absEta)
{
    if (absEta < 0.8)
        return ((1.18638+(0.00314148*pt))+(-6.68993e-06*(pt*pt)))+(3.89288e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.16624+(0.00151884*pt))+(-3.59041e-06*(pt*pt)))+(2.38681e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.15575+(0.000693344*pt))+(-3.02661e-06*(pt*pt)))+(2.39752e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVTMean(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((1.00462+(0.00325971*pt))+(-7.79184e-06*(pt*pt)))+(5.22506e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVTMin(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((0.845757+(0.00186422*pt))+(-4.6133e-06*(pt*pt)))+(3.21723e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVTMax(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((1.16361+(0.00464695*pt))+(-1.09467e-05*(pt*pt)))+(7.21896e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVV1LMean(double pt, double absEta)
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
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVV1LMin(double pt, double absEta)
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
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVV1LMax(double pt, double absEta)
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
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVV1MMean(double pt, double absEta)
{
    if (absEta < 0.8)
        return ((1.06383+(0.00279657*pt))+(-5.75405e-06*(pt*pt)))+(3.4302e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.03709+(0.00169762*pt))+(-3.52511e-06*(pt*pt)))+(2.25975e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.01679+(0.00211998*pt))+(-6.26097e-06*(pt*pt)))+(4.53843e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVV1MMin(double pt, double absEta)
{
    if (absEta < 0.8)
        return ((0.971686+(0.00195242*pt))+(-3.98756e-06*(pt*pt)))+(2.38991e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((0.947328+(0.00117422*pt))+(-2.32363e-06*(pt*pt)))+(1.46136e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.922527+(0.00176245*pt))+(-5.14169e-06*(pt*pt)))+(3.61532e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVV1MMax(double pt, double absEta)
{
    if (absEta < 0.8)
        return ((1.15605+(0.00363538*pt))+(-7.50634e-06*(pt*pt)))+(4.4624e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.12687+(0.00221834*pt))+(-4.71949e-06*(pt*pt)))+(3.05456e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.11102+(0.00247531*pt))+(-7.37745e-06*(pt*pt)))+(5.46589e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVV1TMean(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((1.15047+(0.00220948*pt))+(-5.17912e-06*(pt*pt)))+(3.39216e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVV1TMin(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((0.936862+(0.00149618*pt))+(-3.64924e-06*(pt*pt)))+(2.43883e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVV1TMax(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((1.36418+(0.00291794*pt))+(-6.6956e-06*(pt*pt)))+(4.33793e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVSLV1LMean(double pt, double absEta)
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
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVSLV1LMin(double pt, double absEta)
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
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVSLV1LMax(double pt, double absEta)
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
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVSLV1MMean(double pt, double absEta)
{
    if (absEta < 0.8)
        return ((1.06212+(0.00223614*pt))+(-4.25167e-06*(pt*pt)))+(2.42728e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.04547+(0.00216995*pt))+(-4.579e-06*(pt*pt)))+(2.91791e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.991865+(0.00324957*pt))+(-9.65897e-06*(pt*pt)))+(7.13694e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVSLV1MMin(double pt, double absEta)
{
    if (absEta < 0.8)
        return ((0.903956+(0.00121678*pt))+(-2.04383e-06*(pt*pt)))+(1.10727e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((0.900637+(0.00120088*pt))+(-2.27069e-06*(pt*pt)))+(1.40609e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((0.868875+(0.00222761*pt))+(-6.44897e-06*(pt*pt)))+(4.53261e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVSLV1MMax(double pt, double absEta)
{
    if (absEta < 0.8)
        return ((1.22035+(0.00325183*pt))+(-6.45023e-06*(pt*pt)))+(3.74225e-09*(pt*(pt*pt)));
    else if (absEta < 1.6)
        return ((1.19034+(0.00313562*pt))+(-6.87854e-06*(pt*pt)))+(4.42546e-09*(pt*(pt*pt)));
    else if (absEta < 2.4)
        return ((1.11481+(0.00426745*pt))+(-1.28612e-05*(pt*pt)))+(9.74425e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVSLV1TMean(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((1.09494+(0.00193966*pt))+(-4.35021e-06*(pt*pt)))+(2.8973e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVSLV1TMin(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((0.813331+(0.00139561*pt))+(-3.15313e-06*(pt*pt)))+(2.12173e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}

double BTagScaleFactors::GetSFMistagCSVSLV1TMax(double pt, double absEta)
{
    if (absEta < 2.4)
        return ((1.37663+(0.00247963*pt))+(-5.53583e-06*(pt*pt)))+(3.66635e-09*(pt*(pt*pt)));
    else
    {
        throw runtime_error("Jet pseudorapidity is out of range in BTagScaleFactors.");
        return -100.;
    }
}
// End of automatically generated code
