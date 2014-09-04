/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014 Max-Planck-Institute for Intelligent Systems,
 *                     University of Southern California,
 *                     Karlsruhe Institute of Technology
 *    Jan Issac (jan.issac@gmail.com)
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * @date 05/19/2014
 * @author Jan Issac (jan.issac@gmail.com)
 * Max-Planck-Institute for Intelligent Systems, University of Southern California (USC),
 *   Karlsruhe Institute of Technology (KIT)
 */

#ifndef FAST_FILTERING_FILTER_FILTER_CONTEXT_HPP
#define FAST_FILTERING_FILTER_FILTER_CONTEXT_HPP

#include <fast_filtering/distributions/interfaces/moments_estimable.hpp>

/**
 * @brief State filtering namespace
 */
namespace sf
{

/**
 * @brief FilterContext is a generic interface of a context containing a filter algorithm
 *
 * FilterContext is a generic interface of a filter algorithm context. A specialization of this
 * may run any type of filter with its own interface. Each filter must be able to provide at
 * least the three functions this interface provides. The underlying algorithm and may use any
 * representation for its data and may have any interface it requires.
 *
 * The Filter context layer provides a simple and minimal interface for the client. It also
 * provides the possibility to use stateless and stateful filters. Stateless filters are a clean
 * way to implement an algorithm, however the client has to maintain the state. In this case
 * FilterContext takes care of this matter. A stateful filter is a filter which stores and
 * manages the state internally. This may due to performance reasons such as in the case of
 * GPU implementations. Both, stateless and stateful filter can be used in the same fashion.
 */

// TODO: THESE TEMPLATE PARAMETERS HAVE TO CHANGE!! FOR NOW FOR SIMPLICITY THE BELOW IS A HACK
template <typename Scalar_, int SIZE, typename Observation, typename ControlType>
class FilterContext
{
public:
    typedef Scalar_ Scalar;
    typedef Eigen::Matrix<Scalar, SIZE, 1> Vector;
    typedef Eigen::Matrix<Scalar, SIZE, SIZE> Operator;
    typedef MomentsEstimable<Scalar, Vector, Operator> StateDistribution;

    virtual ~FilterContext() { }

    /**
     * @brief Propagates the current state and updates it using the observation
     *
     * @param observation   Most recent observation used to update the state
     * @param time          Current timestamp
     */
    virtual void predictAndUpdate(const Observation& observation,
                                  double time,
                                  const ControlType& control) = 0;

    /**
     * @return Accesses the filtered state
     */
    virtual StateDistribution& stateDistribution() = 0;
};

}

#endif
