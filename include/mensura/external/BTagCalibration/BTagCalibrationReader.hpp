#ifndef BTagCalibrationReader_H
#define BTagCalibrationReader_H

/**
 * BTagCalibrationReader
 *
 * Helper class to pull out a specific set of BTagEntry's out of a
 * BTagCalibration. TF1 functions are set up at initialization time.
 *
 ************************************************************/

#include <mensura/external/BTagCalibration/BTagCalibration.hpp>
 
#include <memory>
#include <string>



class BTagCalibrationReader
{
public:
  BTagCalibrationReader() noexcept;
  BTagCalibrationReader(BTagEntry::OperatingPoint op,
                        std::string sysType="central");
  ~BTagCalibrationReader() noexcept;

  void load(const BTagCalibration & c,
            BTagEntry::JetFlavor jf,
            std::string measurementType="comb");

  double eval(BTagEntry::JetFlavor jf,
              float eta,
              float pt,
              float discr=0.) const;

  std::pair<float, float> min_max_pt(BTagEntry::JetFlavor jf, 
                                     float eta, 
                                     float discr=0.) const;

protected:
  class BTagCalibrationReaderImpl;
  std::unique_ptr<BTagCalibrationReaderImpl> pimpl;
};


#endif  // BTagCalibrationReader_H
