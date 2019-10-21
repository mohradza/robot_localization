/**\
 * @file x_vel_step_test.cpp\
 * @brief Adaptation of the offb_node.cpp script. Commands the\
 *        vehicle to a takeoff position, then a maneuver start position,\
 *        then it commands a step velocity input, halts, and returns to land.\
 */

#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <math.h>
#include <nav_msgs/Odometry.h>
#include <tf/tf.h>

// Vins Position Callback
nav_msgs::Odometry vins_odom;
void vins_odom_cb(const nav_msgs::Odometry::ConstPtr& msg){
    vins_odom.pose.pose.position.x = msg->pose.pose.position.x;
    vins_odom.pose.pose.position.y = msg->pose.pose.position.y;
    vins_odom.pose.pose.position.z = msg->pose.pose.position.z;
    vins_odom.twist.twist.linear.x = msg->twist.twist.linear.x;
    vins_odom.twist.twist.linear.y = msg->twist.twist.linear.y;
    vins_odom.twist.twist.linear.z = msg->twist.twist.linear.z;
    vins_odom.twist.twist.angular.x = msg->twist.twist.angular.x;
    vins_odom.twist.twist.angular.y = msg->twist.twist.angular.y;
    vins_odom.twist.twist.angular.z = msg->twist.twist.angular.z;
}

// ROSFlight attitude callback
void rosflight_cb(const rosflight_msgs::Attitude::ConstPtr& msg){
    vins_odom.pose.pose.orientation = msg->attitude;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "offb_node");
    ros::NodeHandle nh;

    // Subscribers
    ros::Subscriber vins_odom_sub = nh.subscribe<nav_msgs::Odometry>
            ("/vins_estimator/odometry", 10, vins_odom_cb);
    ros::Subscriber rosflight_attitude_sub = nh.subscribe<rosflight_msgs::Attitude>
            ("/attitude", 10, rosflight_cb);

    // Publishers
    ros::Publisher odom_pub = nh.advertise<nav_msgs::Odometry>
            ("vins_odom", 10);

    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(50);

    // Start timer and choose initial setpoint typ
    ros::Time last_request = ros::Time::now();

    vins_odom.header.frame_id = "world";

    // MAIN CONTROL LOOP
    while(ros::ok()){

        vins_odom.header.stamp = ros::Time::now();
	odom_pub.publish(vins_odom);

        ros::spinOnce();
        rate.sleep();
   }

    return 0;
}
