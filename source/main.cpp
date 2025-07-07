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

    // Load projects from DOMAINS_PATH
    std::vector<std::pair<int, std::string>> projects;
    std::filesystem::path domainsPath(DOMAINS_PATH);
    if (!std::filesystem::exists(domainsPath)) {
        Debug::Log::error(std::format("Domains path does not exist: {}", DOMAINS_PATH), "Main");
        return -1;
    }

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
            } else {
                Debug::Log::warn(std::format("No valid port in .htaccess for {}, using default port {}", projectPath, port), "Main");
            }

            projects.emplace_back(port, projectPath);
            Debug::Log::info(std::format("Loaded project: {} on port {}", entry.path().filename().string(), port), "Main");
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