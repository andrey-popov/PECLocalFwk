/**
 * This program tests BTagEffService.
 */

#include <mensura/BTagEffService.hpp>
#include <mensura/BTagger.hpp>
#include <mensura/Dataset.hpp>

#include <iomanip>
#include <iostream>
#include <utility>


using namespace std;


int main()
{
    BTagger const bTagger(BTagger::Algorithm::CSV, BTagger::WorkingPoint::Tight);
    
    BTagEffService bTagEffService("BTagEff_80Xv3.root");
    bTagEffService.SetDefaultEffLabel("ttbar");
    
    Dataset fakeDataset(Dataset::Type::MC, "ttbar");
    
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
