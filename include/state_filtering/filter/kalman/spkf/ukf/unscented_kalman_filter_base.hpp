

#ifndef FILTERS_SPKF_UKF_UNSCENTED_KALMAN_FILTER_BASE_HPP
#define FILTERS_SPKF_UKF_UNSCENTED_KALMAN_FILTER_BASE_HPP

#include <Eigen/Eigen>

#include <boost/shared_ptr.hpp>

#include <state_filtering/filter/kalman/spkf/sigma_point_kalman_filter.hpp>
#include <state_filtering/filter/kalman/spkf/ukf/unscented_transform.hpp>

namespace filter
{
    class UkfInternalsBase:
            public SpkfInternals
    {
    public:
        typedef boost::shared_ptr<UkfInternalsBase> Ptr;

    public: /* SpkfInternals interface */

        /**
         * @see SpkfInternals::
         */
        virtual void onBeginPredict(const DistributionDescriptor &stateDesc,
                                    DistributionDescriptor& predictedStateDesc);

        /**
         * @see SpkfInternals::
         */
        virtual void process(DistributionDescriptor& currentStateDesc,
                             const ProcessModel::Control& controlInput,
                             const double deltaTime,
                             DistributionDescriptor& predictedStateDesc);

        /**
         * @see SpkfInternals::
         */
        virtual void onFinalizePredict(const DistributionDescriptor &stateDesc,
                                       DistributionDescriptor& predictedStateDesc);

        /**
         * @see UkfInternals::createDistributionDescriptor()
         */
        virtual DistributionDescriptor::Ptr createDistributionDescriptor();

    public:/* UKF internal specifics */
        /**
         *
         */
        virtual void augmentFilteringState(const DistributionDescriptor &stateDesc,
                                           DistributionDescriptor &augmentedStateDesc);

        /**
         *
         */
        virtual void process(const SigmaPointMatrix& sigmaPoints,
                             const ProcessModel::Control& control,
                             const double deltaTime,
                             SigmaPointMatrix& processedSigmaPoints,
                             int stateDimension);

        /**
         *
         */
        virtual void predictMeasurement(const SigmaPointMatrix& predictedStateSigmaPoints,
                                        SigmaPointMatrix& measurementSigmaPoints);
    };
}

#endif
