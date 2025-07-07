#include <filesystem>
#include <vector>
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include "Network/WebServer.h"
#include "Debug/Log.h"
#include "System/HtaccessConfig.h"
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Get path to the executable's directory
std::string getExecutableDir() {
    std::filesystem::path exePath;
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    exePath = std::filesystem::path(buffer).parent_path();
#else
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        exePath = std::filesystem::path(buffer).parent_path();
    } else {
        exePath = std::filesystem::current_path();
    }
#endif
    return exePath.string();
}

// Load configuration from JSON (for future use)
std::string getDomainsPath(const std::string& exeDir) {
//    std::filesystem::path configPath = std::filesystem::path(exeDir) / "resources" / "config.json";
//    std::ifstream configFile(configPath);
//    if (configFile) {
//        nlohmann::json config;
//        configFile >> config;
//        if (config.contains("domains_path")) {
//            return (std::filesystem::path(exeDir) / config["domains_path"].get<std::string>()).string();
//        }
//    }
    return (std::filesystem::path(exeDir) / "domains").string();
}

static void glfw_error_callback(int error, const char* description) {
    Debug::Log::error(std::format("GLFW Error {}: {}", error, description), "GLFW");
}

int main() {
    // Initialize GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        Debug::Log::error("Failed to initialize GLFW", "Main");
        return -1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Web Server", nullptr, nullptr);
    if (!window) {
        Debug::Log::error("Failed to create GLFW window", "Main");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Determine domains path based on executable location
    std::string exeDir = getExecutableDir();
    std::string domainsPath = getDomainsPath(exeDir);
    if (!std::filesystem::exists(domainsPath)) {
        Debug::Log::error(std::format("Domains path does not exist: {}", domainsPath), "Main");
        return -1;
    }

    // Load projects from domains path
    std::vector<std::pair<int, std::string>> projects;
    int defaultPort = 8080;
    for (const auto& entry : std::filesystem::directory_iterator(domainsPath)) {
        if (entry.is_directory()) {
            std::string projectPath = entry.path().string();
            std::filesystem::path htaccessPath = entry.path() / ".htaccess";
            int port = defaultPort++;

            // Parse .htaccess for port
            System::HtaccessConfig config = System::HtaccessConfig::parse(htaccessPath.string());
            if (config.port) {
                port = *config.port;
                Debug::Log::info(std::format("Loaded project: {} on port {}", entry.path().filename().string(), port), "Main");
            } else {
                Debug::Log::warn(std::format("No valid port in .htaccess for {}, using default port {}", projectPath, port), "Main");
            }

            projects.emplace_back(port, projectPath);
        }
    }

    // Initialize WebServer
    boost::asio::io_context ioContext;
    Network::WebServer server(ioContext, projects);

    // Main loop
    bool serverRunning = false;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Control panel window
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Server Control");
        ImGui::TextColored(serverRunning ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1),
                           serverRunning ? "Server Status: Running" : "Server Status: Stopped");
        ImGui::Separator();

        ImGui::BeginDisabled(serverRunning);
        if (ImGui::Button("Start Server", ImVec2(120, 40))) {
            server.start();
            serverRunning = true;
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::BeginDisabled(!serverRunning);
        if (ImGui::Button("Stop Server", ImVec2(120, 40)))
        {
            server.stop();
            serverRunning = false;
        }
        ImGui::EndDisabled();

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Projects", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BeginChild("ProjectsList", ImVec2(0, 150), true);
            for (const auto& [port, path] : server.getProjects()) {
                ImGui::Text("Port: %d, Path: %s", port, path.c_str());
            }
            ImGui::EndChild();
        }
        ImGui::End();

        // Log window
        ImGui::Begin("Logs");
        for (const auto& log : Debug::Log::logs) {
            ImGui::TextUnformatted(log.c_str());
        }
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    if (serverRunning) {
        server.stop();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}