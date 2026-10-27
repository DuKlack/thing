#include <iostream>
#include <string>
#include <unordered_map>
#include <ctime>
#include<vector>
#include <optional>
#include <fstream>
#include <cstdint> 
#include<pthread.h>
#include<string>
#include <algorithm>
#include "HTTP_cache.hpp"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include "Cache.hpp"


std::optional<HTTP_cache> Cache::Find_in_Log(uint32_t target_ip, std::string target_host){
    std::fstream storage("Cache.json");
    if (!storage.is_open()) {
        std::cerr << "Failed to open file!" << std::endl;
        return std::nullopt;
    }
    rapidjson::IStreamWrapper isw(storage);
    rapidjson::Document doc;

    doc.ParseStream(isw);
    storage.close();
    if (doc.HasParseError()) {
        std::cerr << "JSON parse error!" << std::endl;
        return std::nullopt;
    }
    auto matches_target = [target_ip, &target_host](const rapidjson::Value& obj) {
        if (!obj.IsObject() || !obj.HasMember("IP_address")) return false;
        
        const auto& ip_addr = obj["IP_address"];
        if (!ip_addr.IsObject()) return false;

        // Check IP
        if (!ip_addr.HasMember("IP") || ip_addr["IP"].GetUint() != target_ip) {
            return false;
        }

        // Check Host
        if (ip_addr.HasMember("Host") && ip_addr["Host"].IsObject()) {
            const auto& hostObj = ip_addr["Host"];
            if (hostObj.HasMember("Host") && hostObj["Host"].IsString()) {
                std::string fileHost = hostObj["Host"].GetString();
                if (fileHost == target_host) {
                    return true;
                }
            }
        }
        return false;
    };

    // 3. Search handling (supports both an Array of cache objects or a Single cache object)
    if (doc.IsArray()) {
        for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
            if (matches_target(doc[i])) {
                HTTP_cache cache;
                if (cache.FromJson(doc[i])) {
                    return cache;
                }
            }
        }
    } else if (doc.IsObject()) {
        if (matches_target(doc)) {
            HTTP_cache cache;
            if (cache.FromJson(doc)) {
                return cache;
            }
        }
    }

    return std::nullopt; // Not found
}


