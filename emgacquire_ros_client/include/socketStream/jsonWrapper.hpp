/**
 *  Copyright (C) 2020 Iason Batzianoulis
 * 
 *  jsonWrapper C++ class
 *  
 *  [description]
 *  
 *  Developer:  Iason Batzianoulis
 *  email:      iasonbatz@gmail.com
 *  License:    GNU GPLv3
 * 
**/

#ifndef JSONWRAPPER_H_
#define JSONWRAPPER_H_

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>



namespace rapidJson_types{
    typedef int                                 Int;
    typedef float                               Float;
    typedef double                              Double;           
    typedef std::vector<double>                 VecD;
    typedef std::vector< std::vector<double> >  Mat2DD;
    typedef std::vector<float>                  VecFt;
    typedef std::vector< std::vector<float> >   Mat2DFt;
    typedef std::vector<int>                    VecInt;
    typedef std::vector< std::vector<int> >     Mat2DInt;
    typedef std::string                         String;
    typedef std::vector< std::string >          VecString;

    enum rapdJson_types{
    RAPIDJSON_NULL,
    RAPIDJSON_FALSE,
    RAPIDJSON_TRUE,
    RAPIDJSON_OBJECT,
    RAPIDJSON_ARRAY,
    RAPIDJSON_STRING,
    RAPIDJSON_NUMBER

    };
}



class jsonWrapper{

    rapidjson::Document jsonDoc;

    rapidjson::StringBuffer t_buffer;
    rapidjson::Writer<rapidjson::StringBuffer> t_writer;

    bool isobjectok;

    template <class T> 
    struct getValueHelper;


    public:

        jsonWrapper();
        ~jsonWrapper();
        jsonWrapper(std::string jsonString);

        int updateDoc(std::string jsonString);
        int parseValue(std::string field, std::string value);
        int parseValue(std::string field, std::vector<double> value);
        void printValue(std::string fldName){std::cout<< jsonDoc[fldName.c_str()].GetString() << std::endl; }
        std::string getAsString();

        template<class T>
        T getField(std::string fieldName);


};

template<>
struct jsonWrapper::getValueHelper<rapidJson_types::Int>{
    static rapidJson_types::Int getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        return (int)(tmp->jsonDoc[fldName.c_str()].GetInt());
    }
};

template<>
struct jsonWrapper::getValueHelper<rapidJson_types::Float>{
    static rapidJson_types::Float getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        return (tmp->jsonDoc[fldName.c_str()].GetFloat());
    }
};

template<>
struct jsonWrapper::getValueHelper<rapidJson_types::Double>{
    static rapidJson_types::Double getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        return (tmp->jsonDoc[fldName.c_str()].GetDouble());
    }
};


template<>
struct jsonWrapper::getValueHelper<rapidJson_types::String>{
    static rapidJson_types::String getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        return tmp->jsonDoc[fldName.c_str()].GetString();
    }
};


template<>
struct jsonWrapper::getValueHelper<rapidJson_types::VecD>{
    static rapidJson_types::VecD getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        rapidJson_types::VecD trm;
        const rapidjson::Value& a = tmp->jsonDoc[fldName.c_str()];
        std::transform(a.Begin(),a.End(),std::back_inserter(trm),[](const rapidjson::Value &tt){return tt.GetDouble();});
        return trm;
    }
};

template<>
struct jsonWrapper::getValueHelper<rapidJson_types::VecFt>{
    static rapidJson_types::VecFt getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        rapidJson_types::VecFt trm;
        const rapidjson::Value& a = tmp->jsonDoc[fldName.c_str()];
        std::transform(a.Begin(),a.End(),std::back_inserter(trm),[](const rapidjson::Value &tt){return tt.GetFloat();});
        return trm;
    }
};

template<>
struct jsonWrapper::getValueHelper<rapidJson_types::VecInt>{
    static rapidJson_types::VecInt getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        rapidJson_types::VecInt trm;
        const rapidjson::Value& a = tmp->jsonDoc[fldName.c_str()];
        std::transform(a.Begin(),a.End(),std::back_inserter(trm),[](const rapidjson::Value &tt){return (int)tt.GetDouble();});
        return trm;
    }
};

template<>
struct jsonWrapper::getValueHelper<rapidJson_types::Mat2DD>{
    static rapidJson_types::Mat2DD getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        const rapidjson::Value& a = tmp->jsonDoc[fldName.c_str()];
        rapidJson_types::Mat2DD trm(a.Size());
        for (int count = 0; count < (int)trm.size(); count++){
            std::transform(a[count].Begin(),a[count].End(),std::back_inserter(trm[count]),[](const rapidjson::Value &tt) { return tt.GetDouble(); } );
        }
        return trm;
    }
};

template<>
struct jsonWrapper::getValueHelper<rapidJson_types::Mat2DFt>{
    static rapidJson_types::Mat2DFt getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        const rapidjson::Value& a = tmp->jsonDoc[fldName.c_str()];
        rapidJson_types::Mat2DFt trm(a.Size());
        for (int count = 0; count < (int)trm.size(); count++){
            std::transform(a[count].Begin(),a[count].End(),std::back_inserter(trm[count]),[](const rapidjson::Value &tt) { return tt.GetFloat(); } );
        }
        return trm;
    }
};

template<>
struct jsonWrapper::getValueHelper<rapidJson_types::Mat2DInt>{
    static rapidJson_types::Mat2DInt getVal(void *usrPtr, std::string fldName){
        jsonWrapper *tmp = static_cast<jsonWrapper *>(usrPtr);
        const rapidjson::Value& a = tmp->jsonDoc[fldName.c_str()];
        rapidJson_types::Mat2DInt trm(a.Size());
        for (int count = 0; count < (int)trm.size(); count++){
            std::transform(a[count].Begin(),a[count].End(),std::back_inserter(trm[count]),[](const rapidjson::Value &tt) { return (int)tt.GetDouble(); } );
        }
        return trm;
    }
};


template<class T>
T jsonWrapper::getField(std::string fieldName){
    if(!jsonDoc.HasMember(fieldName.c_str())){
        std::cerr << "[jsonWrapper] The json document doesn't contain a field with the name \"" << fieldName << "\"" << std::endl;
        abort();
    }
    return jsonWrapper::getValueHelper<T>::getVal(this, fieldName);
}



#endif