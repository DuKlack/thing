#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "HTTP_cache.hpp"

bool HTTP_cache::add_header(std::vector<HeaderEntry> newHeader){
    if (this->Handle_Singleton_Header_conflict(newHeader) || this->Handle_Mutually_Exclusive_Headers(newHeader)) {
        return false; 
    }


    for (const auto& h : newHeader) {
        // Check if the header already exists in the cache (case-insensitive)
        auto target = find_header(this->header, h.key);
        auto isSignleton=std::find_if(singletons.begin(), singletons.end(), [&h](const std::string& singleton_name) {
            return singleton_name == h.key; // Compare string directly with header key
        });
        
        if (target != this->header.end()&&isSignleton!=singletons.end()) {
            // UPDATE existing header in place
            target->value = h.value;
            std::cout << "a";
        } else {
            // INSERT new header (Fixed missing semicolon here)
            this->header.push_back(h);
        }
        
    }
    this->last_accessed=TIME_UTC;
    this->access_count++;
    return true;
}

    
void HTTP_cache::remove_header(std::string key){
    auto target=find_header(this->header,key);
    if (target== this->header.end()){
        return;
    }
    this->header.erase(target);
    this->last_accessed=TIME_UTC;
}
 
//(Singleton Headers:Content-Length, Host,Transfer-Encoding,Content-Type,Strict-Transport-Security,Access-Control-Allow-Origin.Access-Control-Allow-Credentials,X-Frame-Options,X-Content-Type-Options,
//Authorization,Proxy-Authorization,Location,Content-Range,Content-Disposition,ETag,Last-Modified,Retry-After

bool HTTP_cache::Handle_Singleton_Header_conflict(const std::vector<HeaderEntry> newHeader) {
    for (size_t i = 0; i < newHeader.size(); ++i) {
        auto is_singleton = std::find(singletons.begin(), singletons.end(), newHeader[i].key);
        if (is_singleton != singletons.end()) {
            // Check if this same singleton appears again later in the exact same batch
            for (size_t j = i + 1; j < newHeader.size(); ++j) {
                if (newHeader[j].key == newHeader[i].key) {
                    return true; // Conflict: Duplicate singleton in the same batch!
                }
            }
        }
    }
    return false; // No batch conflicts
}
bool HTTP_cache::Handle_Mutually_Exclusive_Headers(const std::vector<HeaderEntry> newHeaders) {
    
    // Iterate through each incoming header in the batch
    for (const auto& newHeader : newHeaders) {
        std::string incomingKey = to_lower(newHeader.key);
        std::string incomingValue = to_lower(newHeader.value);

        // ====================================================================
        // STEP 1: Check the INCOMING header's internal values/self-contradictions
        // ====================================================================
        if (incomingKey == "cache-control") {
            bool has_no_store = (incomingValue.find("no-store") != std::string::npos);
            bool has_max_age  = (incomingValue.find("max-age") != std::string::npos);
            bool has_no_cache = (incomingValue.find("no-cache") != std::string::npos);
            bool has_immutable = (incomingValue.find("immutable") != std::string::npos);

            if (has_no_store && has_max_age) {
            

                return true;
            }
            if (has_no_cache && has_immutable) {
                return true;
            }
        }

        // ====================================================================
        // STEP 2: Check incoming header AGAINST this->header using find_header()
        // ====================================================================

        // 1. Framing Conflict: Content-Length vs Transfer-Encoding: chunked (RFC 9112)
        auto found_te = find_header(this->header, "transfer-encoding");
        if (incomingKey == "content-length" && found_te != this->header.end()) {
            return true;
        }
        
        if (incomingKey == "transfer-encoding" && incomingValue.find("chunked") != std::string::npos) {
            auto found_cl = find_header(this->header, "content-length");
            if (found_cl != this->header.end()) {
                // Strip / remove existing Content-Length from cache container
                this->header.erase(found_cl);
            }
        }

        // 2. Conditional Requests: If-None-Match vs If-Modified-Since (RFC 9110)
        auto found_inm = find_header(this->header, "if-none-match");
        auto found_ims = find_header(this->header, "if-modified-since");

        if (incomingKey == "if-modified-since" && found_inm != this->header.end()) {
            return true; // Drop incoming If-Modified-Since because ETag takes precedence
        }
        
        if (incomingKey == "if-none-match" && found_ims != this->header.end()) {
            // Remove existing timestamp header since ETag takes precedence
            this->header.erase(found_ims);
        }

        // 3. CORS Security Conflict: Wildcard Origin vs Allow Credentials
        auto found_creds = find_header(this->header, "access-control-allow-credentials", "true");
        if (incomingKey == "access-control-allow-origin" && incomingValue == "*") {
            if (found_creds != this->header.end()) {
             
                return true;
            }
        }

        if (incomingKey == "access-control-allow-credentials" && incomingValue == "true") {
            auto found_wildcard_origin = find_header(this->header, "access-control-allow-origin", "*");
            if (found_wildcard_origin != this->header.end()) {
                return true;
            }
        }

        // 4. Connection vs Upgrade (Hop-by-hop conflict)
        if (incomingKey == "connection" && incomingValue == "close") {
            auto found_upgrade = find_header(this->header, "upgrade");
            if (found_upgrade != this->header.end()) {
                this->header.erase(found_upgrade);
            }
        }
    }

    return false; // No mutual exclusion conflict found across the batch
}
bool HTTP_cache::FromJson(const rapidjson::Value& obj) {
        if (!obj.IsObject() || !obj.HasMember("IP_address")) {
            return false;
        }

        const auto& ip_address = obj["IP_address"];
        if (!ip_address.IsObject()) return false;

        // 1. Extract IPv4
        if (ip_address.HasMember("IP") && ip_address["IP"].IsUint()) {
            IPv4 = ip_address["IP"].GetUint();
        }

        // 2. Extract Host Object
        if (ip_address.HasMember("Host") && ip_address["Host"].IsObject()) {
            const auto& hostObj = ip_address["Host"];

            // Extract host string
            if (hostObj.HasMember("Host") && hostObj["Host"].IsString()) {
                host = hostObj["Host"].GetString();
            }

            // 3. Extract HTTP File Object
            if (hostObj.HasMember("HTTP") && hostObj["HTTP"].IsObject()) {
                const auto& httpFile = hostObj["HTTP"];

                // Access count
                if (httpFile.HasMember("access_count") && httpFile["access_count"].IsUint64()) {
                    access_count = httpFile["access_count"].GetUint64();
                }

                // Timestamps
                if (httpFile.HasMember("last_accessed") && httpFile["last_accessed"].IsInt64()) {
                    last_accessed = static_cast<std::time_t>(httpFile["last_accessed"].GetInt64());
                }
                if (httpFile.HasMember("Last_Modified") && httpFile["Last_Modified"].IsInt64()) {
                    Last_Modified = static_cast<std::time_t>(httpFile["Last_Modified"].GetInt64());
                }
                if (httpFile.HasMember("Expiration_Time") && httpFile["Expiration_Time"].IsInt64()) {
                    Expiration_Time = static_cast<std::time_t>(httpFile["Expiration_Time"].GetInt64());
                }

                // Headers Array
                if (httpFile.HasMember("header") && httpFile["header"].IsArray()) {
                    header.clear();
                    const auto& headerArray = httpFile["header"];
                    for (rapidjson::SizeType i = 0; i < headerArray.Size(); ++i) {
                        header.push_back(HeaderEntry::FromJson(headerArray[i]));
                    }
                }

                // Body (stored as string, converted back to std::vector<char>)
                if (httpFile.HasMember("body") && httpFile["body"].IsString()) {
                    const char* bodyStr = httpFile["body"].GetString();
                    rapidjson::SizeType bodyLen = httpFile["body"].GetStringLength();
                    body.assign(bodyStr, bodyStr + bodyLen);
                }
            }
        }

        return true;
    }


//testing, Testing  


void test_header_entry_normalization() {
    std::cout << "Running test_header_entry_normalization..." << std::endl;
    HeaderEntry entry("Content-Type", "application/JSON; charset=UTF-8");
    assert(entry.key == "content-type");
    assert(entry.value == "application/JSON; charset=UTF-8"); 
    std::cout << "[PASSED] test_header_entry_normalization\n" << std::endl;
}

void test_http_cache_initialization() {
    std::cout << "Running test_http_cache_initialization..." << std::endl;
    HTTP_cache cache;
 
    assert(cache.header.empty()); // Fixed from headers to header
    assert(cache.body.empty());
    std::cout << "[PASSED] test_http_cache_initialization\n" << std::endl;
}

void test_add_and_remove_headers() {
    std::cout << "Running test_add_and_remove_headers..." << std::endl;
    HTTP_cache cache;
    
    std::vector<HeaderEntry> new_headers = {
        {"Authorization", "Bearer Token123XYZ"},
        {"X-Custom-Header", "ValueWithCase"}
    };
    
    cache.add_header(new_headers);
    assert(cache.header.size() == 2); // Fixed from headers to header
    assert(cache.header[0].key == "authorization");
    assert(cache.header[0].value == "Bearer Token123XYZ"); 
    
    cache.remove_header("AUTHORIZATION");
    assert(cache.header.size() == 1);
    assert(cache.header[0].key == "x-custom-header");
    std::cout << "[PASSED] test_add_and_remove_headers\n" << std::endl;
}
void test_singleton_conflict_handler() {
    std::cout << "Running test_singleton_conflict_handler..." << std::endl;
    
    // Test 1: Valid batch with unique singleton headers (should succeed)
    HTTP_cache cache1;
    std::vector<HeaderEntry> valid_batch = {
        {"Content-Length", "500"},
        {"Host", "example.com"},
        {"Content-Type", "application/json"}
    };
    cache1.add_header(valid_batch);
    assert(cache1.header.size() == 3);

    // Test 2: Invalid batch with duplicate singleton headers in the same payload (should be blocked)
    HTTP_cache cache2;
    std::vector<HeaderEntry> duplicate_singleton_batch = {
        {"Content-Length", "500"},
        {"Content-Length", "200"} // Conflict: duplicate singleton in batch
    };
    cache2.add_header(duplicate_singleton_batch);
    assert(cache2.header.empty()); // Blocked by Handle_Singleton_Header_conflict

    std::cout << "[PASSED] test_singleton_conflict_handler\n" << std::endl;
}

void test_mutually_exclusive_headers_conflict() {
    std::cout << "Running test_mutually_exclusive_headers_conflict..." << std::endl;
    
    // Test 1: Cache-Control self-contradiction (no-store + max-age)
    HTTP_cache cache1;
    std::vector<HeaderEntry> bad_cache_control = {
        {"Cache-Control", "no-store, max-age=3600"}
    };
    cache1.add_header(bad_cache_control);
    assert(cache1.header.empty()); // Should be dropped/blocked by mutual exclusion check

    // Test 2: Framing conflict (Adding Content-Length when Transfer-Encoding: chunked is cached)
    HTTP_cache cache2;
    // Pre-seed cache with chunked transfer encoding
    cache2.header.push_back(HeaderEntry("Transfer-Encoding", "chunked"));
    
    std::vector<HeaderEntry> incoming_content_length = {
        {"Content-Length", "1024"}
    };
    cache2.add_header(incoming_content_length);
    
    // Content-Length must be rejected due to existing Transfer-Encoding: chunked
    auto cl_target = find_header(cache2.header, "content-length");
    assert(cl_target == cache2.header.end());

    // Test 3: Conditional request conflict (If-Modified-Since added when If-None-Match exists)
    HTTP_cache cache3;
    cache3.header.push_back(HeaderEntry("If-None-Match", "\"xyz123\""));
    
    std::vector<HeaderEntry> incoming_ims = {
        {"If-Modified-Since", "Wed, 21 Oct 2015 07:28:00 GMT"}
    };
    cache3.add_header(incoming_ims);
    
    // If-Modified-Since should be blocked because ETag/If-None-Match takes precedence
    auto ims_target = find_header(cache3.header, "if-modified-since");
    assert(ims_target == cache3.header.end());

    std::cout << "[PASSED] test_mutually_exclusive_headers_conflict\n" << std::endl;
}

int main() {
    std::cout << "=== STARTING HTTP CACHE TESTS ===\n" << std::endl;
    try {
        test_header_entry_normalization();
        test_http_cache_initialization();
        test_add_and_remove_headers();
        
        test_singleton_conflict_handler();
        test_mutually_exclusive_headers_conflict();
        std::cout << "=== ALL TESTS PASSED SUCCESSFULLY ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "TEST FAILED WITH EXCEPTION: " << e.what() << std::endl;
        return 1;
    }
       
}