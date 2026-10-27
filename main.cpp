#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Exception.h"
#include <iostream>
#include <string>
#include <cstring>

using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;

// Helper function to extract a header value (like "Location")
std::string getHeaderValue(const std::string& response, const std::string& headerName) {
    std::string searchKey = headerName + ": ";
    size_t pos = response.find(searchKey);
    if (pos != std::string::npos) {
        size_t start = pos + searchKey.length();
        size_t end = response.find("\r\n", start);
        if (end != std::string::npos) {
            return response.substr(start, end - start);
        }
    }
    return "";
}

// Helper function to extract a port number from a URL string (e.g., "http://example.com:8080/path" -> 8080)
int extractPortFromUrl(const std::string& url) {
    // Look for a colon after the protocol (e.g., after "http://")
    size_t protocolEnd = url.find("://");
    size_t searchStart = (protocolEnd != std::string::npos) ? protocolEnd + 3 : 0;
    
    size_t colonPos = url.find(':', searchStart);
    if (colonPos != std::string::npos) {
        size_t slashPos = url.find('/', colonPos);
        std::string portStr = url.substr(colonPos + 1, (slashPos != std::string::npos ? slashPos : url.length()) - (colonPos + 1));
        try {
            return std::stoi(portStr);
        } catch (...) {
            return -1;
        }
    }
    return -1; // No custom port specified in URL (defaults to 80 for http)
}

int main() {
    try {
        std::string host = "example.com";
        SocketAddress address(host, 80);
        StreamSocket socket;
        
        std::cout << "Connecting to " << host << " on port 80..." << std::endl;
        socket.connect(address);

        // 1. Print the Socket's Destination Port directly from the connection
        SocketAddress peer = socket.peerAddress();
        std::cout << "-> Socket Destination IP: " << peer.host().toString() << std::endl;
        std::cout << "-> Socket Destination Port: " << peer.port() << std::endl; // Will print 80

        // 2. Send HTTP request
        std::string request = "GET / HTTP/1.1\r\n"
                              "Host: " + host + "\r\n"
                              "Connection: close\r\n\r\n";
        socket.sendBytes(request.data(), request.length());

        // 3. Receive the response message
        char buffer[4096];
        std::memset(buffer, 0, sizeof(buffer));
        int n = socket.receiveBytes(buffer, sizeof(buffer) - 1);

        if (n > 0) {
            std::string response(buffer, n);

            // 4. Check if the message contains a redirect "Location" header with a destination port
            std::string locationHeader = getHeaderValue(response, "Location");
            if (!locationHeader.empty()) {
                std::cout << "-> Location Header found: " << locationHeader << std::endl;
                int messagePort = extractPortFromUrl(locationHeader);
                if (messagePort != -1) {
                    std::cout << "-> Destination Port parsed from message: " << messagePort << std::endl;
                } else {
                    std::cout << "-> No explicit port in URL (uses default HTTP port 80/443)." << std::endl;
                }
            } else {
                std::cout << "-> No 'Location' header in the message." << std::endl;
            }
        }

    } catch (const Poco::Exception& exc) {
        std::cerr << "Error: " << exc.displayText() << std::endl;
    }

    return 0;
}