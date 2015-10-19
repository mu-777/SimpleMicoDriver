//
// Created by ryosuke on 15/10/19.
//

#include "ros/ros.h"
#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include "Kinova/KinovaTypes.h"
#include "Kinova/Kinova.API.CommLayerUbuntu.h"
#include "Kinova/Kinova.API.UsbCommandLayerUbuntu.h"
#include "simple_mico_driver/JacoAPI.h"
#include <jaco_msgs/JointVelocity.h>

class SimpleMicoDriver {
public:
    SimpleMicoDriver(const ros::NodeHandle &nodeHandle) : node_handle_(nodeHandle) {
        int result = jacoAPI_.MyInitAPI();
        std::cout << "Initialization's result :" << result << std::endl;
        KinovaDevice list[MAX_KINOVA_DEVICE];
        int devicesCount = jacoAPI_.MyGetDevices(list, result);
        isConnected = (devicesCount == 1);
        pointToSend_.InitStruct();
        pointToSend_.Position.Type = ANGULAR_VELOCITY;
        joint_velocities_.InitStruct();
    };

    ~SimpleMicoDriver() {
        int result = jacoAPI_.MyCloseAPI();
        std::cout << "Close's result :" << result << std::endl;
    };

    void moveHome() {
        jacoAPI_.MyMoveHome();
    };

    void jointVelCallback(const jaco_msgs::JointVelocityConstPtr &joint_vel) {
        std::cout << "called!" << std::endl;
        pointToSend_.Position.Actuators.Actuator1 = joint_vel->joint1;
        pointToSend_.Position.Actuators.Actuator2 = joint_vel->joint2;
        pointToSend_.Position.Actuators.Actuator3 = joint_vel->joint3;
        pointToSend_.Position.Actuators.Actuator4 = joint_vel->joint4;
        pointToSend_.Position.Actuators.Actuator5 = joint_vel->joint5;
        pointToSend_.Position.Actuators.Actuator6 = joint_vel->joint6;
    };

    void activate() {
        subJointVelInput_ = node_handle_.subscribe("/mico_driver_good/in/joint_velocity", 10,
                                                   &SimpleMicoDriver::jointVelCallback, this);
    };

    void sendCommand() {
        std::cout << "RUN" << std::endl;
        ROS_INFO("Joint vel : %f, %f, %f, %f, %f, %f",
                 pointToSend_.Position.Actuators.Actuator1, pointToSend_.Position.Actuators.Actuator2,
                 pointToSend_.Position.Actuators.Actuator3, pointToSend_.Position.Actuators.Actuator4,
                 pointToSend_.Position.Actuators.Actuator5, pointToSend_.Position.Actuators.Actuator6);
        jacoAPI_.MySendBasicTrajectory(pointToSend_);
    }

    bool isConnected;

private:
    JacoAPI jacoAPI_;
    TrajectoryPoint pointToSend_;
    AngularInfo joint_velocities_;
    ros::NodeHandle node_handle_;
    ros::Subscriber subJointVelInput_;
};


int main(int argc, char **argv) {
    ros::init(argc, argv, "mico_driver_good");
    ros::NodeHandle nh("~");
    ros::Rate rate(100);

    SimpleMicoDriver driver(nh);
    if (driver.isConnected) {
        driver.moveHome();
        driver.activate();
        while (ros::ok()) {
            driver.sendCommand();
            ros::spinOnce();
            rate.sleep();
        }
    }

    return 0;
}
