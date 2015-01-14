/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014 Max-Planck-Institute for Intelligent Systems,
 *                     University of Southern California
 *    Manuel Wuthrich (manuel.wuthrich@gmail.com)
 *
 *
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
 * @date 05/25/2014
 * @author Manuel Wuthrich (manuel.wuthrich@gmail.com)
 * Max-Planck-Institute for Intelligent Systems, University of Southern California
 */

#ifndef FAST_FILTERING_MODELS_PROCESS_MODELS_DAMPED_WIENER_PROCESS_MODEL_HPP
#define FAST_FILTERING_MODELS_PROCESS_MODELS_DAMPED_WIENER_PROCESS_MODEL_HPP

//TODO: THIS IS A LINEAR GAUSSIAN PROCESS, THIS CLASS SHOULD DISAPPEAR

#include <Eigen/Dense>

#include <fl/util/assertions.hpp>
#include <ff/models/process_models/interfaces/stationary_process_model.hpp>
#include <fl/distribution/gaussian.hpp>

namespace fl
{

// Forward declarations
template <typename State> class DampedWienerProcessModel;

/**
 * DampedWienerProcess distribution traits specialization
 * \internal
 */
template <typename State_>
struct Traits<DampedWienerProcessModel<State_>>
{
    typedef State_                                              State;
    typedef typename State::Scalar                              Scalar;
    typedef Eigen::Matrix<Scalar, State::SizeAtCompileTime, 1>  Input;
    typedef Eigen::Matrix<Scalar, State::SizeAtCompileTime, 1>  Noise;

    typedef Gaussian<Noise> GaussianType;
    typedef typename GaussianType::Operator Operator;

    typedef StationaryProcessModel<State, Input> ProcessModelBase;
    typedef GaussianMap<State, Noise> GaussianMapBase;
};

/**
 * \class DampedWienerProcess
 *
 * \ingroup process_models
 */
template <typename State>
class DampedWienerProcessModel:
        public Traits<DampedWienerProcessModel<State>>::ProcessModelBase,
        public Traits<DampedWienerProcessModel<State>>::GaussianMapBase
{
public:
    typedef DampedWienerProcessModel<State> This;

    typedef typename Traits<This>::Scalar         Scalar;
    typedef typename Traits<This>::Operator       Operator;
    typedef typename Traits<This>::Input          Input;
    typedef typename Traits<This>::Noise          Noise;
    typedef typename Traits<This>::GaussianType   GaussianType;

public:
    explicit DampedWienerProcessModel(const unsigned& dimension = State::SizeAtCompileTime):
        Traits<This>::GaussianMapBase(dimension),
        gaussian_(dimension)
    {
        // check that state is derived from eigen
        static_assert_base(State, Eigen::Matrix<typename State::Scalar, State::SizeAtCompileTime, 1>);
    }

    virtual ~DampedWienerProcessModel() { }

    virtual State MapStandardGaussian(const Noise& sample) const
    {
        return gaussian_.MapStandardGaussian(sample);
    }

    virtual void Condition(const Scalar&  delta_time,
                           const State&  state,
                           const Input&   input)
    {
        gaussian_.Mean(Mean(delta_time, state, input));
        gaussian_.DiagonalCovariance(Covariance(delta_time));
    }

    virtual void Parameters(const Scalar& damping,
                            const Operator& noise_covariance)
    {
        damping_ = damping;
        noise_covariance_ = noise_covariance;
    }

    virtual unsigned Dimension() const
    {
        return this->NoiseDimension(); // all dimensions are the same
    }

private:
    State Mean(const Scalar& delta_time,
                    const State& state,
                    const Input& input)
    {
        if(damping_ == 0)
            return state + delta_time * input;

        State state_expectation = (1.0 - exp(-damping_*delta_time)) / damping_ * input +
                                              exp(-damping_*delta_time)  * state;

        // if the damping_ is too small, the result might be nan, we thus return the limit for damping_ -> 0
        if(!std::isfinite(state_expectation.norm()))
            state_expectation = state + delta_time * input;

        return state_expectation;
    }

    Operator Covariance(const Scalar& delta_time)
    {
        if(damping_ == 0)
            return delta_time * noise_covariance_;

        Scalar factor = (1.0 - exp(-2.0*damping_*delta_time))/(2.0*damping_);
        if(!std::isfinite(factor))
            factor = delta_time;

        return factor * noise_covariance_;
    }

private:
    // conditional
    GaussianType gaussian_;

    // parameters
    Scalar damping_;
    Operator noise_covariance_;

    // euler-mascheroni constant
    static constexpr Scalar gamma_ = 0.57721566490153286060651209008240243104215933593992;
};

}

#endif
