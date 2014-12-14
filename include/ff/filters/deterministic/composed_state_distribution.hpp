/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014 Max-Planck-Institute for Intelligent Systems,
 *                     University of Southern California
 *    Jan Issac (jan.issac@gmail.com)
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
 * @date 2014
 * @author Jan Issac (jan.issac@gmail.com)
 * @author Manuel Wuthrich (manuel.wuthrich@gmail.com)
 * Max-Planck-Institute for Intelligent Systems, University of Southern California
 */

#ifndef FAST_FILTERING_STATES_COMPOSED_STATE_DISTRIBUTION_HPP
#define FAST_FILTERING_STATES_COMPOSED_STATE_DISTRIBUTION_HPP

#include <Eigen/Dense>
#include <vector>
#include <boost/static_assert.hpp>

#include <fl/util/traits.hpp>
#include <fl/distribution/gaussian.hpp>

namespace fl
{


//template <typename VectorPartition>
//class FactorizedGaussian
//{
//public:
//    typedef fl::Gaussian<VectorPartition> PartitionGaussian;

//    virtual const PartitionGaussian& Gaussian(size_t i)
//    {
//        return gaussians_[i];
//    }

//    virtual const PartitionGaussian& operator[](size_t i)
//    {
//        return gaussians_[i];
//    }

//protected:
//    std::vector<PartitionGaussian> gaussians_;
//};


//template <typename VectorPartition0, typename VectorPartition>
//class FactorizedGaussian<VectorPartition>:
//        public FactorizedGaussian<VectorPartition>
//{
//public:
//    typedef fl::Gaussian<VectorPartition0> HeadGaussianPartition;
//    using PartitionGaussian;
//    using Gaussian;
//    using operator[];

//    virtual const HeadGaussianPartition& HeadGaussian()
//    {
//        return head_gaussian_;
//    }


//protected:
//    HeadGaussianPartition head_gaussian_;
//};


// Forward declarations
template <typename CohesiveState, typename FactorizedState, typename Observation>
class ComposedStateDistribution;

/**
 * ComposedStateDistribution distribution traits specialization
 * \internal
 */
template <typename CohesiveState,
          typename FactorizedState,
          typename Observation>
struct Traits<ComposedStateDistribution<
        CohesiveState, FactorizedState, Observation>>
{
    typedef typename CohesiveState::Scalar Scalar;

    typedef Eigen::Matrix<Scalar,
                          CohesiveState::SizeAtCompileTime,
                          CohesiveState::SizeAtCompileTime> Cov_aa;

    typedef Eigen::Matrix<Scalar,
                          CohesiveState::SizeAtCompileTime,
                          FactorizedState::SizeAtCompileTime> Cov_ab;

    typedef Eigen::Matrix<Scalar,
                          CohesiveState::SizeAtCompileTime,
                          Observation::SizeAtCompileTime> Cov_ay;

    typedef Eigen::Matrix<Scalar,
                          FactorizedState::SizeAtCompileTime,
                          FactorizedState::SizeAtCompileTime> Cov_bb;

    typedef Eigen::Matrix<Scalar,
                          FactorizedState::SizeAtCompileTime,
                          Observation::SizeAtCompileTime> Cov_by;

    typedef Eigen::Matrix<Scalar, Observation::SizeAtCompileTime, 1> Y;
    typedef Eigen::Matrix<Scalar,
                          Observation::SizeAtCompileTime,
                          Observation::SizeAtCompileTime> Cov_yy;
};

/**
 * \class ComposedStateDistribution
 * \ingroup states
 */
template <typename CohesiveState,
          typename FactorizedState,
          typename Observation>
class ComposedStateDistribution
{
public:
    typedef ComposedStateDistribution<
                CohesiveState,
                FactorizedState,
                Observation
            > This;

    typedef typename Traits<This>::Scalar Scalar;
    typedef typename Traits<This>::Cov_aa Cov_aa;
    typedef typename Traits<This>::Cov_ab Cov_ab;
    typedef typename Traits<This>::Cov_ay Cov_ay;
    typedef typename Traits<This>::Cov_bb Cov_bb;
    typedef typename Traits<This>::Cov_by Cov_by;
    typedef typename Traits<This>::Cov_yy Cov_yy;
    typedef typename Traits<This>::Y Y;

    struct JointPartitions
    {
        FactorizedState mean_b;
        Y mean_y;

        Cov_ab cov_ab;
        Cov_ay cov_ay;
        Cov_bb cov_bb;
        Cov_by cov_by;
        Cov_yy cov_yy;
    };

public:
    /**
     * Creates a composed state distribution. This distribution harbors a
     * cohesive state part and a number of factorized states. The total state
     * dimension is \f$\dim(CohesiveState) + FACTORIZED\_STATES *
     * \dim(FactorizedState) \f$.
     */
    ComposedStateDistribution()
    {
    }    

    virtual ~ComposedStateDistribution() { }

    void initialize(const CohesiveState& initial_a,
                    const size_t factorized_states_count,
                    const FactorizedState& initial_b_i,
                    const Scalar sigma_a = 1.0,
                    const Scalar sigma_b_i = 1.0)
    {
        mean_a = initial_a;
        mean_a_predicted = initial_a;
        cov_aa = Cov_aa::Identity(a_dimension(), a_dimension()) * sigma_a;

        joint_partitions.resize(factorized_states_count);
        for (auto& partition: joint_partitions)
        {
            partition.mean_b = initial_b_i;
            partition.cov_bb = Cov_bb::Identity(b_i_dimension(),
                                                b_i_dimension()) * sigma_b_i;
        }
    }

    size_t a_dimension() const
    {
        return mean_a.rows();
    }

    size_t b_i_dimension() const
    {
        if (joint_partitions.size() > 0)
        {
            return joint_partitions[0].mean_b.rows();
        }

        return 0;
    }

    size_t count_partitions() const
    {
        return joint_partitions.size();
    }

public:
    CohesiveState mean_a;
    CohesiveState mean_a_predicted;
    Cov_aa cov_aa;
    Cov_aa cov_aa_inverse;
    std::vector<JointPartitions> joint_partitions;
};

}

#endif
