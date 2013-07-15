/**
 * \file WeightPileUpInterface.hpp
 * \author Andrey Popov
 * 
 * The module defines and interface class to introduce pile-up reweighting.
 */


#pragma once

#include <DatasetForward.hpp>


/**
 * \class WeightPileUpInterface
 * \brief An abstract class to define an interface for pile-up reweighting
 * 
 * The actual implementation is left for a derived class.
 */
class WeightPileUpInterface
{
    public:
        /// An auxiliary structure to aggregate central weight and its systematical variations
        struct Weights
        {
            /// Constructor without parameters sets all weights to zeros
            Weights();
            
            /// Constructor with initialisation
            Weights(double central, double up, double down);
            
            /// Default copy constructor
            Weights(Weights const &) = default;
            
            /// Default assignment operator
            Weights &operator=(Weights const &) = default;
            
            /// Resets data members
            void Set(double central, double up, double down);
            
            /// Central weight
            double central;
            
            /// Up variation
            double up;
            
            /// Down variation
            double down;
        };
    
    public:
        /// Default constructor
        WeightPileUpInterface() = default;
        
        /// Default destructor
        virtual ~WeightPileUpInterface() = default;
    
    public:
        /**
         * \brief Returns a newly-initialized copy of the class instance
         * 
         * The method is expected to be called before the first call to SetDataset or GetWeights.
         * The created instance must be initialized exactly in the same manner as this, but it might
         * not copy data members describing the current state of the instance. The method must be
         * implemented in a derived class.
         */
        virtual WeightPileUpInterface *Clone() const = 0;
        
        /**
         * \brief Notifies this of the dataset that is about to be processed
         * 
         * The method must be called whenever processing of a new dataset is started. In the default
         * implementation the method is empty, but a derived class might use it to update the MC
         * pile-up distribution to match the current dataset.
         */
        virtual void SetDataset(Dataset const &dataset);
        
        /**
         * \brief Calculates event weight given the "true" number of pile-up interactions
         * 
         * A pure virtual method to calculate the central and systematically-varied weights given
         * the "true" number of pile-up interactions.
         */
        virtual Weights GetWeights(double nTruth) const = 0;
};