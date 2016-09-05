/**
 * This program tests BTagSFService.
 */

#include <mensura/core/BTagger.hpp>
#include <mensura/extensions/BTagSFService.hpp>

#include <iomanip>
#include <iostream>
#include <utility>


using namespace std;


int main()
{
    BTagger const bTagger(BTagger::Algorithm::CSV, BTagger::WorkingPoint::Medium);
    
    BTagSFService bTagSFService(bTagger, "BTagSF_76X_CSVv2.csv");
    bTagSFService.SetMeasurement(BTagSFService::Flavour::Bottom, "mujets");
    bTagSFService.SetMeasurement(BTagSFService::Flavour::Charm, "mujets");
    bTagSFService.SetMeasurement(BTagSFService::Flavour::Light, "incl");
    
    
    cout << fixed;
    
    for (double const &pt: {15., 25., 30., 50., 100., 1000., 2000.})
    {
        cout << "Scale factors for b-tagging for pt = " << setprecision(0) << pt << ", eta = 0:\n";
        cout << setprecision(3);
        
        for (auto const &f: {make_pair(5, "b"), make_pair(4, "c"), make_pair(0, "light")})
        {
            double const nominal =
              bTagSFService.GetScaleFactor(pt, 0., f.first, BTagSFService::Variation::Nominal);
            double const up =
              bTagSFService.GetScaleFactor(pt, 0., f.first, BTagSFService::Variation::Up);
            double const down =
              bTagSFService.GetScaleFactor(pt, 0., f.first, BTagSFService::Variation::Down);
            
            cout << " " << left << setw(5) << f.second << " :  " << nominal << "  [" << down <<
              ",  " << up << "]\n";
        }
        
        cout << '\n';
    }
    
    
    return EXIT_SUCCESS;
}
