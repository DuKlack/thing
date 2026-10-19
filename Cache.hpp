#ifndef CACHE_HPP
#define CACHE_HPP
#define LOG_DESTINATION="/Cache.log"
#define BUFFER_SIZE =5   

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

typedef struct{
    u_int16_t count;
    pthread_mutex_t mutex;
}queue_data;

class Cache{
public :
//std::queue <HTTP_cache> LRU(std::vector<HTTP_cache> storage);    
    std::unordered_map<HTTP_cache,queue_data> buffer;
//Most Frequencly use 
    
    HTTP_cache Find_in_Queue(std::string Host);
    std::optional<HTTP_cache> Find_in_Log (uint32_t target_ip,std::string Host);
    void add_to_queue(HTTP_cache cache);
    void remove_from_queue(HTTP_cache cache);
    void Send_Request();
    void Send_Respsone();
    void write_to_log(HTTP_cache Cache);
private:



};
#endif