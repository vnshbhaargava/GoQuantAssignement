#include <iostream>
#include <string>
#include <curl/curl.h>
#include "include/json.hpp"
#include <cstdlib> // For getenv

using json = nlohmann::json;

// Constants for API URLs
const std::string BASE_URL = "https://test.deribit.com/api/v2";

// Function to handle the response from the cURL request
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// General function to send a cURL request with optional access token
std::string sendRequest(const std::string& url, const json& payload, const std::string& accessToken = "") {
    std::string readBuffer;
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        std::cout << "Sending request to URL: " << url << std::endl;
        std::cout << "Payload: " << payload.dump(4) << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);  // Set the HTTP method to POST

        // Set the request payload
        std::string jsonStr = payload.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

        // Set headers, including Authorization if accessToken is provided
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (!accessToken.empty()) {
            headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
            std::cout << "Authorization Header: Bearer " << accessToken << std::endl;
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        // Add HTTP status code check
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (res != CURLE_OK || http_code != 200) {
            std::cerr << "Request failed: " << curl_easy_strerror(res)
                      << " | HTTP Code: " << http_code << std::endl;
        } else {
            std::cout << "Request succeeded. HTTP Code: " << http_code << std::endl;
        }

        // Debug: Print raw response
        std::cout << "Raw Response: " << readBuffer << std::endl;

        // Free Resources
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return readBuffer;
}

// Function to get the access token
std::string getAccessToken(const std::string& clientId, const std::string& clientSecret) {
    json payload = {
        {"id", 0},
        {"method", "public/auth"},
        {"params", {
            {"grant_type", "client_credentials"},
            {"scope", "session:apiconsole-c5i26ds6dsr"},
            {"client_id", clientId},
            {"client_secret", clientSecret}
        }},
        {"jsonrpc", "2.0"}
    };

    std::cout << "Getting access token..." << std::endl;
    std::cout << "Auth Payload: " << payload.dump(4) << std::endl;

    std::string response = sendRequest(BASE_URL + "/public/auth", payload);

    try {
        auto responseJson = json::parse(response);
        std::cout << "Auth Response JSON: " << responseJson.dump(4) << std::endl;
        if (responseJson.contains("result") && responseJson["result"].contains("access_token")) {
            std::cout << "Access token retrieved successfully." << std::endl;
            return responseJson["result"]["access_token"];
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }

    std::cerr << "Failed to retrieve access token. Response: " << response << std::endl;
    return "";
}

// Function to place an order
void placeOrder(const std::string& price, const std::string& accessToken, const std::string& amount, const std::string& instrument) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/buy"},
        {"params", {
            {"instrument_name", instrument},
            {"type", "limit"},
            {"price", price},
            {"amount", amount}
        }},
        {"id", 1}
    };

    std::cout << "Placing order..." << std::endl;
    std::cout << "Order Payload: " << payload.dump(4) << std::endl;

    std::string response = sendRequest(BASE_URL + "/private/buy", payload, accessToken);

    try {
        auto responseJson = json::parse(response);
        std::cout << "Order Response JSON: " << responseJson.dump(4) << std::endl;
        if (responseJson.contains("error")) {
            std::cerr << "Error placing order: " << responseJson["error"]["message"] << std::endl;
        } else {
            std::cout << "Order placed successfully: " << responseJson.dump(4) << std::endl;
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
}

// Function to cancel an order
void cancelOrder(const std::string& accessToken, const std::string& orderID) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/cancel"},
        {"params", {{"order_id", orderID}}},
        {"id", 6}
    };

    std::string response = sendRequest(BASE_URL + "/private/cancel", payload, accessToken);

    try {
        auto responseJson = json::parse(response);
        if (responseJson.contains("error")) {
            std::cerr << "Error canceling order: " << responseJson["error"]["message"] << std::endl;
        } else {
            std::cout << "Order canceled successfully: " << responseJson.dump(4) << std::endl;
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
}

// Function to modify an order
void modifyOrder(const std::string& accessToken, const std::string& orderID, int amount, double price) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/edit"},
        {"params", {
            {"order_id", orderID},
            {"amount", amount},
            {"price", price}
        }},
        {"id", 11}
    };

    std::string response = sendRequest(BASE_URL + "/private/edit", payload, accessToken);

    try {
        auto responseJson = json::parse(response);
        if (responseJson.contains("error")) {
            std::cerr << "Error modifying order: " << responseJson["error"]["message"] << std::endl;
        } else {
            std::cout << "Order modified successfully: " << responseJson.dump(4) << std::endl;
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
}

// Function to retrieve the order book
void getOrderBook(const std::string& instrument) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "public/get_order_book"},
        {"params", {{"instrument_name", instrument}}},
        {"id", 15}
    };

    std::string response = sendRequest(BASE_URL + "/public/get_order_book", payload);

    try {
        auto responseJson = json::parse(response);
        if (responseJson.contains("result")) {
            std::cout << "Order Book for " << instrument << ":\n\n";
            std::cout << "Best Bid Price: " << responseJson["result"]["best_bid_price"] << ", Amount: " << responseJson["result"]["best_bid_amount"] << '\n';
            std::cout << "Best Ask Price: " << responseJson["result"]["best_ask_price"] << ", Amount: " << responseJson["result"]["best_ask_amount"] << '\n';
        } else {
            std::cerr << "Error retrieving order book." << std::endl;
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
}

// Main function
int main() {
    // Retrieve client credentials from environment variables
    const char* clientId = std::getenv("CLIENT_ID");
    const char* clientSecret = std::getenv("CLIENT_SECRET");

    if (!clientId || !clientSecret) {
        std::cerr << "Error: CLIENT_ID and CLIENT_SECRET must be set in environment variables\n";
        return 1;
    }

    std::cout << "CLIENT_ID: " << clientId << "\nCLIENT_SECRET: " << clientSecret << std::endl;

    // Retrieve the access token
    std::string accessToken = getAccessToken(clientId, clientSecret);

    if (!accessToken.empty()) {
        // Example usage
        placeOrder("50000", accessToken, "1", "ETH-PERPETUAL");
    } else {
        std::cerr << "Failed to obtain access token\n";
        return 1;
    }

    return 0;
}
