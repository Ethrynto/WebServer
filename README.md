
## Description
This is a simple web server, processes the request and outputs the result. Works in asynchronous mode, so it is possible to process several requests simultaneously

## Dependencies
> [!NOTE]
> Require **[CMake](https://cmake.org/), [Boost](https://boost.org), [Vcpkg](https://vcpkg.io/en/)**

## Web Server Documentation
#### Overview
The **WebServer** is a lightweight, asynchronous HTTP server built using **Boost.Asio** for network operations and **ImGui** for a graphical user interface. It serves static files (e.g., `.html`, `.css`) and dynamic PHP scripts from specified directories, supporting multiple projects on different ports. The server is designed to be cross-platform, running on Windows, Linux, and macOS, and is implemented in C++20 with a focus on extensibility and scalability.

#### Features
- **Asynchronous Request Handling**: Utilizes Boost.Asio's `io_context` and `async_accept` for non-blocking TCP connections.
- **Multi-Port Support**: Serves multiple projects, each on a unique port, with distinct root directories.
- **Static and Dynamic Content**: Supports static files (e.g., HTML) and PHP scripts via `php` (Windows) or `php-cgi` (Unix).
- **Graphical Interface**: Includes an ImGui-based GUI for starting/stopping the server and viewing logs.
- **Logging**: Logs events (e.g., connections, errors) to both a GUI panel and a file (`app_logs.txt`).
- **Cross-Platform**: Uses `std::filesystem` and conditional compilation for PHP execution (`php` vs. `php-cgi`).

#### Architecture
The server is composed of the following components:
- **WebServer**: Manages TCP acceptors and coordinates asynchronous connection handling.
- **RequestHandler**: Processes HTTP requests, serving static files or executing PHP scripts.
- **Debug::Log**: Provides logging to a file and GUI.
- **main.cpp**: Initializes projects, sets up the ImGui GUI, and manages the server lifecycle.

##### File Structure
- `WebServer.h`, `WebServer.cpp`: Core server logic for managing connections.
- `RequestHandler.h`, `RequestHandler.cpp`: HTTP request processing and content serving.
- `Log.h` (assumed): Logging utility for debugging and monitoring.
- `main.cpp`: Entry point, project loading, and GUI rendering.
- `CMakeLists.txt`: Build configuration for cross-platform compilation.
- `domains/`: Directory containing project folders (e.g., `Test`, `Test2`) with static and PHP files.

#### Usage
1. **Prerequisites**:
    - Install dependencies: Boost.Asio, ImGui, GLFW, spdlog, fmt (via vcpkg).
    - Install PHP and add it to PATH (e.g., `C:\php` on Windows).
    - Ensure `DOMAINS_PATH` (`D:/Development/C++/WebServer/domains/`) contains project directories with files like `index.html` or `test.php`.

2. **Building**:
   ```bash
   mkdir build
   cd build
   cmake -G Ninja ..
   ninja
   ```

3. **Running**:
    - Execute `./WebServer` (Windows: `WebServer.exe`).
    - In the GUI, click "Start Server" to begin accepting connections.
    - Access projects via `http://localhost:<port>` (e.g., `http://localhost:8080`).

4. **Directory Structure**:
    - Example:
      ```
      D:/Development/C++/WebServer/domains/
      ├── Test/
      │   ├── index.html
      │   ├── test.php
      ├── Test2/
      │   ├── index.html
      ```

#### Implementation Details
##### WebServer
- **Constructor**:
    - Initializes with a reference to `boost::asio::io_context` and a vector of projects (`std::vector<std::pair<int, std::string>>`).
    - Calls `startAccept` for each project to set up TCP acceptors.
- **start()**:
    - Sets `running_ = true` and launches `io_context.run()` in a separate thread (`ioThread_`).
- **stop()**:
    - Sets `running_ = false`, closes all acceptors, stops `io_context`, joins `ioThread_`, and reinitializes acceptors for restart.
- **startAccept(int port, const std::string& rootDir)**:
    - Creates a TCP acceptor for the specified port and a `RequestHandler` for the root directory.
    - Initiates the asynchronous accept loop via `doAccept`.
- **doAccept(...)**:
    - Performs `async_accept` to accept incoming connections.
    - On success, spawns a thread to process the request via `RequestHandler::handleRequest`.
    - Continues the loop by calling itself if `running_ == true`.

##### RequestHandler
- **Constructor**:
    - Takes `io_context` and `rootDir` for file serving.
- **handleRequest(std::shared_ptr<boost::asio::ip::tcp::socket> socket)**:
    - Reads HTTP request until `\r\n\r\n` using `boost::asio::read_until`.
    - Extracts the requested path (e.g., `/`, `/test.php`).
    - Serves static files via `serveStaticFile` or PHP scripts via `handlePhpRequest`.
    - Sends the HTTP response with headers (`HTTP/1.1 200 OK`, `Content-Type: text/html`, `Connection: close`).
- **serveStaticFile(const std::string& path, std::stringstream& responseStream)**:
    - Reads the file content and appends it to the response stream.
    - Returns 404 if the file does not exist.
- **handlePhpRequest(const std::string& path, std::stringstream& requestStream, std::stringstream& responseStream)**:
    - Checks if PHP is available (`php --version` or `php-cgi --version`).
    - Executes the PHP script using `_popen` (`php` for Windows, `php-cgi` for Unix).
    - Captures output and appends it to the response stream.
    - Returns 500 if PHP is not installed or execution fails.

##### Logging
- Uses `Debug::Log::info` and `Debug::Log::error` to log events and errors.
- Logs are displayed in the ImGui GUI and written to `app_logs.txt`.

#### Scalability
To make the server scalable for high loads, consider the following enhancements:
1. **Thread Pool**:
    - Replace `std::thread` with `boost::asio::thread_pool` for request handling:
      ```cpp
      #include <boost/asio/thread_pool.hpp>
      static boost::asio::thread_pool pool(4);
      boost::asio::post(pool, [handler, socket]() { handler->handleRequest(socket); });
      ```
    - Benefits: Reduces thread creation overhead, supports concurrent request processing.
2. **Multiple I/O Threads**:
    - Run `io_context.run()` in multiple threads:
      ```cpp
      std::vector<std::thread> ioThreads;
      for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
          ioThreads.emplace_back([this]() { ioContext_.run(); });
      }
      ```
    - Benefits: Increases throughput for high connection rates.
3. **Keep-Alive Connections**:
    - Add support for `Connection: keep-alive` to handle multiple requests per connection:
      ```cpp
      std::string connectionHeader = requestStream.str().find("Connection: keep-alive") != std::string::npos ? "keep-alive" : "close";
      responseStream << "Connection: " << connectionHeader << "\r\n\r\n";
      if (connectionHeader == "keep-alive") {
          boost::asio::async_read_until(*socket, request, "\r\n\r\n", /* continue reading */);
      }
      ```
    - Benefits: Reduces TCP connection overhead.
4. **File Caching**:
    - Cache frequently accessed static files in memory:
      ```cpp
      static std::unordered_map<std::string, std::string> fileCache;
      if (fileCache.contains(path)) {
          responseStream << fileCache[path];
      } else {
          std::ifstream file(path);
          std::stringstream content;
          content << file.rdbuf();
          fileCache[path] = content.str();
          responseStream << content.str();
      }
      ```
    - Benefits: Speeds up static file serving.
5. **Load Balancing**:
    - Deploy multiple server instances behind a load balancer (e.g., Nginx).
    - Benefits: Distributes traffic across instances for high availability.
6. **HTTPS Support**:
    - Use `boost::asio::ssl::stream` for secure connections:
      ```cpp
      #include <boost/asio/ssl.hpp>
      boost::asio::ssl::context sslContext(boost::asio::ssl::context::tlsv13);
      sslContext.use_certificate_chain_file("server.crt");
      sslContext.use_private_key_file("server.key", boost::asio::ssl::context::pem);
      ```
    - Benefits: Ensures secure communication.
7. **Metrics and Monitoring**:
    - Add Prometheus or custom metrics for request counts, response times, and errors.
    - Benefits: Enables performance monitoring and optimization.

#### Limitations
- **Single-Threaded I/O**: Currently uses one `io_context` thread, limiting scalability under high load.
- **No Keep-Alive**: Each request closes the connection, increasing TCP overhead.
- **Basic Error Handling**: Only supports 200, 404, and 500 HTTP codes.
- **PHP Dependency**: Requires PHP installation, with no fallback for other scripting languages.

#### Testing
1. **Static Files**:
    - Create `index.html` in `domains/Test`:
      ```html
      <h1>Test Page</h1>
      ```
    - Access `http://localhost:8080` and verify the response.
2. **PHP Scripts**:
    - Create `test.php` in `domains/Test`:
      ```php
      <?php echo "<h1>Hello from PHP!</h1>"; ?>
      ```
    - Access `http://localhost:8080/test.php` and verify the output.
3. **Multiple Requests**:
    - Use `curl` to send multiple requests:
      ```bash
      curl http://localhost:8080
      curl http://localhost:8080/test.php
      ```
    - Check logs for repeated `[INFO] Starting async_accept ...` and `[INFO] Accepted connection ...`.

#### Troubleshooting
- **Server Stops After One Request**:
    - Check logs for `[ERROR] Accept error ...` or `[INFO] IO context stopped`.
    - Ensure `running_` is `true` and no exceptions in `doAccept`.
- **PHP Error: `'php' is not recognized`**:
    - Install PHP and add it to PATH (e.g., `C:\php`).
    - Verify with `php --version`.
- **Port Conflicts**:
    - Check with `netstat -a -n -o | find "8080"`.
    - Change ports in `DOMAINS_PATH` subdirectories if needed.

#### Future Enhancements
- Add WebSocket support for real-time applications.
- Implement request routing for REST API endpoints.
- Support additional scripting languages (e.g., Python, Node.js).
- Add configuration file support (e.g., JSON) for ports and directories.

---