#pragma once

#include <ros/ros.h>
#include "std_msgs/String.h"

#include <dynamic_gap/config/DynamicGapConfig.h>
// #include <nav_msgs/Odometry.h>
#include <geometry_msgs/Pose.h>
// #include <geometry_msgs/Twist.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <unsupported/Eigen/MatrixFunctions>
#include <limits>
// #include <sensor_msgs/Imu.h>
// #include <tf2_ros/buffer.h>
// #include <sensor_msgs/LaserScan.h>
// #include <tf2_ros/transform_listener.h>
// #include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <Eigen/Core>
#include <Eigen/Dense>

#include <dynamic_gap/gap_estimation/Estimator.h>

// #include <random>


// using namespace Eigen;

namespace dynamic_gap 
{
    class PerfectEstimator : public Estimator 
    {
        private:            
            std::vector<geometry_msgs::Pose> agentPoses_; /**< poses of all agents in environment (ground truth information used for certain estimator classes */
            std::vector<geometry_msgs::Vector3Stamped> agentVels_; /**< velocities of all agents in environment (ground truth information used for certain estimator classes) */

        public:

            PerfectEstimator();

            void initialize(const std::string & side, const int & modelID, 
                            const float & gapPtX, const float & gapPtY,
                            const ros::Time & t_update, const geometry_msgs::TwistStamped & lastRbtVel,
                            const geometry_msgs::TwistStamped & lastRbtAcc);

            void transfer(const Estimator & incomingModel);

            void update(const Eigen::Vector2f & measurement, 
                        const std::vector<geometry_msgs::TwistStamped> & intermediateRbtVels, 
                        const std::vector<geometry_msgs::TwistStamped> & intermediateRbtAccs, 
                        const std::vector<geometry_msgs::Pose> & agentPoses,
                        const std::vector<geometry_msgs::Vector3Stamped> & agentVels,
                        const ros::Time & tUpdate);
 
            /**
            * \brief use ground truth information on agent poses and velocities to perfectly update model state

            * \return updated model state
            */
            Eigen::Vector4f updateStateFromEnv();
            // Eigen::Vector4f getState();
            // Eigen::Vector4f getTrueState();

            // Eigen::Vector4f getGapState();
            // Eigen::Vector4f getRewindGapState();
            // Eigen::Vector4f get_modified_polar_state();
            // Eigen::Vector4f get_frozen_modified_polar_state();
            // Eigen::Vector4f get_rewind_modified_polar_state();

            // Eigen::Vector2f getXTilde();

            // geometry_msgs::TwistStamped getRobotVel();
            // Eigen::Vector4f integrate();
            // void linearize(const int & idx);
            // void discretizeQ(const int & idx);

            // void gapStatePropagate(const float & dt);
            // void rewindPropagate(const float & dt);
            // void isolateGapDynamics();
            // void setRewindState();
            // int getID();

    };
}