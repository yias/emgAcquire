
/**
 * This script gets the EMG signal from the emgAcquire server 
 * and publish them to a ros topic
 *
 */


#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64MultiArray.h"
#include "std_msgs/MultiArrayDimension.h"

#include "emgAcquire.h"

#include <vector>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{

    // define the IP of the machine that runs the emgAcquire (server). In this case, the server would be a local server.
    std::string svrIP = "128.179.140.26"; // "128.179.140.26"

    // define port for the communication, 
    int svrPort = 10352; // this is the default port. The same port should be in both ends of the communication (server and client)

    // define the frequency of receiving the data
    float freq = 20;

    // define the number of EMG channels to receive
    float nb_ch = 16;

    emgAcquire::Client emgListener(freq, nb_ch);

    // set the server IP on the emgAcquire 
    emgListener.setServerIP(svrIP);

    // initialize the emgAcquire client
    if(emgListener.initialize()<0){
        std::cout << "unable to initialize" << std::endl;
        return -1;
    }


    // a 2D matrix of doubles to store the received data
    std::vector< std::vector<double> > mat_double;
  
    // define ros node
    ros::init(argc, argv, "emgAcquire_node");
    
    ros::NodeHandle n;

    // define ros publishers
    ros::Publisher emgAcquire_pub = n.advertise<std_msgs::Float64MultiArray>("emgAcquire/Data", 2);      // for publishing the contains of a field as a matrix

    // define ros message

    // multi-array
    std_msgs::Float64MultiArray mArray_msg;

    // set-up the layout of the message and the labels of the dimensions 
    mArray_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
    mArray_msg.layout.dim[0].label = "channels";

    mArray_msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
    mArray_msg.layout.dim[1].label = "samples";

    // helper vector for serialization
    std::vector<double> helper_vec;

    // set the frequncy of the ros loop to 20Hz
    ros::Rate loop_rate(freq);

    // start the acquisition of the signal
    emgListener.start();

    while (ros::ok())
    {
        
        // retrieve the acquired signals
        mat_double = emgListener.getSignals();

        // set-up the size and stride of each dimension
        mArray_msg.layout.dim[0].size = mat_double.size();
        mArray_msg.layout.dim[0].stride = mat_double.size()*mat_double[0].size(); // assuming that all the rows of a vector is of equal length
        mArray_msg.layout.dim[1].size = mat_double[0].size();
        mArray_msg.layout.dim[1].stride = mat_double[0].size();

        // reshape the received data matrix row-wise (row-major)
        helper_vec.resize(mat_double.size()*mat_double[0].size());
        for(int i = 0; i < (int)mat_double.size(); i++){
            std::copy(mat_double[i].begin(), mat_double[i].end(), helper_vec.begin()+i*mat_double[i].size());
        }

        // place the data to the multi-array message
        mArray_msg.data = helper_vec;
                
        // publish the multi-array message
        emgAcquire_pub.publish(mArray_msg);

        // clear the helper vector
        helper_vec.clear();

        ros::spinOnce();

        loop_rate.sleep();
    }

    // shut-down the emgAcquire client
    emgListener.shutdown();


    return 0;
}