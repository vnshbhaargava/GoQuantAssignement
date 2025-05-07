# Deribit API Trading System in C++

This project is a trading system built in C++ for interacting with the Deribit Test API. It allows you to authenticate, place orders, cancel orders, modify orders, and retrieve order book data. The project uses `cURL` for HTTP requests and `nlohmann/json` for JSON parsing.

## Features

- **Authentication**: Retrieve an access token using client credentials.
- **Order Placement**: Submit limit orders for specific instruments with price and quantity.
- **Order Management**: Cancel and modify existing orders.
- **Order Book Retrieval**: View detailed order book data, including bids, asks, and market information.

## Requirements

- **C++ Compiler**: GCC (recommended) or any modern C++ compiler with C++17 support.
- **cURL**: Library for HTTP requests.
- **JSON for Modern C++**: Header-only library for JSON parsing by `nlohmann/json`.

## Dependencies

1. **cURL**: Install via the following commands:
    ```bash
    sudo apt update
    sudo apt install libcurl4-openssl-dev
    ```

2. **JSON for Modern C++**: Download the header file from [nlohmann/json GitHub page](https://github.com/nlohmann/json) or install via a package manager:
    ```bash
    sudo apt install nlohmann-json3-dev
    ```

## Setup

1. Clone the repository to your local machine:
    ```bash
    git clone https://github.com/yourusername/your-repo-name.git
    cd your-repo-name
    ```

2. Ensure the necessary libraries (`curl` and `nlohmann/json`) are installed.

3. Add the path to `json.hpp` if it's not already in your include path:
    - Place `json.hpp` in the `include/` directory within the project.

4. Set up your environment variables for authentication:
    ```bash
    export CLIENT_ID="your_client_id"
    export CLIENT_SECRET="your_client_secret"
    ```

## Compilation

Use the following command to compile the project:

```bash
g++ main.cpp -o trading_system -lcurl -I include
```

## Usage

1. **Run the Program**:
    ```bash
    ./trading_system
    ```

2. **Example Workflow**:
    - The program will:
        1. Authenticate with the Deribit API using your `CLIENT_ID` and `CLIENT_SECRET`.
        2. Retrieve an access token.
        3. Place a limit order for `ETH-PERPETUAL` at a price of `50000` with a quantity of `1`.

3. **Debugging**:
    - The program includes detailed debugging logs to help identify issues with API requests or responses.

## Debugging Logs

- **Authentication**:
    - Logs the payload sent to the `/public/auth` endpoint.
    - Logs the raw response and parsed JSON response.

- **Order Placement**:
    - Logs the payload sent to the `/private/buy` endpoint.
    - Logs the raw response and parsed JSON response.

- **HTTP Status Codes**:
    - Logs the HTTP status code for each request.

## Example Output

### Successful Authentication
```plaintext
Getting access token...
Auth Payload: {
    "id": 0,
    "jsonrpc": "2.0",
    "method": "public/auth",
    "params": {
        "client_id": "your_client_id",
        "client_secret": "your_client_secret",
        "grant_type": "client_credentials",
        "scope": "session:apiconsole-c5i26ds6dsr"
    }
}
Request succeeded. HTTP Code: 200
Auth Response JSON: {
    "jsonrpc": "2.0",
    "result": {
        "access_token": "your_access_token",
        "expires_in": 3600
    },
    "id": 0
}
Access token retrieved successfully.
```

### Successful Order Placement
```plaintext
Placing order...
Order Payload: {
    "id": 1,
    "jsonrpc": "2.0",
    "method": "private/buy",
    "params": {
        "amount": "1",
        "instrument_name": "ETH-PERPETUAL",
        "price": "50000",
        "type": "limit"
    }
}
Request succeeded. HTTP Code: 200
Order Response JSON: {
    "jsonrpc": "2.0",
    "result": {
        "order_id": "123456789",
        "status": "open"
    },
    "id": 1
}
Order placed successfully: {
    "order_id": "123456789",
    "status": "open"
}
```

### Error Handling
- If the request fails, the program will log the error message and HTTP status code:
```plaintext
Request failed: No error | HTTP Code: 400
JSON parse error: [json.exception.parse_error.101] parse error at line 1, column 1: syntax error while parsing value - unexpected end of input; expected '[', '{', or a literal
```

## Notes

- This project is designed to work with the Deribit **Test API**. Ensure you are using the correct credentials and endpoints for the test environment.
- For more information on the Deribit API, refer to the [official documentation](https://docs.deribit.com/).

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
