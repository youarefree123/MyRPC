#pragma once 

#include "noncopyable.h"
#include "log.h"
#include <unordered_map>
#include <string>
#include <filesystem>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace mrpc {


class Config : public noncopyable {
public:
    void Init( const char* ini_path );
    void Format();


private:
    std::unordered_map<std::string, std::string> m_config;
};


void Config::Init( const char* ini_path ) {
  
    if (!std::filesystem::exists(ini_path)) {
        CRITICAL( "{} not exists.", ini_path );
    }

    boost::property_tree::ptree root, tag;
    boost::property_tree::ini_parser::read_ini(ini_path, root);
    tag = root.get_child("config");
    
    m_config[ "rpc_ip" ] = tag.get<std::string>( "rpc_ip" );
    m_config[ "rpc_port" ] = tag.get<std::string>( "rpc_port" );
    m_config[ "zk_ip" ] = tag.get<std::string>( "zk_ip" );
    m_config[ "zk_port" ] = tag.get<std::string>( "zk_port" );
    
}

void Config::Format() {
    for( auto& [k,v] : m_config ) {
        std::cout<<k<<":"<<v<<std::endl;
    }
}


} // mrpc

