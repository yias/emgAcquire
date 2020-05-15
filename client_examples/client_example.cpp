/**
 * 
 *  Examples on how to use the socketStream class for implementing 
 *  a TCP client and listen to server
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  
 *  
*/

// #include "socketStream.h"
// #include "jsonWrapper.hpp"
#include "emgAcquire.h"
#include <chrono>
#include <ctime>

int main(int argc, char **argv){

    // define the variable that holds the server IP. In this case, the server would be a local server.
    const char *srvIP = "128.179.140.26";

    // if no new server IP is defined from the user, continue with the pre-defined server IP (localhost)
    if(argc!=2){
        std::cout << "No server IP suplied. Continue with localhost" << std::endl;
    }else{
        srvIP=argv[1];
    }

    // create an sockectStream object with the selected server IP address 
    socketStream socketHdlr(srvIP);

    // set the size of the buffer
    // socketHdlr.setBufferSize(64);

    

    // initialize the socket
    if(socketHdlr.initialize_socketStream()<0){
        std::cerr << "Unable to initialize socket" << std::endl;
        return -4;
    }

    socketHdlr.set_clientName("listener");

    // attemp a connection with the server
    if(socketHdlr.make_connection()<0){
        std::cerr << "Unable to connect to " << srvIP << std::endl;
        return -5;
    }

    // auto start = std::chrono::steady_clock::now();
    // auto end = std::chrono::steady_clock::now();

    // double time2run = 120000;

    std::chrono::milliseconds timespan(10);

    // a 2D matrix of doubles to store the received data
    std::vector< std::vector<double> > mat_double;

    //  define a boolean variable for checking if the message is new or not
    bool isNew = false;

    std::string msg;
    
    while(true){
        // send the message to the server
        if(socketHdlr.socketStream_ok()){
            msg = socketHdlr.get_latest(&isNew);
            // std::cout << "test\n";
            if(isNew){
                // std::cout << "main\n";
                // std::cout << msg << std::endl;
                
                // if the message is new:
                // parse the json string in a json document
                jsonWrapper testObj(msg);

                // get the contents of the field "data"
                mat_double = testObj.getField<rapidJson_types::Mat2DD>(std::string("data"));
                std::cout << "size of the matrix: " << mat_double.size() << ", " << mat_double[0].size() << std::endl;

                
            }            
        }


        if(kbhit()){
            #ifdef _WIN32
                if(getch()=='q')
                    break;
            #endif
            #ifdef __linux__
                if(getch()=='q')
                    break;
            #endif
        }
        // end = std::chrono::steady_clock::now();
        // std::this_thread::sleep_for(timespan);
    }
    

    // close communication with the server
    socketHdlr.closeCommunication();

    return 0;
}