#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <ctime>
#include "httplib.h"

namespace fs = std::filesystem;

// Helper function to extract and record files from incoming requests
// Helper function to extract and record files from incoming requests
void save_request_files(const httplib::Request& req) {
    std::string output_dir = "./captured_files";
    if (!fs::exists(output_dir)) {
        fs::create_directories(output_dir);
    }

    // 1. Handle multipart/form-data file uploads via req.form
    if (req.is_multipart_form_data()) {
        // req.form contains the parsed multipart data
        // You can check specific file fields or iterate through them.
        // If you know the field name, use req.form.get_file_value("fieldname").
        // For a general approach, we can check common fields or process the body/form parameters.
        
        // As a general fallback for generic catch-all file captures via form:
        // Let's inspect standard fields or check if files exist.
        // Alternatively, if you want to inspect specific files:
        // Example: if (req.form.has_file("file")) { const auto& file = req.form.get_file_value("file"); ... }
        
        // Let's iterate or handle generic raw body/form payloads gracefully:
        std::cout << "[INFO] Multipart form data received, but traversal depends on field names. Check req.form documentation for specific field mapping.\n";
    }
    
    // 2. Handle raw body payloads (e.g., PUT or raw POST streams, binary uploads)
    if (!req.body.empty()) {
        std::string filename = "payload_" + std::to_string(std::time(nullptr)) + ".bin";
        
        // If the request path contains a filename, attempt to use it
        fs::path p(req.path);
        if (p.has_filename() && p.filename().string() != "/") {
            filename = p.filename().string();
        }

        std::string full_path = output_dir + "/" + filename;
        std::ofstream ofs(full_path, std::ios::binary);
        ofs.write(req.body.data(), req.body.size());

        std::cout << "[SAVED BODY PAYLOAD] " << full_path 
                  << " (" << req.body.size() << " bytes)\n";
    }
}
int main() {
    httplib::Server svr;

    // Catch-all route for GET requests (downloads/page views)
    svr.Get(R"(.*)", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[GET Request] Path: " << req.path 
                  << " | Client IP: " << req.get_header_value("REMOTE_ADDR") << std::endl;
        res.set_content("HTTP Recorder Active", "text/plain");
    });

    // Catch-all route for POST requests (file uploads)
    svr.Post(R"(.*)", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[POST Request] Path: " << req.path << std::endl;
        save_request_files(req);
        res.set_content("File recorded successfully.", "text/plain");
    });

    // Catch-all route for PUT requests (file uploads/transfers)
    svr.Put(R"(.*)", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[PUT Request] Path: " << req.path << std::endl;
        save_request_files(req);
        res.set_content("File recorded successfully.", "text/plain");
    });

    int port = 80;
    std::cout << "Starting HTTP file recorder on port " << port << "...\n";
    
    // Bind and listen on all interfaces for port 80
    if (!svr.listen("0.0.0.0", port)) {
        std::cerr << "Error: Failed to bind to port " << port << ". "
                  << "Note that binding to port 80 requires root / Administrator privileges.\n";
        return 1;
    }

    return 0;
}