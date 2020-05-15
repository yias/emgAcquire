/** 
 *  Copyright (C) 2020 Iason Batzianoulis
 *  
 *  Acquisition source file
 *  
 * 
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    GNU GPLv3
 * 
**/

#include "socketStream.h"

namespace SOCKETSTREAM{
    const char *DEFAULT_HOST_IP = "localhost"; 
}

#ifdef __linux__



int kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}


#endif



socketStream::socketStream(void){

    mode = SOCKETSTREAM::SOCKETSTREAM_CLIENT;

    nb_connections = 10;

    ConnectSocket = INVALID_SOCKET;

    ListenSocket = INVALID_SOCKET;                      // for the server

    result = NULL;
    
    ptr = NULL;

    isServer = false;
    serverRunning = false;

    msg_idf = "!&?5";

    endMSG = "!3tt";

    ec_id = "\ne@c";

    headerSize = 8;

    bufferSize = 16;

    msgOverhead = 0;

    msgHeader = std::string(headerSize, ' ');

    msgOHstring = std::string(std::to_string(bufferSize).length(), ' ');

    clientName = randomString(5);

    Host_IP = SOCKETSTREAM::DEFAULT_HOST_IP;

    Host_Port = SOCKETSTREAM::DEFAULT_PORT;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    isComActive = false;

    msgInitilized = false;

    useHashKey = true;

    verbose = true;

    minMsgSize = std::strlen(msg_idf) + headerSize + std::strlen(endMSG) + std::to_string(bufferSize).length() +1;

    std::cout << "[socketStream] Starting socketStream with default parameters" << std::endl;
    std::cout << "[socketStream] IP address: " << Host_IP << ", Port: " << Host_Port << std::endl;
    std::cout << "[socketStream] Client mode" << std::endl;
}


socketStream::socketStream(const char* svrIPAddress){

    nb_connections = 10;

    mode = SOCKETSTREAM::SOCKETSTREAM_CLIENT;

    ConnectSocket = INVALID_SOCKET;

    result = NULL;
    
    ptr = NULL;
    
    isServer = false;

    msg_idf = "!&?5";

    endMSG = "!3tt";

    ec_id = "\ne@c";

    headerSize = 8;

    bufferSize=16;

    msgOverhead = 0;

    msgHeader = std::string(headerSize, ' ');

    msgOHstring = std::string(std::to_string(bufferSize).length(), ' ');

    clientName = randomString(5);

    Host_IP = svrIPAddress;

    Host_Port = SOCKETSTREAM::DEFAULT_PORT;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    isComActive = false;

    msgInitilized = false;

    useHashKey = true;

    verbose = true;

    minMsgSize = std::strlen(msg_idf) + headerSize + std::strlen(endMSG) + std::to_string(bufferSize).length() + 1;

    std::cout << "[socketStream] Starting up socketStream on server address " << Host_IP << " and default port " << Host_Port << std::endl;
    std::cout << "[socketStream] Client mode" << std::endl;
}


socketStream::socketStream(const char* svrIPAddress, int srvPosrt, const int socketStreamMode){

    nb_connections = 10;

    mode = socketStreamMode;

    ConnectSocket = INVALID_SOCKET;

    result = NULL;
    
    ptr = NULL;

    if(socketStreamMode>0){
        isServer = true;
    }else{
        isServer = false;
    }

    msg_idf = "!&?5";

    endMSG = "!3tt";

    ec_id = "\ne@c";

    headerSize=8;

    bufferSize=16;

    msgOverhead = 0;

    msgHeader = std::string(headerSize, ' ');

    msgOHstring = std::string(std::to_string(bufferSize).length(), ' ');

    clientName = randomString(5);

    Host_IP = svrIPAddress;

    Host_Port = (unsigned int)srvPosrt;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if(isServer){
        hints.ai_flags = AI_PASSIVE;                   // for the server
        serverRunning = false;
        clientsSockets = std::vector <SOCKET> (nb_connections);
        clientsAddresses = std::vector <std::string> (nb_connections);
        firstMsgReceived = std::vector <bool> (nb_connections, false);
        isNewMsgReceived = std::vector <bool> (nb_connections, false);
        connetionSlots = std::vector <bool> (nb_connections, false);
        warningsWatcher = std::vector <bool> (nb_connections, false);
        clientIDs = std::vector <std::string> (nb_connections);
        clientMsgs = std::vector <std::string> (nb_connections);
        connectionThreads = std::vector <std::thread> (nb_connections);
        giveWarning = false;
    } 

    isComActive = false;

    msgInitilized = false;

    useHashKey = true;

    verbose = true;

    minMsgSize = std::strlen(msg_idf) + headerSize + std::strlen(endMSG) + std::to_string(bufferSize).length()+1;

    std::cout << "[socketStream] Starting socketStream on server address " << Host_IP << " and port " << Host_Port << std::endl;

    if(isServer){
        std::cout << "[socketStream] Server mode" << std::endl;
    }else{
        std::cout << "[socketStream] Client mode" << std::endl;
    }
}

int socketStream::initialize_socketStream(){

    #ifdef _WIN32
        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            std::cerr << "[socketStream] WSAStartup failed with error: " << iResult << std::endl;
            return iResult;
        }
    #endif

    std::ostringstream i2s;
    i2s << Host_Port;

    // Resolve the server address and port
    iResult = getaddrinfo(Host_IP, i2s.str().c_str(), &hints, &result);
    if ( iResult != 0 ) {
        std::cerr << "[socketStream] Getaddrinfo failed with error: " << iResult << std::endl;
        #ifdef _WIN32
            WSACleanup();
        #endif
        return iResult;
    }

    if(isServer){
        // Create a SOCKET for connecting to server
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            std::cerr << "[socketStream] Socket failed with error:";
            #ifdef _WIN32
                std::cerr << WSAGetLastError(); ;
                WSACleanup();
            #endif
            std::cerr << std::endl;
            freeaddrinfo(result);
            return -3;
        }

        // Setup the TCP listening socket
        iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "[socketStream] Bind failed with error:";
            #ifdef _WIN32
                std::cerr << WSAGetLastError();
                WSACleanup();
            #else
                std::cerr << strerror(errno);
            #endif
            std::cerr << std::endl;
            #ifdef _WIN32
                closesocket(ListenSocket);
            #else
                close(ListenSocket);
            #endif
            freeaddrinfo(result);
            return -4;
        }

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "[socketStream] Listen failed with error: "; 
            #ifdef _WIN32
                std::cerr << WSAGetLastError();
                WSACleanup();
            #endif
            std::cerr << std::endl;
            #ifdef _WIN32
                closesocket(ListenSocket);
            #else
                close(ListenSocket);
            #endif
            return -2;
        }

        // int val = 5;

        // unsigned int sz=sizeof(val);

        // struct timeval timeout;
        // timeout.tv_sec = 5;
        // timeout.tv_usec = 0;

        // iResult = setsockopt(ListenSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
        // if (iResult == SOCKET_ERROR) {
        //     std::cerr<< "[socketStream] setsockopt for SO_RCVTIMEO failed with error:";
        //     #ifdef _WIN32
        //         std::cerr << WSAGetLastError();
        //     #endif
        //     std::cout << std::endl;
        // } else
        //     std::cout << "[sosketStream] timeout has been set for " << timeout.tv_sec << "." << timeout.tv_usec/1000<< " s" << std::endl;


        serverRunning = true;

        freeaddrinfo(result);
    }

    return 0;

}


int socketStream::initialize_socketStream(const char* svrIPAddress, int srvPosrt){
    
    // setting the server IP address and port
    Host_IP = svrIPAddress;

    Host_Port = (unsigned int)srvPosrt;

    #ifdef _WIN32
        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            std::cerr << "[socketStream] WSAStartup failed with error: " << iResult << std::endl;
            return iResult;
        }
    #endif

    std::ostringstream i2s;
    i2s << Host_Port;

    // Resolve the server address and port
    iResult = getaddrinfo(Host_IP, i2s.str().c_str(), &hints, &result);
    if ( iResult != 0 ) {
        std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;;
        #ifdef _WIN32
            WSACleanup();
        #endif
        return iResult;
    }

    if(isServer){
        // Create a SOCKET for connecting to server
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            std::cerr << "[socketStream] Socket failed with error:";
            #ifdef _WIN32
                std::cerr << WSAGetLastError(); ;
                WSACleanup();
            #endif
            std::cerr << std::endl;
            freeaddrinfo(result);
            return -3;
        }

        // Setup the TCP listening socket
        iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "[socketStream] Bind failed with error:";
            #ifdef _WIN32
                std::cerr << WSAGetLastError();
                WSACleanup();
            #endif
            std::cerr << std::endl;
            #ifdef _WIN32
                closesocket(ListenSocket);
            #else
                close(ListenSocket);
            #endif
            freeaddrinfo(result);
            return -4;
        }

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "[socketStream] Listen failed with error: "; 
            #ifdef _WIN32
                std::cerr << WSAGetLastError();
                WSACleanup();
            #endif
            std::cerr << std::endl;
            #ifdef _WIN32
                closesocket(ListenSocket);
            #else
                close(ListenSocket);
            #endif
            return -2;
        }

        // struct timeval timeout;
        // timeout.tv_sec = 2;
        // timeout.tv_usec = 0;

        // // int val = 5;

        // // unsigned int sz=sizeof(val);

        // iResult = setsockopt(ListenSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
        // if (iResult == SOCKET_ERROR) {
        //     std::cerr<< "[socketStream] setsockopt for SO_RCVTIMEO failed with error:";
        //     #ifdef _WIN32
        //         std::cerr << WSAGetLastError();
        //     #endif
        //     std::cout << std::endl;
        // } else
        //     std::cout << "[sosketStream] timeout has been set for " << timeout.tv_sec << "." << timeout.tv_usec/1000<< " s" << std::endl;

        serverRunning = true;

        freeaddrinfo(result);
    }

    return 0;

}

int socketStream::make_connection(){

    std::cout << "[socketStream] Attempting to connect to server with address " << Host_IP << " in the port " << Host_Port << std::endl;

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cerr << "[socketStream] socket failed with error: "; 
            #ifdef _WIN32
                std::cerr <<  WSAGetLastError();
                WSACleanup();
            #else
                std::cerr << strerror(errno);
            #endif
            std::cerr << std::endl;
            return -1;
        }

        
        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            #ifdef _WIN32
                closesocket(ConnectSocket);
            #else
                close(ConnectSocket);
            #endif
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "[socketStream] Unable to connect to server!" << std::endl;
        #ifdef _WIN32
            WSACleanup();
        #endif
        return -2;
    }

    bool validConnection = handshake_server(10);
    if(!validConnection){
        std::cout << "[socketStream] Handshake protocol failed! Closing connection with the server" << std::endl;
        #ifdef _WIN32
            closesocket(ConnectSocket);
        #else
            close(ConnectSocket);
        #endif
        ConnectSocket = INVALID_SOCKET;
        #ifdef _WIN32
            WSACleanup();
        #endif
        return -3;
    }

    if(clientsSockets.size()<1){
        clientsAddresses.push_back(std::string(Host_IP));
        firstMsgReceived.push_back(false);
        isNewMsgReceived.push_back(false);
        clientsSockets.push_back(ConnectSocket);
        connetionSlots.push_back(true);
        clientMsgs.push_back(std::string(""));
        connectionThreads.push_back(std::thread(&socketStream::runReceiver, this, 0));
        // connectionThreads.back().detach();
        
    }else{
        clientsAddresses[0] = std::string(Host_IP);
        firstMsgReceived[0] = false;
        isNewMsgReceived[0] = false;
        clientsSockets[0] = ConnectSocket;
        connetionSlots[0] = true;
        clientMsgs[0] = std::string("");
        connectionThreads[0] = std::thread(&socketStream::runReceiver, this, 0);
        // connectionThreads[0].detach();

    }

    // std::thread receiverThread(&socketStream::runReceiver, this, 0);
    // receiverThread.detach();

    std::cout << "[socketStream] Connected to server " << Host_IP << " in port " << Host_Port << std::endl;

    isComActive = true;

    return 0;

}


int socketStream::setBufferSize(unsigned int newBufferSize){

    bufferSize = newBufferSize;

    msgOHstring = std::string(std::to_string(bufferSize).length(), ' ');

    minMsgSize = std::strlen(msg_idf) + headerSize + std::strlen(endMSG) + std::to_string(bufferSize).length()+1;

    std::cout << "[socketStream] Buffer size is set to " << bufferSize << std::endl;

    return 0;
}


int socketStream::setNumberOfConnections(unsigned int nbConnections){

    if (nbConnections>SOCKETSTREAM::MAX_NB_CONNECTIONS){
        std::cout << "[socketStream] The selected number of connections exceeds the maximum number of connections (" << SOCKETSTREAM::MAX_NB_CONNECTIONS << ")" << std::endl;
        std::cout << "[socketStream] Continue with the default number of connections (" << nb_connections << ")" << std::endl;
        return -1;
    }else{
        nb_connections = nbConnections;
        clientsSockets = std::vector <SOCKET> (nb_connections);
        clientsAddresses = std::vector <std::string> (nb_connections);
        firstMsgReceived = std::vector <bool> (nb_connections, false);
        isNewMsgReceived = std::vector <bool> (nb_connections, false);
        connetionSlots = std::vector <bool> (nb_connections, false);
        warningsWatcher = std::vector <bool> (nb_connections, false);
        clientIDs = std::vector <std::string> (nb_connections);
        clientMsgs = std::vector <std::string> (nb_connections);
        connectionThreads = std::vector <std::thread> (nb_connections);
        std::cout << "[socketStream] Number of connections is set to " << nb_connections << std::endl;
        return 0;
    }

    
}

int socketStream::set_clientName(std::string cID){

    clientName = cID;

    return 0;
}

int socketStream::initialize_msgStruct(std::vector <std::string> fields){
    
    // declare a string object to throw the fields
    std::string initial_rjson_obj;

    // construct the string with a proper syntax
    initial_rjson_obj += "{\"";

    for (unsigned int i=0; i< fields.size(); i++){
        initial_rjson_obj += fields[i];
        initial_rjson_obj += "\" : null, \"";
    }

    initial_rjson_obj.erase(initial_rjson_obj.length()-3);
    initial_rjson_obj += "}";

    // check if all the fields are 
    if(dDoc.Parse(initial_rjson_obj.c_str()).HasParseError()){
        return -1;
    }

    // check if json object is properly created
    assert(dDoc.IsObject());

    // check if a fields are properly defined
    for(unsigned int i = 0; i < fields.size(); i++ ){
        if(!dDoc.HasMember(fields[i].c_str())){
            std::cout << "dDoc doesn't have a member " << fields[i] << std::endl;
        }
    }

    std::cout << "[socketStream] The message struct initialized successfully" << std::endl;

    msgInitilized = true;

    return 0;
}


int socketStream::updateMSG(std::string field, const char *value){

    if(msgInitilized){
        if(dDoc.HasMember(field.c_str())){
            dDoc[field.c_str()].SetString(value, strlen(value));
        }else{
            std::cerr << "[socketStream] Not valid field name" << std::endl;
        }
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}

int socketStream::updateMSG(std::string field, int value){

    if(msgInitilized){
        if(dDoc.HasMember(field.c_str())){
            dDoc[field.c_str()].SetInt(value);
        }else{
            std::cerr << "[socketStream] Not valid field name" << std::endl;
        }
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}


int socketStream::updateMSG(std::string field, double value){

    if(msgInitilized){
        if(dDoc.HasMember(field.c_str())){
            dDoc[field.c_str()].SetDouble(value);
        }else{
            std::cerr << "[socketStream] Not valid field name" << std::endl;
        }
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}


int socketStream::updateMSG(std::string field, int *value, int arraylength){


    if(msgInitilized){
        if(dDoc.HasMember(field.c_str())){
            rapidjson::Value t_a(rapidjson::kArrayType);
            rapidjson::Document::AllocatorType& allocator = dDoc.GetAllocator();
            for(int j= 0; j<arraylength; j++){
                t_a.PushBack(value[j],allocator);
            }
            if(!dDoc[field.c_str()].IsArray()){
                dDoc[field.c_str()].SetArray();
            }
            dDoc[field.c_str()]=t_a;
        }else{
            std::cerr << "[socketStream] Not valid field name" << std::endl;
        }
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}


int socketStream::updateMSG(std::string field, double *value, int arraylength){


    if(msgInitilized){
        if(dDoc.HasMember(field.c_str())){
            rapidjson::Value t_a(rapidjson::kArrayType);
            rapidjson::Document::AllocatorType& allocator = dDoc.GetAllocator();
            for(int j= 0; j<arraylength; j++){
                t_a.PushBack(value[j],allocator);
            }
            if(!dDoc[field.c_str()].IsArray()){
                dDoc[field.c_str()].SetArray();
            }
            dDoc[field.c_str()]=t_a;
        }else{
            std::cerr << "[socketStream] Not valid field name" << std::endl;
        }
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}


int socketStream::updateMSG(std::string field, std::vector <int> value){

    if(msgInitilized){
        if(dDoc.HasMember(field.c_str())){
            rapidjson::Value t_a(rapidjson::kArrayType);
            rapidjson::Document::AllocatorType& allocator = dDoc.GetAllocator();
            for(int j= 0; j<(int)value.size(); j++){
                t_a.PushBack(value[j],allocator);
            }
            if(!dDoc[field.c_str()].IsArray()){
                dDoc[field.c_str()].SetArray();
            }
            dDoc[field.c_str()]=t_a;
        }else{
            std::cerr << "[socketStream] Not valid field name" << std::endl;
        }
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}


int socketStream::updateMSG(std::string field, std::vector <double> value){

    if(msgInitilized){
        if(dDoc.HasMember(field.c_str())){
            rapidjson::Value t_a(rapidjson::kArrayType);
            rapidjson::Document::AllocatorType& allocator = dDoc.GetAllocator();
            for(int j= 0; j<(int)value.size(); j++){
                t_a.PushBack(value[j],allocator);
            }
            if(!dDoc[field.c_str()].IsArray()){
                dDoc[field.c_str()].SetArray();
            }
            dDoc[field.c_str()]=t_a;
        }else{
            std::cerr << "[socketStream] Not valid field name" << std::endl;
        }
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}


int socketStream::updateMSG(std::string field, std::vector <std::vector <int> > value){

    if(msgInitilized){
        if(dDoc.HasMember(field.c_str())){
            rapidjson::Value t_a(rapidjson::kArrayType);
            rapidjson::Document::AllocatorType& allocator = dDoc.GetAllocator();
            t_a.Reserve(value.size(),allocator);
            for(int j= 0; j<(int)value.size(); j++){
                rapidjson::Value t_b(rapidjson::kArrayType);
                t_b.Reserve(value[j].size(),allocator);
                for(int k=0; k<(int)value[j].size(); k++){
                    t_b.PushBack(value[j][k], allocator);
                }
                t_a.PushBack(t_b,allocator);
            }
            if(!dDoc[field.c_str()].IsArray()){
                dDoc[field.c_str()].SetArray();
            }
            dDoc[field.c_str()]=t_a;
        }else{
            std::cerr << "[socketStream] Not valid field name" << std::endl;
        }
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}


int socketStream::updateMSG(std::string field, std::vector <std::vector <double> > value){

    if(msgInitilized){
        if(dDoc.HasMember(field.c_str())){
            rapidjson::Value t_a(rapidjson::kArrayType);
            rapidjson::Document::AllocatorType& allocator = dDoc.GetAllocator();
            t_a.Reserve(value.size(),allocator);
            for(int j= 0; j<(int)value.size(); j++){
                rapidjson::Value t_b(rapidjson::kArrayType);
                t_b.Reserve(value[j].size(),allocator);
                for(int k=0; k<(int)value[j].size(); k++){
                    t_b.PushBack(value[j][k], allocator);
                }
                t_a.PushBack(t_b,allocator);
            }
            if(!dDoc[field.c_str()].IsArray()){
                dDoc[field.c_str()].SetArray();
            }
            dDoc[field.c_str()]=t_a;
        }else{
            std::cerr << "[socketStream] Not valid field name" << std::endl;
        }
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}

int socketStream::printArray(rapidjson::Value::ConstMemberIterator itr){

    
    std::cout << itr->name.GetString() << " = [";
    for(rapidjson::SizeType j =0 ; j<itr->value.Size(); j++){
        if(itr->value[j].IsArray()){
            std::cout << "[";
            for(rapidjson::SizeType k =0 ; k<itr->value[j].Size()-1; k++){
                if(itr->value[j][k].IsDouble()){
                    std::cout << itr->value[j][k].GetDouble() << ", ";
                }else{
                    if(itr->value[j][j].IsInt()){
                        std::cout << itr->value[j][k].GetInt() << ", ";
                    }else{
                        if(itr->value[j][j].IsUint()){
                            std::cout << itr->value[j][k].GetUint() << ", ";
                        }
                    }
                }
            }
            std::cout << itr->value[j][itr->value[j].Size()-1].GetDouble() << "]";
            if(j!=itr->value.Size()-1){
               std::cout << std::endl; 
            }
        }else{
            if(itr->value[j].IsDouble()){
                std::cout << itr->value[j].GetDouble();
            }else{
                if(itr->value[j].IsInt()){
                    std::cout << itr->value[j].GetInt();
                }else{
                     if(itr->value[j].IsUint()){
                         std::cout << itr->value[j].GetUint();
                     }
                }
            }
            if(j!=itr->value.Size()-1){
                std::cout << ", ";
            }
        }
    }
    std::cout << "]" << std::endl; 

    return 0;
}

int socketStream::printMSGcontents(){

    if(msgInitilized){
        for (rapidjson::Value::ConstMemberIterator itr = dDoc.MemberBegin(); itr != dDoc.MemberEnd(); ++itr){
            switch (itr->value.GetType())
            {
            case 0:
                std::cout << itr->name.GetString() << " = NULL" << std::endl; 
                break;
            case 1:
                {
                    std::cout << std::boolalpha;
                    std::cout << itr->name.GetString() << " = " << bool(itr->value.GetBool()) << std::endl; 
                }
                break;
            case 2:
                {   
                    std::cout << std::boolalpha;
                    std::cout << itr->name.GetString() << " = " << bool(itr->value.GetBool()) << std::endl; 
                }
                break;
            case 3:
                {
                    std::cout << itr->name.GetString() << " = [object]" << std::endl;  
                }
                break;
            case 4:
                {
                    printArray(itr);
                    // std::cout << itr->name.GetString() << " = [";
                    // for(rapidjson::SizeType j =0 ; j<itr->value.Size()-1; j++){
                    //     std::cout << itr->value[j].GetDouble() << ", ";
                    // }
                    // std::cout << itr->value[itr->value.Size()-1].GetDouble()<< "]" << std::endl;    
                }
                break; 
            case 5:
                {
                    std::cout << itr->name.GetString() << " = " << itr->value.GetString() << std::endl; 
                }
                break;
                
            case 6:
                {
                    if(itr->value.IsInt64()){
                        std::cout << itr->name.GetString() << " = " << itr->value.GetInt64() << std::endl;
                    }else{
                        if(itr->value.IsInt()){
                            std::cout << itr->name.GetString() << " = " << itr->value.GetInt() << std::endl;
                        }else{
                            if(itr->value.IsUint()){
                                std::cout << itr->name.GetString() << " = " << itr->value.GetUint() << std::endl;
                            }else{
                                if(itr->value.IsUint64()){
                                    std::cout << itr->name.GetString() << " = " << itr->value.GetUint64() << std::endl;
                                }else{
                                    if(itr->value.IsDouble()){
                                        std::cout << itr->name.GetString() << " = " << itr->value.GetDouble() << std::endl;
                                    }else{
                                        std::cout << "Unknown type of number" << std::endl;
                                    }
                                }
                            }

                        }
                        
                    }
                }
                break;
                
            default:
                {
                    std::cerr << "[socketStream] Unknown type of field" << std::endl;
                }
                break;                
            }
        }
    }

    return 0;
}


int socketStream::printMSGcontentsTypes(){

    // print the types of the message contains

    if(msgInitilized){
        static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
        std::cout<< "[socketStream] Message contens types:" << std::endl;
        for (rapidjson::Value::ConstMemberIterator itr = dDoc.MemberBegin(); itr != dDoc.MemberEnd(); ++itr)
            std::cout << itr->name.GetString() << ": [" << kTypeNames[itr->value.GetType()] << "]" << std::endl;
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }
    
    return 0;
}

int socketStream::printMSGString(){

    // print the message as a string

    if(msgInitilized){
        rapidjson::StringBuffer t_buffer;
        rapidjson::Writer<rapidjson::StringBuffer> t_writer(t_buffer);
        dDoc.Accept(t_writer);
        std::cout << t_buffer.GetString() << std::endl;
    }else{
        std::cerr << "[socketStream] The message struct is NOT initialized" << std::endl;
        return -1;
    }

    return 0;
}

int socketStream::sendMSg(){
    /** 
     * 
     * The function composes the message and sends it
     * 
     * Return:
     *      the number of bytes sent, if completed correctly
     *      the error type (negative integer), if not completed correctly
     * 
     */
    

    if(msgInitilized){

        // clear the buffers
        str_buffer.Clear();
        final_msg.clear();
        msg2send.clear();
        msgHeader = std::string(headerSize, ' ');

        // rapidjson::Writer<rapidjson::StringBuffer> writer(str_buffer);

        // get the json object in a string format
        writer.Reset(str_buffer);

        dDoc.Accept(writer);

        msg2send = str_buffer.GetString();

        // introduce the lenght of the string in the header of the file
        std::ostringstream i2s;
        i2s << msg2send.length();

        msgHeader.replace(msgHeader.begin(),msgHeader.end()+i2s.str().length()-msgHeader.length(),i2s.str());

        // if a checksum is needed, create and introduce it to the message
        if(useHashKey){
            std::string md5_key = md5(msg2send.c_str());
            msg2send += md5_key;
        }
        
        // compute the overhead of the message
        msgOverhead = (int((msg2send.length()+minMsgSize)/bufferSize)+1)*bufferSize - (msg2send.length()+minMsgSize);

        std::ostringstream i2s2;
        i2s2 << msgOverhead;        

        msgOHstring.replace(msgOHstring.begin(),msgOHstring.end()+i2s2.str().length()-msgOHstring.length(),i2s2.str());


        // compose the final message
        final_msg = msg_idf + msgHeader + std::to_string(int(useHashKey)) + msgOHstring + msg2send + std::string(msgOverhead, ' ') + endMSG;

        // send the message
        if(isComActive){
            // if the communication is active, send message
            iResult = send( ConnectSocket, final_msg.c_str(), (int)final_msg.size(), 0 );
            if (iResult == SOCKET_ERROR) {
                std::cerr << "[socketStream] Send message failed with error: ";
                #ifdef _WIN32
                    std::cerr << WSAGetLastError();
                #else
                    std::cerr << strerror(errno);
                #endif
                std::cerr << std::endl;
                #ifdef _WIN32
                    closesocket(ConnectSocket);
                #else
                    close(ConnectSocket);
                #endif
                #ifdef _WIN32
                    WSACleanup();
                #endif
                return -1;
            }
        }else{
            std::cerr << "[socketStream] The communication with the server is not open. " << std::endl;
            return -2;
        }

    }else{
        std::cerr << "[socketStream] The message struct id NOT initialized. " << std::endl;
        return -3;
    }

    // final_msg.clear();
    return iResult;
}

int socketStream::sendMSg2Client(unsigned int clID){
    /** 
     * 
     * The function composes the message and sends it to the client with id number clID
     * 
     * Return:
     *      the number of bytes sent, if completed correctly
     *      the error type (negative integer), if not completed correctly
     * 
     */
    

    if(msgInitilized){

        // clear the buffers
        str_buffer.Clear();
        final_msg.clear();
        msg2send.clear();
        msgHeader = std::string(headerSize, ' ');

        // rapidjson::Writer<rapidjson::StringBuffer> writer(str_buffer);

        // get the json object in a string format
        writer.Reset(str_buffer);

        dDoc.Accept(writer);

        msg2send = str_buffer.GetString();
        

        // introduce the lenght of the string in the header of the file
        std::ostringstream i2s;
        i2s << msg2send.length();

        msgHeader.replace(msgHeader.begin(),msgHeader.end()+i2s.str().length()-msgHeader.length(),i2s.str());

        // if a checksum is needed, create and introduce it to the message
        if(useHashKey){
            std::string md5_key = md5(msg2send.c_str());
            msg2send += md5_key;
        }
        
        // compute the overhead of the message
        msgOverhead = (int((msg2send.length()+minMsgSize)/bufferSize)+1)*bufferSize - (msg2send.length()+minMsgSize);

        std::ostringstream i2s2;
        i2s2 << msgOverhead;        

        msgOHstring.replace(msgOHstring.begin(),msgOHstring.end()+i2s2.str().length()-msgOHstring.length(),i2s2.str());


        // compose the final message
        final_msg = msg_idf + msgHeader + std::to_string(int(useHashKey)) + msgOHstring + msg2send + std::string(msgOverhead, ' ') + endMSG;
        // std::cout << final_msg << std::endl;

        // send the message
        if(connetionSlots[clID] && serverRunning){
            // if the communication is active, send message
            // threadMutex.lock();
            iResult = send( clientsSockets[clID], final_msg.c_str(), (int)final_msg.size(), 0 );
            // threadMutex.unlock();
            if (iResult == SOCKET_ERROR) {
                int error_NB = 0;
                #ifdef _WIN32
                    error_NB = WSAGetLastError();
                    if (connetionSlots[clID] && error_NB!=10058){
                        std::cerr << "[socketStream] Send message failed with error: " << error_NB << std::endl;
                        closesocket(clientsSockets[clID]);
                        
                        // #ifdef _WIN32
                        //     WSACleanup();
                        // #endif
                        return -1;
                    }
                #else
                    error_NB = errno;
                    if (connetionSlots[clID] && error_NB!=9){
                    std::cerr << "[socketStream] Send message failed with error: " << strerror(error_NB) << std::endl;
                    close(clientsSockets[clID]);
                    
                    return -1;
                }
                #endif
                // std::cout << errno << std::endl;
                // if (connetionSlots[clID] && error_NB!=10058){
                //     std::cerr << "[socketStream] testSend message failed with error: " << error_NB << std::endl;
                //     #ifdef _WIN32
                //         closesocket(clientsSockets[clID]);
                //     #else
                //         close(clientsSockets[clID]);
                //     #endif
                //     // #ifdef _WIN32
                //     //     WSACleanup();
                //     // #endif
                //     return -1;
                // }
            }
        }else{
            if(mode==SOCKETSTREAM::SOCKETSTREAM_SERVER){
                if (!warningsWatcher[clID]){
                    std::cerr << "[socketStream] The communication with the client is not open. " << std::endl;
                    warningsWatcher[clID] = true;
                }
                
            }else{
                std::cerr << "[socketStream] The communication with the server is not open. " << std::endl;
            }
            
            return -2;
        }

    }else{
        std::cerr << "[socketStream] The message struct id NOT initialized. " << std::endl;
        return -3;
    }

    // final_msg.clear();
    return iResult;

}

int socketStream::sendMSg2Client(std::string clID){
    /** 
     * 
     * The function search for a client id in the clients' ids and sends the message to this client
     * 
     * Return:
     *      the number of clients to which the message was sent correctly
     *      -1, if there is no client with the spefied ID
     * 
     */
    
    int nb_clients_found = 0;
    int nb_clients = 0;
    for (unsigned int i = 0; i < (unsigned int)clientIDs.size(); i++ ){
        if(clientIDs[i].compare(clID)==0){
            if(connetionSlots[i]){
                if(sendMSg2Client(i)>0){
                    nb_clients++;
                }else{
                    if (!warningsWatcher[i]){
                        std::cerr << "[socketStream] A client with id \"" << clID << "\" found but the message was not sent. " << std::endl;
                        warningsWatcher[i] = true;
                    }
                }
                nb_clients_found++;
            }
        }
    }

    if(nb_clients_found==0){
        if(!giveWarning){
            std::cout << "[socketStream] No client with id \"" << clID << "\" was found. " << std::endl;
            giveWarning = true;
        }
    }
    
    if(nb_clients>0){
        return nb_clients;
    }else{
        return -1;
    }
}

int socketStream::sendMSg2All(){
    /** 
     * 
     * The function sends the message to all the clients
     * 
     * Return:
     *      the number of clients to which the message was sent correctly
     *      -1, if there is no client with the spefied ID
     * 
     */

    int nb_clients = 0;
    for(unsigned int i = 0; i< (unsigned int)connetionSlots.size(); i++){
        if(connetionSlots[i]){
            if(sendMSg2Client(i)>0){
                nb_clients++;
            }else{
                std::cerr << "[socketStream] The message to client with id \"" << clientIDs[i] << "\" was not sent. " << std::endl;
            }
        }
    }

    if(nb_clients>0){
        return nb_clients;
    }else{
        return -1;
    }
}

int socketStream::sendEndCommMsg(unsigned int clID){

    iResult = send(clientsSockets[clID], ec_id, (int)strlen(ec_id), 0 );
    if (iResult == SOCKET_ERROR) {
        std::cerr << "[socketStream] Send message failed with error: ";
        #ifdef _WIN32
            std::cerr << WSAGetLastError();
            WSACleanup();
        #else
            std::cerr << strerror(errno);
        #endif
        std::cerr << std::endl;
            
        #ifdef _WIN32
            closesocket(clientsSockets[clID]);
            WSACleanup();
        #else
            close(clientsSockets[clID]);
        #endif
        return -1;
    }
    return 0;
}

int socketStream::closeCommunication(){

    // std::cout << "inside clcoseCommunication" << std::endl;
    // std::cout << int(isServer) << std::endl;


    // cleanup
    if(isServer){

        // shutdown the connections, if exist
        for(int i = 0; i < (int)clientsSockets.size(); i++){
            if(connetionSlots[i]){
                if(sendEndCommMsg(i)<0){
                    std::cout<< "[socketStream] Unable to send ending message to client " << i << std::endl;
                }
                iResult = shutdown(clientsSockets[i], SD_SEND);
                if (iResult == SOCKET_ERROR) {
                    std::cerr << "[socketStream] Send message failed with error: ";
                    #ifdef _WIN32
                        std::cerr << WSAGetLastError();
                        WSACleanup();
                    #else
                        std::cerr << strerror(errno);
                    #endif
                    std::cerr << std::endl;
                    
                    return -3;
                }
                #ifdef _WIN32
                    closesocket(clientsSockets[i]);
                #else
                    close(clientsSockets[i]);
                #endif
                connetionSlots[i] = false;
                // if(connectionThreads[i].joinable()){
                //     std::cout << "close communication the thread is still running\n";
                //     connectionThreads[i].join();
                // }
            }
        }

        if(serverRunning){
            threadMutex.lock();
            serverRunning = false;
            threadMutex.unlock();
            if(falseConnection()<0){
                std::cout << "Test failed?\n";
            }
            listenerThread.join();
            watcherThread.join();
            // if(falseConnection()<0){
            //     std::cout << "Test failed?\n";
            // }
            #ifdef _WIN32
                iResult = shutdown(ListenSocket, SD_SEND);
                if ((iResult == SOCKET_ERROR) && (WSAGetLastError() != 10057)) {
                    std::cerr << "[socketStream] Shutdown failed with error: " << WSAGetLastError() << std::endl;
                    closesocket(ListenSocket);
                    WSACleanup();
                    return -2;
                }
            #else
                iResult = shutdown(ListenSocket, SD_SEND);
                if (iResult < 0){
                    std::cerr << "[socketStream] Shutdown failed with error: " << strerror(errno) << std::endl;
                    iResult = close(ListenSocket);
                    return -2;
                }
            #endif
        }

        for (int i=0; i< (int)clientsSockets.size(); i++){
            #ifdef _WIN32
                closesocket(clientsSockets[i]);
            #else
                close(clientsSockets[i]);
            #endif
        }

        

        #ifdef _WIN32
            closesocket(ListenSocket);
        #else
            close(ListenSocket);
            close(ConnectSocket);
        #endif
        
        

        threadMutex.lock();
        std::cout << "[socketStream] All sockets are successfully closed" << std::endl;
        std::cout << "[socketStream] Server is shutted-down" << std::endl;
        threadMutex.unlock();
    }
    else{
        std::cout<<"[socketStream] Ending connection ..."<<std::endl;

        // if(connectionThreads[0].joinable()){
        
        // }

        if(isComActive){            

            iResult = send(ConnectSocket, ec_id, (int)strlen(ec_id), 0 );
            if (iResult == SOCKET_ERROR) {
                std::cerr << "[socketStream] Send message failed with error: ";
                #ifdef _WIN32
                    std::cerr << WSAGetLastError();
                    WSACleanup();
                #else
                    std::cerr << strerror(errno);
                #endif
                std::cerr << std::endl;
                
                #ifdef _WIN32
                    closesocket(ConnectSocket);
                    WSACleanup();
                #else
                    close(ConnectSocket);
                #endif
                return -1;
            }

            #ifdef _WIN32
                iResult = shutdown(ConnectSocket, SD_SEND);
                if (iResult == SOCKET_ERROR) {
                    std::cerr << "[socketStream] Shutdown failed with error: " << WSAGetLastError() << std::endl;
                    closesocket(ConnectSocket);
                    WSACleanup();
                    return -2;
                }
            #else
                iResult = shutdown(ConnectSocket, SD_SEND);
                if (iResult < 0){
                    std::cerr << "[socketStream] Shutdown failed with error: " << strerror(errno) << std::endl;
                    iResult = close(ConnectSocket);
                    return -2;
                }
            #endif
        }

        
        isComActive = false;
        connetionSlots[0] = false;
        connectionThreads[0].join();
        for (int i=0; i<(int)clientsSockets.size();i++){
            #ifdef _WIN32
                closesocket(clientsSockets[i]);
            #else
                close(clientsSockets[i]);
            #endif
        }
        
        

        // std::cout << "isComActive: " << int(isComActive) << std::endl; 


        #ifdef _WIN32
            closesocket(ConnectSocket);
        #else
            close(ConnectSocket);
        #endif

    }
    
    #ifdef _WIN32
        WSACleanup();
    #endif

    return 0;
}

int socketStream::setHashKey(bool hKey){
    useHashKey=hKey;
    return 0;
}

int socketStream::setVerbose(bool _verbose){

    verbose = _verbose;
    return 0;
}

int socketStream::setHeaderSize(unsigned int hSize){
    headerSize=hSize;
    msgHeader = std::string(headerSize, ' ');
    return 0;
}

std::string socketStream::getFullmsg(){
    return final_msg;
}


int socketStream::checkKeyPressed(std::string key){

    std::lock_guard<std::mutex> guard(threadMutex);
    while(true){
        if(kbhit()){
            if(key[0]==getchar()){
                serverRunning = false;
                break;
            }
        }
    }

    return 0;
}


int socketStream::findEmptySlot(){
    for (int i = 0; i < (int)connetionSlots.size(); i++ ){
        if (!connetionSlots[i]){
            return i;
        }
    }
    return -1;
}

int socketStream::runServer(){
    if(serverRunning){
        listenerThread = std::thread(&socketStream::wait_connections, this);
        // listenerThread.detach();
        watcherThread = std::thread(&socketStream::connectionsWatcher, this);
    }else{
        std::cerr << "[socketStream] Server is not running" << std::endl;
        return -1;
    }

    return 0;
}



int socketStream::wait_connections(){

    if(!isServer){
        std::cerr << "[socketStream] socketStream is not in the server mode" << std::endl;
        return -1;
    }

    if(!serverRunning){
        std::cerr << "[socketStream] Server is not initialized properly" << std::endl;
        return -2;
    }

    socklen_t cli_addr_size;

    int slotNumber = -1;
    int nbConnections = 0;
    std::cout << "[socketStream] Server initialized. Waiting for connections ... " << std::endl;
    while(serverRunning){
        struct sockaddr_in clientAddress;
        cli_addr_size = sizeof(clientAddress);

        // Accept a client socket
        SOCKET ClientSocket = accept(ListenSocket, (struct sockaddr *)&clientAddress,  &cli_addr_size);
        if(!serverRunning){
            break;
        }
        // std::cout << "wait_connections test\n";
        if (ClientSocket == INVALID_SOCKET) {
            
            threadMutex.lock();
            
            #ifdef _WIN32
                if(WSAGetLastError()!=10004){
                    std::cerr << "[socketStream] Accept failed with error: ";
                    std::cerr << WSAGetLastError();
                    WSACleanup();
                    std::cerr << std::endl;
                }
            #else
                // if(strerror(errno)!=10004){
                if(errno!=EINTR){
                    std::cerr << "[socketStream] Accept failed with error: " << strerror(errno) << " " << errno << std::endl;
                }
            #endif
            threadMutex.unlock();
            #ifdef _WIN32
                closesocket(ListenSocket);
            #else
                close(ListenSocket);
            #endif
            break;
            // return -2;
        }
        std::string clAddStr = inet_ntoa(clientAddress.sin_addr);
        slotNumber = findEmptySlot();
        if(slotNumber < 0){
            iResult = shutdown(ClientSocket, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                std::cerr << "[socketStream] Shutdown failed with error:" ;
                #ifdef _WIN32
                    std::cerr << WSAGetLastError();
                    // WSACleanup();
                #endif
                std::cerr << std::endl;
                #ifdef _WIN32
                    closesocket(ClientSocket);
                #else
                    close(ClientSocket);
                #endif
            }
            threadMutex.lock();
            std::cout << "[socketStream] Connection from " << clAddStr << " is rejected due to maximum connections capacity reached" << std::endl; 
            threadMutex.unlock();
        }else{
            bool validclient = handshake_client(ClientSocket, 10, slotNumber);
            if(!validclient){
                threadMutex.lock();
                std::cout << "[socketStream] Handshake protocol with client " << clAddStr << " failed! Closing connection with client." << std::endl;
                threadMutex.unlock();
                iResult = shutdown(ClientSocket, SD_SEND);
                if (iResult == SOCKET_ERROR) {
                    std::cerr << "[socketStream] Shutdown failed with error:" ;
                    #ifdef _WIN32
                        std::cerr << WSAGetLastError();
                        WSACleanup();
                    #else
                        std::cerr << strerror(errno);
                    #endif
                    std::cerr << std::endl;
                    #ifdef _WIN32
                        closesocket(ClientSocket);
                    #else
                        close(ClientSocket);
                    #endif
                }
            }else{
                threadMutex.lock();
                if(connectionThreads[slotNumber].joinable()){
                    std::cout << "[socketStream] A thread is still running\n";
                    connectionThreads[slotNumber].join();
                }
                clientsAddresses[slotNumber] = clAddStr;
                firstMsgReceived[slotNumber]= false;
                isNewMsgReceived[slotNumber] = false;
                clientsSockets[slotNumber] = ClientSocket;
                connetionSlots[slotNumber] = true;
                warningsWatcher[slotNumber] = false;
                giveWarning = false;
                verbose = true;
                // clientIDs[slotNumber] = std::string("sS_c#") + std::to_string(slotNumber);
                std::cout << "[socketStream] Accepted connection from client: " << clAddStr << " with id: " << clientIDs[slotNumber] << ", slot: " << slotNumber << std::endl; 
                threadMutex.unlock();          

                if(mode==SOCKETSTREAM::SOCKETSTREAM_SERVER){
                    connectionThreads[slotNumber] = std::thread(&socketStream::runReceiver, this, slotNumber);

                    // connectionThreads[slotNumber].detach();
                    // std::thread receiverThread(&socketStream::runReceiver, this, slotNumber);
                    // receiverThread.detach();
                }    
                nbConnections++;
            }
        }
        
        if(verbose){
            threadMutex.lock();
            std::cout << "[socketStream] Waiting for new connections ... " << std::endl;
            threadMutex.unlock();
        }
    }

    serverRunning = false;

    std::cout << "[socketStream] Server has stopped waiting for connections" << std::endl;
    return 0;
}



int socketStream::runReceiver(int connectionID){

    int iResutlReceiver = 0;
    char tmp_buf[SOCKETSTREAM::DEFAULT_BUFLEN];
    std::string fullmsg;
    bool connectionExists = true;
    char clientRecvbuf[SOCKETSTREAM::DEFAULT_BUFLEN];
    std::string validMsg;
    bool msgValidity = false;

    threadMutex.lock();
    SOCKET clntSocket=clientsSockets[connectionID];
    threadMutex.unlock();
    int counter = 0;

    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    do{
        // threadMutex.lock();
        iResutlReceiver = recv(clntSocket, clientRecvbuf, bufferSize, 0);
        // threadMutex.unlock();
        if(iResutlReceiver<0){
            std::cerr << "[socketStream] 1Unable to receive data" << std::endl;
        }
        memcpy(tmp_buf, clientRecvbuf, strlen(msg_idf));
        if(strcmp(tmp_buf, msg_idf)==0){
            memcpy(tmp_buf, clientRecvbuf + strlen(msg_idf), strlen(clientRecvbuf) - strlen(msg_idf));
            // for(int ii=0; ii< strlen(tmp_buf); ii++){
            //     std::cout << tmp_buf[ii];
            // }
            // std::cout << std::endl;
            fullmsg = std::string(tmp_buf);
            // std::cout << fullmsg << std::endl;
            while(true){
                // threadMutex.lock();
                if(mode == SOCKETSTREAM::SOCKETSTREAM_CLIENT){
                    if(!isComActive){
                        // std::cout << "test1 on receiver\n";
                        break;
                    }
                }
                if(mode == SOCKETSTREAM::SOCKETSTREAM_SERVER){
                    if(!connetionSlots[connectionID]){
                        break;
                    }
                }
                iResutlReceiver = recv(clntSocket, clientRecvbuf, bufferSize, 0);
                // threadMutex.unlock();
                if(iResutlReceiver<0){
                    std::cerr << "[socketStream] Unable to receive data" << std::endl;
                }
                
                
                
                fullmsg += clientRecvbuf;
                memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
                if(fullmsg.substr(fullmsg.length()-strlen(endMSG), strlen(endMSG)).compare(endMSG)==0){
                    // std::cout << "test 2 on receiver\n";
                    // std::cout << fullmsg << std::endl;
                    validMsg = messageExtractor(fullmsg, &msgValidity);
                    // std::cout << validMsg << std::endl;
                    // std::cout << msgValidity << std::endl;
                    if(msgValidity){
                        threadMutex.lock();
                        clientMsgs[connectionID] = "";
                        clientMsgs[connectionID] = validMsg;
                        if(!isNewMsgReceived[connectionID]){
                            isNewMsgReceived[connectionID] = true;
                        }
                        threadMutex.unlock();
                        counter++;
                        end = std::chrono::steady_clock::now();
                        if((double)(std::chrono::duration_cast<std::chrono::microseconds>(end-start).count())/1000.0 > 1000){
                            
                            if(verbose){
                                if(mode == SOCKETSTREAM::SOCKETSTREAM_SERVER){
                                    threadMutex.lock();
                                    std::cout<< "[socketStream] Receiving from client \"" << clientIDs[connectionID] << "\" with frequency " << counter << " Hz" << std::endl;
                                    threadMutex.unlock();
                                }
                                if(mode == SOCKETSTREAM::SOCKETSTREAM_CLIENT){
                                    threadMutex.lock();
                                    std::cout<< "[socketStream] Receiving from server with frequency " << counter << " Hz" << std::endl;
                                    threadMutex.unlock();
                                }
                            }
                            
                            counter = 0;
                            start = std::chrono::steady_clock::now();
                        }

                    }
                    if(!firstMsgReceived[connectionID])
                        firstMsgReceived[connectionID] = true;
                    break;
                }
            }            
        }
        // std::cout << "test 3 on receiver\n";
        memcpy(tmp_buf, clientRecvbuf, strlen(ec_id));
        if(strcmp(tmp_buf, ec_id)==0){
            // the connection is terminated from the other end
            iResutlReceiver = shutdown(clntSocket, SD_SEND);
            if (iResutlReceiver == SOCKET_ERROR) {
                std::cerr << "[socketStream] Shutdown failed with error:" ;
                #ifdef _WIN32
                    std::cerr << WSAGetLastError();
                    // WSACleanup();
                #else
                    std::cerr << strerror(errno);
                #endif
                std::cerr << std::endl;
                
                // return -1;
            }
            #ifdef _WIN32
                closesocket(clntSocket);
            #else
                close(clntSocket);
            #endif
            
            if(mode == SOCKETSTREAM::SOCKETSTREAM_SERVER){
                std::cout << "[socketStream] Connection terminated by client (id: \"" << clientIDs[connectionID] << "\", address: " << clientsAddresses[connectionID] << ")" << std::endl;
            }
            if(mode == SOCKETSTREAM::SOCKETSTREAM_CLIENT){
                std::cout<< "[socketStream] Connection with the server \"" << clientsAddresses[connectionID] << "\" terminated by the server" << std::endl;
                isComActive = false;
            }
            threadMutex.lock();
            firstMsgReceived[connectionID]= false ;
            clientsSockets[connectionID] = INVALID_SOCKET;
            clientsAddresses[connectionID] = " ";
            isNewMsgReceived[connectionID] = false;
            connetionSlots[connectionID] = false;
            if(mode == SOCKETSTREAM::SOCKETSTREAM_SERVER){
                clientIDs[connectionID] = " ";
            }
            if(mode == SOCKETSTREAM::SOCKETSTREAM_CLIENT){
                isComActive = false;
            }
            clientMsgs[connectionID] = " ";
            threadMutex.unlock();
            connectionExists = false;
        }
        memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
        memset(tmp_buf, 0, sizeof(tmp_buf));
        if(mode == SOCKETSTREAM::SOCKETSTREAM_SERVER){
            if(!serverRunning){
                break;
            }
        }
        if(mode == SOCKETSTREAM::SOCKETSTREAM_CLIENT){
            if(!isComActive){
                break;
            }
        }
        
    }while((iResutlReceiver>0));

    if(connectionExists && connetionSlots[connectionID]){
        // std::cout << "test 4 on receiver\n";
        iResutlReceiver = shutdown(clientsSockets[connectionID], SD_SEND);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "[socketStream] Shutdown failed with error:" ;
            #ifdef _WIN32
                std::cerr << WSAGetLastError();
                // WSACleanup();
            #else
                std::cerr << strerror(errno);
            #endif
            std::cerr << std::endl;
            #ifdef _WIN32
                closesocket(clientsSockets[connectionID]);
            #else
                close(clientsSockets[connectionID]);
            #endif
            connectionExists = false;
            return -1;
        }
        
        #ifdef _WIN32
            closesocket(clientsSockets[connectionID]);
        #else
            close(clientsSockets[connectionID]);
        #endif
        

        if(mode == SOCKETSTREAM::SOCKETSTREAM_SERVER){
            threadMutex.lock();
            std::cout << "[socketStream] Connection with the client (id: " << clientIDs[connectionID] << ", address: " << clientsAddresses[connectionID] << " ) forcibly terminated" << std::endl;
            threadMutex.unlock();
        }
        if(mode == SOCKETSTREAM::SOCKETSTREAM_CLIENT){
            threadMutex.lock();
            std::cout<< "[socketStream] Connection with the server: " << clientsAddresses[connectionID] << " terminated." << std::endl;
            threadMutex.unlock();
        }
        firstMsgReceived[connectionID]= false ;
        clientsSockets[connectionID] = INVALID_SOCKET;
        clientsAddresses[connectionID] = " ";
        isNewMsgReceived[connectionID] = false;
        connetionSlots[connectionID] = false;
        if(mode == SOCKETSTREAM::SOCKETSTREAM_SERVER){
            clientIDs[connectionID] = " ";
        }
        if(mode == SOCKETSTREAM::SOCKETSTREAM_CLIENT){
            isComActive = false;
        }
        clientMsgs[connectionID] = " ";
        connectionExists = false;
        memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
        memset(tmp_buf, 0, sizeof(tmp_buf));
    }
    if(verbose){
        std::cout << "[socketStream] Communication slot " << connectionID << " freed" << std::endl;
    }
    return 0;
}


std::string socketStream::messageExtractor(std::string fullmsg, bool* msgValidity){
    
    int bfrDigits = std::strlen(std::to_string(bufferSize).c_str());
    int msgSize = std::stoi(fullmsg.substr(0, headerSize).c_str());
    // int msgOverhead = std::stoi(fullmsg.substr(headerSize + 1, headerSize + 1 + bfrDigits));
    std::string tmp_msg = fullmsg.substr(headerSize+1+bfrDigits,msgSize);
    if(((char)fullmsg[headerSize+1]-'0')==1){
        std::string md5_key = md5(tmp_msg.c_str());
        std::string hashcode = fullmsg.substr(headerSize+1+bfrDigits+msgSize, md5_key.length());
        if(hashcode.compare(md5_key)==0){
            *msgValidity = true;
        }else{
            *msgValidity = false;
        }
    }else{
        *msgValidity = true;
    }
    

    return tmp_msg;

}

std::string socketStream::get_latest(bool* newMSG){
    std::lock_guard<std::mutex> lck (threadMutex);
    std::string l_msg = clientMsgs[0];
    *newMSG = isNewMsgReceived[0];
    if(firstMsgReceived[0]){
        if(isNewMsgReceived[0]){
            
            isNewMsgReceived[0] = false;
            
        }
    }
    // threadMutex.unlock();
    return l_msg;
}


std::string socketStream::get_latest(std::string cltName, bool* newMSG){
    // std::string l_msg = "";
    std::lock_guard<std::mutex> lck (threadMutex);
    auto srch_result = std::find(std::begin(clientIDs), std::end(clientIDs), cltName);
    if(srch_result != std::end(clientIDs)){
        int idx= std::distance(clientIDs.begin(),srch_result);
        *newMSG = isNewMsgReceived[idx];
        if(isNewMsgReceived[idx]){ 
            
            isNewMsgReceived[idx] = false;
            
        }

        return clientMsgs[idx];
    }
    // threadMutex.lock();
    // std::cout << "[socketStream] The client \"" << cltName << "\" is not connected. Check if the name is correct" << std::endl;
    // std::cout << "[socketStream] Returning empty string" << std::endl;
    // threadMutex.unlock();

    return "";
}

bool socketStream::socketStream_ok(){
    bool tmp = false;
    
    std::for_each(connetionSlots.begin(), connetionSlots.end(),[&tmp](const bool &connetionSlots){tmp = tmp || connetionSlots;});
    
    std::for_each(firstMsgReceived.begin(), firstMsgReceived.end(),[&tmp](const bool &firstMsgReceived){tmp = tmp || firstMsgReceived;});
    
    std::for_each(isNewMsgReceived.begin(), isNewMsgReceived.end(),[&tmp](const bool &isNewMsgReceived){tmp = tmp || isNewMsgReceived;});
    
    if (mode==SOCKETSTREAM::SOCKETSTREAM_SERVER){
        return (tmp && serverRunning);
    }else{
        return tmp;
    }
    

}


std::string socketStream::randomString(int strlength){
    std::string numbersLetters = "0123456789ABCDEFGHIJKLMNOPQRSTWXYZabcdefghijklmnopqrstwxyz";

    std::string randomStr;
    for(int i = 0; i < strlength; i++){
        randomStr+=numbersLetters[rand() % numbersLetters.length()];
    }

    return randomStr;
}


bool socketStream::handshake_client(SOCKET conc, int strlength, int slotNb){

    int iResutlReceiver = 0;
    int uHsKey = 1;
    char clientRecvbuf[SOCKETSTREAM::DEFAULT_BUFLEN];
    bool msgValidity = false;
    std::vector <double> ping_times(10,0);
    std::vector <double> compute_times(10,0);
    std::vector <bool> validity_counter(10, false);
    std::string validMsg;
    std::string fullmsg;

    auto start_ping = std::chrono::steady_clock::now();
    auto end_ping = std::chrono::steady_clock::now();

    memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
    for(int i = 0; i < 10; i++){
        while(true){
            iResutlReceiver = recv(conc, clientRecvbuf, bufferSize, 0);
            if(iResutlReceiver<0){
                std::cerr << "[socketStream] Unable to receive data" << std::endl;
            }
            fullmsg += std::string(clientRecvbuf);
            memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
            if(fullmsg.substr(fullmsg.length()-strlen(endMSG), strlen(endMSG)).compare(endMSG)==0){
                 break;
            }
        }
        end_ping = std::chrono::steady_clock::now();
        ping_times[i]=(double)(std::chrono::duration_cast<std::chrono::microseconds>(end_ping-start_ping).count())/1000.0;
        validMsg = messageExtractor(fullmsg.substr(strlen(msg_idf), fullmsg.length()), &msgValidity);
        validity_counter[i] = msgValidity;
        
        std::string ranString=randomString(strlength);
        start_ping = std::chrono::steady_clock::now();
        std::string md5_key = md5(ranString.c_str());

        

        // introduce the lenght of the string in the header of the file
        std::ostringstream i2s;
        i2s << ranString.length();

        msgHeader.replace(msgHeader.begin(),msgHeader.end()+i2s.str().length()-msgHeader.length(),i2s.str());
        ranString += md5_key;
        // compute the overhead of the message
        msgOverhead = (int((ranString.length()+minMsgSize)/bufferSize)+1)*bufferSize - (ranString.length()+minMsgSize);

        std::ostringstream i2s2;
        i2s2 << msgOverhead;        

        msgOHstring.replace(msgOHstring.begin(),msgOHstring.end()+i2s2.str().length()-msgOHstring.length(),i2s2.str());

        // compose the final message
        final_msg = msg_idf + msgHeader + std::to_string(uHsKey) + msgOHstring + ranString + std::string(msgOverhead, ' ') + endMSG;

        end_ping = std::chrono::steady_clock::now();
        compute_times[i] = (double)(std::chrono::duration_cast<std::chrono::microseconds>(end_ping-start_ping).count())/1000.0;
        start_ping = std::chrono::steady_clock::now();
        iResutlReceiver = send(conc, final_msg.c_str(), (int)final_msg.size(), 0 );
        if (iResutlReceiver == SOCKET_ERROR) {
            std::cerr << "[socketStream] Send message failed with error: ";
            #ifdef _WIN32
                std::cerr << WSAGetLastError();
                WSACleanup();
            #else
                std::cerr << strerror(errno);
            #endif
            std::cerr << std::endl;
            
            #ifdef _WIN32
                closesocket(conc);
                WSACleanup();
            #else
                close(conc);
            #endif
            return false;
        }
        
        memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
        fullmsg.clear();
        final_msg.clear();
        msgHeader = std::string(headerSize, ' ');
    }

    double sumVec1 = 0;
    // find the average computational time and ping time and print it in the terminal
    for(auto& n: compute_times){
        sumVec1 += n;
    }
    double avCompute = sumVec1/(double)compute_times.size();

    double sumVec2 = 0;
    for(auto& n: ping_times){
        sumVec2 += n;
    }
    double avPing = sumVec2/(double)ping_times.size();

    double validityAverage = 0;
    for(unsigned int i = 0; i<validity_counter.size(); i++){
        validityAverage += double(validity_counter[i]);
    }
    validityAverage = validityAverage/(double)validity_counter.size();

    if(validityAverage==1){
        while(true){
            iResutlReceiver = recv(conc, clientRecvbuf, bufferSize, 0);
            if(iResutlReceiver<0){
                std::cerr << "[socketStream] Unable to receive data" << std::endl;
            }
            fullmsg += std::string(clientRecvbuf);
            memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
            if(fullmsg.substr(fullmsg.length()-strlen(endMSG), strlen(endMSG)).compare(endMSG)==0){
                 break;
            }
        }        
        validMsg = messageExtractor(fullmsg.substr(strlen(msg_idf), fullmsg.length()), &msgValidity);

        threadMutex.lock();
        if(msgValidity){
            clientIDs[slotNb]=validMsg;
        }else{
            clientIDs[slotNb]="None";
        }
        if(verbose){
            std::cout << "[socketStream] Average ping time to the client: " << avPing << " ms" << std::endl;
            std::cout << "[socketStream] message construction time: " << avCompute << " ms" << std::endl;
        }
        threadMutex.unlock();
        return true;
    }

    
    

    return false;
}

bool socketStream::handshake_server(int strlength){

    // std::cout << "inside handshake server" << std::endl;

    int iResutlReceiver = 0;
    int uHsKey = 1;
    char clientRecvbuf[SOCKETSTREAM::DEFAULT_BUFLEN];
    bool msgValidity = false;
    std::vector <double> ping_times(10,0);
    std::vector <double> compute_times(10,0);
    std::vector <bool> validity_counter(10, false);
    std::string validMsg;
    std::string fullmsg;

    auto start_ping = std::chrono::steady_clock::now();
    auto end_ping = std::chrono::steady_clock::now();

    memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
    for(int i = 0; i < 10; i++){
        std::string ranString=randomString(strlength);
        start_ping = std::chrono::steady_clock::now();
        std::string md5_key = md5(ranString.c_str());

        

        // introduce the lenght of the string in the header of the file
        std::ostringstream i2s;
        i2s << ranString.length();

        msgHeader.replace(msgHeader.begin(),msgHeader.end()+i2s.str().length()-msgHeader.length(),i2s.str());
        ranString += md5_key;
        // compute the overhead of the message
        msgOverhead = (int((ranString.length()+minMsgSize)/bufferSize)+1)*bufferSize - (ranString.length()+minMsgSize);

        std::ostringstream i2s2;
        i2s2 << msgOverhead;        

        msgOHstring.replace(msgOHstring.begin(),msgOHstring.end()+i2s2.str().length()-msgOHstring.length(),i2s2.str());

        // compose the final message
        final_msg = msg_idf + msgHeader + std::to_string(uHsKey) + msgOHstring + ranString + std::string(msgOverhead, ' ') + endMSG;

        end_ping = std::chrono::steady_clock::now();
        compute_times[i] = (double)(std::chrono::duration_cast<std::chrono::microseconds>(end_ping-start_ping).count())/1000.0;
        start_ping = std::chrono::steady_clock::now();
        iResutlReceiver = send(ConnectSocket, final_msg.c_str(), (int)final_msg.size(), 0 );
        if (iResutlReceiver == SOCKET_ERROR) {
            std::cerr << "[socketStream] Send message failed with error: ";
            #ifdef _WIN32
                std::cerr << WSAGetLastError();
                WSACleanup();
            #else
                std::cerr << strerror(errno);
            #endif
            std::cerr << std::endl;
            
            #ifdef _WIN32
                closesocket(ConnectSocket);
                WSACleanup();
            #else
                close(ConnectSocket);
            #endif
            return false;
        }
        
        while(true){
            iResutlReceiver = recv(ConnectSocket, clientRecvbuf, bufferSize, 0);
            if(iResutlReceiver<0){
                std::cerr << "[socketStream] Unable to receive data" << std::endl;
            }
            fullmsg += std::string(clientRecvbuf);
            memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
             if(fullmsg.substr(fullmsg.length()-strlen(endMSG), strlen(endMSG)).compare(endMSG)==0){
                break;
            }
        }
        end_ping = std::chrono::steady_clock::now();
        ping_times[i]=(double)(std::chrono::duration_cast<std::chrono::microseconds>(end_ping-start_ping).count())/1000.0;
        validMsg = messageExtractor(fullmsg.substr(strlen(msg_idf), fullmsg.length()), &msgValidity);
        validity_counter[i] = msgValidity;

        memset(clientRecvbuf, 0, sizeof(clientRecvbuf));
        fullmsg.clear();
        final_msg.clear();
        msgHeader = std::string(headerSize, ' ');
    }

    double sumVec1 = 0;
    // find the average computational time and ping time and print it in the terminal
    for(auto& n: compute_times){
        sumVec1 += n;
    }
    double avCompute = sumVec1/(double)compute_times.size();

    double sumVec2 = 0;
    for(auto& n: ping_times){
        sumVec2 += n;
    }
    double avPing = sumVec2/(double)ping_times.size();

    double validityAverage = 0;
    for(unsigned int i = 0; i<validity_counter.size(); i++){
        validityAverage += double(validity_counter[i]);
    }
    validityAverage = validityAverage/(double)validity_counter.size();

    if(validityAverage==1){
        //send the name of the client
        std::string ranString=clientName;
        std::string md5_key = md5(ranString.c_str());
        

        // introduce the lenght of the string in the header of the file
        std::ostringstream i2s;
        i2s << ranString.length();
        // std::cout << "length of my name"

        msgHeader.replace(msgHeader.begin(),msgHeader.end()+i2s.str().length()-msgHeader.length(),i2s.str());
        ranString += md5_key;
        // compute the overhead of the message
        msgOverhead = (int((ranString.length()+minMsgSize)/bufferSize)+1)*bufferSize - (ranString.length()+minMsgSize);

        std::ostringstream i2s2;
        i2s2 << msgOverhead;        

        msgOHstring.replace(msgOHstring.begin(),msgOHstring.end()+i2s2.str().length()-msgOHstring.length(),i2s2.str());

        // compose the final message
        final_msg = msg_idf + msgHeader + std::to_string(uHsKey) + msgOHstring + ranString + std::string(msgOverhead, ' ') + endMSG;  
        iResutlReceiver = send(ConnectSocket, final_msg.c_str(), (int)final_msg.size(), 0 );
        if (iResutlReceiver == SOCKET_ERROR) {
            std::cerr << "[socketStream] Send message failed with error: ";
            #ifdef _WIN32
                std::cerr << WSAGetLastError();
                WSACleanup();
            #else
                std::cerr << strerror(errno);
            #endif
            std::cerr << std::endl;
            
            #ifdef _WIN32
                closesocket(ConnectSocket);
                WSACleanup();
            #else
                close(ConnectSocket);
            #endif
            return false;
        }

        // print some diagnostics
        if (verbose){
            threadMutex.lock();
            std::cout << "[socketStream] Average ping time to the server: " << avPing << " ms" << std::endl;
            std::cout << "[socketStream] Average message construction time: " << avCompute << " ms" << std::endl;
            threadMutex.unlock();
        }
        fullmsg.clear();
        final_msg.clear();
        msgHeader = std::string(headerSize, ' ');
        return true;
    }

    return false;
}

int socketStream::connectionsWatcher(){

    while(serverRunning){
        for (unsigned int i=0; i<nb_connections; i++){
            if(!connetionSlots[i]){
                if(connectionThreads[i].joinable()){
                    connectionThreads[i].join();
                }
            }
        }
    }

    return 0;
}

int socketStream::falseConnection(){

    std::ostringstream i2s;
    i2s << Host_Port;

    // Resolve the server address and port
    iResult = getaddrinfo(Host_IP, i2s.str().c_str(), &hints, &result);
    if ( iResult != 0 ) {
        std::cerr << "[socketStream] getaddrinfo failed with error: " << iResult << std::endl;;
        #ifdef _WIN32
            WSACleanup();
        #endif
        return iResult;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cerr << "[socketStream] socket failed with error: "; 
            #ifdef _WIN32
                std::cerr <<  WSAGetLastError();
                WSACleanup();
            #else
                std::cerr << strerror(errno);
            #endif
            std::cerr << std::endl;
            return -1;
        }

        
        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            #ifdef _WIN32
                closesocket(ConnectSocket);
            #else
                close(ConnectSocket);
            #endif
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    #ifdef _WIN32
        closesocket(ConnectSocket);
    #else
        close(ConnectSocket);
    #endif
    ConnectSocket = INVALID_SOCKET;

    return 0;    
}

socketStream::~socketStream(){

    if(isServer){
        if(serverRunning){
            #ifdef _WIN32
                closesocket(ListenSocket);
            #else
                close(ListenSocket);
            #endif
            // shutdown the connection since we're done
            for(int i = 0; i < (int)clientsSockets.size(); i++){
                iResult = shutdown(clientsSockets[i], SD_SEND);
                if (iResult == SOCKET_ERROR) {
                    std::cerr << "[socketStream] Shutting-down socket client " << i << " failed with error:";
                    #ifdef _WIN32
                        std::cerr << WSAGetLastError();
                        WSACleanup();
                    #else
                        std::cerr << strerror(errno);
                    #endif
                    std::cerr << std::endl;
                }
                #ifdef _WIN32
                    closesocket(clientsSockets[i]);
                #else
                    close(clientsSockets[i]);
                #endif
                connetionSlots[i] = false;
                if(connectionThreads[i].joinable()){
                    connectionThreads[i].join();
                }
            }
            serverRunning = false;
            threadMutex.lock();
            std::cout << "[socketStream] All sockets are successfully closed" << std::endl;
            std::cout << "[socketStream] Server is shutted-down" << std::endl;
            threadMutex.unlock();
        }
    }else{
        if(isComActive){
            if(closeCommunication()<0){
                std::cerr << "[socketStream] The socket is NOT successully terminated. The port might still be occupied." << std::endl;
                }
            isComActive = false;
        }
    }

    threadMutex.lock();
    std::cout << "[socketStream] socketStream out" << std::endl;
    threadMutex.unlock();
}