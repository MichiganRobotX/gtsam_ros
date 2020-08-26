/* ----------------------------------------------------------------------------
 * Copyright 2020, Li Chen <ilnehc@umich.com>
 * All Rights Reserved
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 *  @file   gtsam_core.h
 *  @author Li Chen
 *  @brief  Header file for the core of the GTSAM
 *  @date   August 19, 2020
 **/

#ifndef GTSAM_CORE_H
#define GTSAM_CORE_H
#include <memory>
#include <chrono>
#include <string>
#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#if GTSAM_USE_MUTEX
#include <mutex>
#endif
#include <algorithm>

#include <gtsam/geometry/Rot3.h>
#include <gtsam/geometry/Pose3.h>
#include <gtsam/slam/PriorFactor.h>
#include <gtsam/slam/BetweenFactor.h>
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/nonlinear/GaussNewtonOptimizer.h>
#include <gtsam/nonlinear/LevenbergMarquardtOptimizer.h>
#include <gtsam/nonlinear/Marginals.h>
#include <gtsam/nonlinear/Values.h>
#include <gtsam/nonlinear/ISAM2.h>
#include <gtsam/navigation/CombinedImuFactor.h>
#include <gtsam/navigation/GPSFactor.h>
#include <gtsam/navigation/ImuFactor.h>
#include <gtsam/inference/Symbol.h>

#include "Measurement.h"

using namespace gtsam;
using namespace std;
using symbol_shorthand::B;  // Bias  (ax,ay,az,gx,gy,gz)
using symbol_shorthand::V;  // Vel   (xdot,ydot,zdot)
using symbol_shorthand::X;  // Pose3 (x,y,z,r,p,y)

namespace GTSAM {

struct RobotxCalibration {
    double body_ptx;
    double body_pty;
    double body_ptz;
    double body_prx;
    double body_pry;
    double body_prz;
    double accelerometer_sigma;
    double gyroscope_sigma;
    double integration_sigma;
    double accelerometer_bias_sigma;
    double gyroscope_bias_sigma;
    double average_delta_t;
  
  	RobotxCalibration(const Vector12& calibration) { 
      	body_ptx = calibration(0);
    	body_pty = calibration(1);
    	body_ptz = calibration(2);
    	body_prx = calibration(3);
    	body_pry = calibration(4);
    	body_prz = calibration(5);
    	accelerometer_sigma = calibration(6);
    	gyroscope_sigma = calibration(7);
    	integration_sigma = calibration(8);
    	accelerometer_bias_sigma = calibration(9);
    	gyroscope_bias_sigma = calibration(10);
    	average_delta_t = calibration(11);
	}
};

class GTSAM_CORE {
public:
    void initialzie(Vector12& calibration, Vector3& position);
    Pose3 getCurPose();
  	Values getResult();
    Matrix getMarginalPoseCov();

    // x, y, z
  	void addGPS(shared_ptr<PoseMeasurement> ptr);	
  	// quaternion(qx,qy,qz,qw), anglular velocity(omegax, omegay, omegaz), linear acceleration(accx, accy, accz)
    void addIMU(shared_ptr<ImuMeasurement> ptr);

private:
    std::shared_ptr<PreintegratedImuMeasurements> current_summarized_measurement;
	double t1;
  	double t2;
  	double dt;
    size_t included_imu_measurement_count;
  	double g_;
    uint64_t GPS_update_count;
  	int gps_skip;

    ISAM2Params imu_params;
    noiseModel noise_model_gps;
	imuBias::ConstantBias current_bias;
    Pose3 current_pose_global;
    Matrix current_pose_cov;
    Vector3 current_velocity_global;
    RobotxCalibration robotx_calibration;

    ISAM2 isam2;
	Values result;  
    // Create the factor graph and values object that will store new factors and values to add to the incremental graph
    NonlinearFactorGraph new_factors;
    Values new_values;
};

}   // end of namespace GTSAM

#endif 