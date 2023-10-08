#pragma once

#include <ros/ros.h>
#include <math.h>
#include <dynamic_gap/utils/Gap.h>
#include <dynamic_gap/utils/Utils.h>
#include <dynamic_gap/config/DynamicGapConfig.h>
#include <dynamic_gap/trajectory_scoring/TrajectoryScorer.h>
#include <vector>
#include <geometry_msgs/PoseStamped.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <sensor_msgs/LaserScan.h>
#include <boost/shared_ptr.hpp>

namespace dynamic_gap 
{
    class GapManipulator 
    {
        public: 
            GapManipulator(ros::NodeHandle& nh, const dynamic_gap::DynamicGapConfig& cfg) {cfg_ = &cfg;};
            GapManipulator& operator=(GapManipulator & other) {cfg_ = other.cfg_; return *this; };
            GapManipulator(const GapManipulator &t) {cfg_ = t.cfg_;};

            void updateEgoCircle(boost::shared_ptr<sensor_msgs::LaserScan const>);
            void updateStaticEgoCircle(const sensor_msgs::LaserScan &);
            void updateDynamicEgoCircle(dynamic_gap::Gap&,
                                        const std::vector<sensor_msgs::LaserScan> &);

            void setGapWaypoint(dynamic_gap::Gap& gap, geometry_msgs::PoseStamped localgoal, bool initial); //, sensor_msgs::LaserScan const dynamic_laser_scan);
            void setTerminalGapWaypoint(dynamic_gap::Gap& gap, geometry_msgs::PoseStamped localgoal);
            
            void reduceGap(dynamic_gap::Gap&, geometry_msgs::PoseStamped, bool); //), sensor_msgs::LaserScan const);
            void convertRadialGap(dynamic_gap::Gap&, bool); //, sensor_msgs::LaserScan const);
            void radialExtendGap(dynamic_gap::Gap&, bool); //, sensor_msgs::LaserScan const);
            void inflateGapSides(dynamic_gap::Gap& gap, bool initial);

        private:
            boost::shared_ptr<sensor_msgs::LaserScan const> msg;
            sensor_msgs::LaserScan static_scan, dynamic_scan;
            const DynamicGapConfig* cfg_;
            int num_of_scan;
            int half_num_scan;
            double angle_min;
            double angle_increment; 
            boost::mutex egolock;

            bool checkGoalVisibility(geometry_msgs::PoseStamped, float theta_r, float theta_l, float rdist, float ldist, sensor_msgs::LaserScan const scan);
    };
}