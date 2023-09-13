#pragma once

#include "noncopyable.h"
#include <unordered_map>
#include <string>
#include <filesystem>
#include <optional>
#include <iostream> // fixbug 
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace mrpc {


class Config : public noncopyable {
public:
    // 因为函数定义都放在.h中了，所以要加inline， log.h 同理
    inline bool Init( const char* ini_path );
    inline void Format();
    static Config& Instance() {
        static Config conf;
        return conf;
    }
    inline std::optional<std::string> Load( const char* ) ;
    inline std::optional<std::string> Load( std::string&& );

private:
    Config() = default;
    ~Config() = default;
    std::unordered_map<std::string, std::string> m_config;
};


 bool Config::Init( const char* ini_path ) {
  
    if (!std::filesystem::exists(ini_path)) {
        return false;
    }

    boost::property_tree::ptree root, tag;
    boost::property_tree::ini_parser::read_ini(ini_path, root);
    tag = root.get_child("config");
    
    // 获取config下的每个键值对
    for( const auto& it : tag ) {
        m_config[ it.first ] = it.second.data();
    }

    // Format();

    return true;
    
}

void Config::Format() {
    for( auto& [k,v] : m_config ) {
        std::cout<<k<<":"<<v<<std::endl;
    }
}

std::optional<std::string> Config::Load( const char* key ) {
    return Load( std::string(key) );
}

std::optional<std::string> Config::Load( std::string&& key ) {
    if( m_config.find( key ) != m_config.end() ) {
        return m_config[ key ];
    }
    return std::nullopt;
}


} // mrpc

