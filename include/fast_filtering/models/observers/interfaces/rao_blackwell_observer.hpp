/*************************************************************************
This software allows for filtering in high-dimensional observation and
state spaces, as described in

M. Wuthrich, P. Pastor, M. Kalakrishnan, J. Bohg, and S. Schaal.
Probabilistic Object Tracking using a Range Camera
IEEE/RSJ Intl Conf on Intelligent Robots and Systems, 2013

In a publication based on this software pleace cite the above reference.


Copyright (C) 2014  Manuel Wuthrich

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/


#ifndef STATE_FILTERING_MODELS_OBSERVERS_INTERFACE_RAO_BLACKWELL_OBSERVER_HPP
#define STATE_FILTERING_MODELS_OBSERVERS_INTERFACE_RAO_BLACKWELL_OBSERVER_HPP

#include <vector>
#include <state_filtering/utils/traits.hpp>


namespace sf
{
template<typename State, typename Observation_>
class RaoBlackwellObserver
{
public:
    typedef typename internal::VectorTraits<State>::Scalar Scalar;
    typedef Observation_  Observation;

public:
    virtual ~RaoBlackwellObserver() {}

    // since we can not implicitly cast a vector globally we do it here locally
    virtual std::vector<Scalar> Loglikes(const std::vector<State>& states,
                                         std::vector<size_t>& indices,
                                         const bool& update = false) = 0;

    virtual void SetObservation(const Observation& image, const Scalar& delta_time) = 0;

    // reset the latent variables
    virtual void Reset() = 0;
};

}
#endif