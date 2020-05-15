/**
 *  Copyright (C) 2020 Iason Batzianoulis
 * 
 *  jsonWrapper source file
 *    
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    GNU GPLv3
 * 
**/

#include "jsonWrapper.hpp"


jsonWrapper::jsonWrapper(){

    std::cout << "[jsonWrapper] Hello from jsonWrapper" << std::endl;

    isobjectok = false;
}

jsonWrapper::jsonWrapper(std::string jsonString){

    if (jsonDoc.Parse(jsonString.c_str()).HasParseError()){
        std::cerr << "[jsonWrapper] Parsing rapidjson error. Unable to parse json string" << std::endl; 
        std::cout << jsonString << std::endl;
    }

    if(jsonDoc.IsObject()){
        isobjectok = true;
    }else{
        isobjectok = false;
        std::cout << "is not an objsect\n";
    }

        
}

int jsonWrapper::updateDoc(std::string jsonString){

    if (jsonDoc.Parse(jsonString.c_str()).HasParseError()){
        std::cerr << "[jsonWrapper] Parsing rapidjson error. Unable to parse json string" << std::endl; 
    }

    if(jsonDoc.IsObject()){
        isobjectok = true;
        return 0;
    }else{
        isobjectok = false;
        return -1;
    }
    
    
}

int jsonWrapper::parseValue(std::string field, std::string value){
    if(isobjectok){
        
        if(jsonDoc.HasMember(field.c_str())){
            const char *t_value = value.c_str();
            rapidjson::Value a;
            a.SetString(t_value, std::strlen(t_value), this->jsonDoc.GetAllocator());
            jsonDoc[field.c_str()] = a;
        }else{
            std::cerr << "[jsonWrapper] Not valid field name" << std::endl;
            return -1;
        }
    }else{
        std::cerr << "[jsonWrapper] The json document is NOT initialized" << std::endl;
        return -2;
    }


    return 0;

}

int jsonWrapper::parseValue(std::string field, std::vector<double> value){
    
    if(isobjectok){
        if(jsonDoc.HasMember(field.c_str())){
            rapidjson::Value t_a(rapidjson::kArrayType);
            rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();
            for(int j= 0; j<(int)value.size(); j++){
                t_a.PushBack(value[j],allocator);
            }
            if(!jsonDoc[field.c_str()].IsArray()){
                jsonDoc[field.c_str()].SetArray();
            }
            jsonDoc[field.c_str()]=t_a;
        }else{
            std::cerr << "[jsonWrapper] Not valid field name" << std::endl;
            return -1;
        }
    }else{
        std::cerr << "[jsonWrapper] The json document is NOT initialized" << std::endl;
        return -2;
    }

    return 0;

}

std::string jsonWrapper::getAsString(){
    t_buffer.Clear();
    t_writer.Reset(t_buffer);
    jsonDoc.Accept(t_writer);

    return t_buffer.GetString();
}


jsonWrapper::~jsonWrapper(){
    isobjectok = false;
    t_buffer.Clear();
}
