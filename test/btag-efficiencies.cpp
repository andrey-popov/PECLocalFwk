#include <PECFwk/core/BTagger.hpp>
#include <PECFwk/core/Dataset.hpp>
#include <PECFwk/extensions/BTagEffService.hpp>

#include <iomanip>
#include <iostream>
#include <utility>


using namespace std;


int main()
{
    BTagger const bTagger(BTagger::Algorithm::CSV, BTagger::WorkingPoint::Tight);
    
    BTagEffService bTagEffService("BTagEff_74X_v1.0.root");
    // bTagEffService.SetProcessLabel(Dataset::Process::ttSemilep, "ttbar-semilep");
    bTagEffService.SetDefaultProcessLabel("ttbar");
    
    Dataset fakeDataset({Dataset::Process::ttbar, Dataset::Process::ttSemilep});
    
    bTagEffService.BeginRun(fakeDataset);
    
    
    cout << fixed;
    
    for (double const &pt: {15., 25., 50., 2000.})
        for (double const &eta: {1., -1., 3.})
        {
            cout << setprecision(0);
            cout << "Efficiencies of b-tagging for pt = " << pt << ", eta = " << eta << ":\n";
            cout << setprecision(4);
            
            for (auto const &f: {make_pair(5, "b"), make_pair(4, "c"), make_pair(0, "light")})
            {
                double const eff = bTagEffService.GetEfficiency(bTagger, pt, eta, f.first);
                
                cout << " " << left << setw(5) << f.second << " :  ";
                
                if (eff < 1.e-3)
                    cout << "\033[0;37m" << eff << "\033[0m";
                else
                    cout << eff;
                
                cout << '\n';
            }
            
            cout << '\n';
        }
    
    
    return EXIT_SUCCESS;
}
