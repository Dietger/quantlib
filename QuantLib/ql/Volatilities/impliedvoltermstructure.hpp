
/*
 Copyright (C) 2003 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file impliedvoltermstructure.hpp
    \brief Implied Black Vol Term Structure

    \fullpath
    ql/Volatilities/%impliedvoltermstructure.hpp
*/

// $Id$

#ifndef quantlib_implied_vol_term_structure_hpp
#define quantlib_implied_vol_term_structure_hpp

#include <ql/voltermstructure.hpp>

namespace QuantLib {
    namespace VolTermStructures {

        //! Implied vol term structure at a given date in the future
        /*! The given date will be the implied reference date.
            \note This term structure will remain linked to the original
                  structure, i.e., any changes in the latter will be reflected
                  in this structure as well.

            \warning It doesn't make financial sense to have an
                     asset-dependant implied Vol Term Structure.
                     This class should be used with term structures that
                     are time dependant only
        */
        class ImpliedVolTermStructure : public BlackVarianceTermStructure,
                                        public Patterns::Observer {
          public:
            ImpliedVolTermStructure(
                const RelinkableHandle<BlackVolTermStructure>& originalTS,
                const Date& newReferenceDate);
            Date referenceDate() const { return newReferenceDate_; }
            DayCounter dayCounter() const ;
            Date maxDate() const ;
            // Observer interface
            void update();
          protected:
            virtual double blackVarianceImpl(Time t, double strike,
                bool extrapolate = false) const;
          private:
            RelinkableHandle<BlackVolTermStructure> originalTS_;
            Date newReferenceDate_;
        };



        inline ImpliedVolTermStructure::ImpliedVolTermStructure(
            const RelinkableHandle<BlackVolTermStructure>& originalTS,
            const Date& newReferenceDate)
        : originalTS_(originalTS), newReferenceDate_(newReferenceDate) {
            registerWith(originalTS_);
        }

        inline DayCounter ImpliedVolTermStructure::dayCounter() const {
            return originalTS_->dayCounter();
        }

        inline Date ImpliedVolTermStructure::maxDate() const {
            return originalTS_->maxDate();
        }

        inline void ImpliedVolTermStructure::update() {
            notifyObservers();
        }

        inline double ImpliedVolTermStructure::blackVarianceImpl(Time t,
            double strike, bool extrapolate) const {
                /* timeShift (and/or variance) variance at evaluation date
                   cannot be cached since the original curve could change
                   between invocations of this method */
                Time timeShift = dayCounter().yearFraction(
                    originalTS_->referenceDate(), newReferenceDate_);
                /* t is relative to the current reference date
                   and needs to be converted to the time relative
                   to the reference date of the original curve */
                return originalTS_->blackForwardVariance(timeShift,
                    timeShift+t, strike, extrapolate);
        }
    }
}


#endif
