#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <filesystem>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <boost/asio.hpp>

#include "Debug/Log.h"
#include "Network/WebServer.h"

namespace {
    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr int INITIAL_PORT = 8080;
    constexpr const char* WINDOW_TITLE = "Web Server Control Panel";
    constexpr const char* GLSL_VERSION = "#version 130";
}

// GLFW error callback
static void glfwErrorCallback(int error, const char* description) {
    Debug::Log::error(std::format("GLFW Error {}: {}", error, description));
}

// Initialize GLFW and create window
GLFWwindow* initializeGLFW() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    return window;
}

// Initialize ImGui
void initializeImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

// Load projects from DOMAINS_PATH
std::vector<std::pair<int, std::string>> loadProjects() {
    std::vector<std::pair<int, std::string>> projects;
    int port = INITIAL_PORT;
    try {
        if (!std::filesystem::exists(DOMAINS_PATH)) {
            Debug::Log::error(std::format("Domains path does not exist: {}", DOMAINS_PATH));
            return projects;
        }
        for (const auto& entry : std::filesystem::directory_iterator(DOMAINS_PATH)) {
            if (entry.is_directory()) {
                projects.emplace_back(port++, entry.path().string());
                Debug::Log::info(std::format("Loaded project: {} on port {}", entry.path().filename().string(), port-1));
            }
        }
        if (projects.empty()) {
            Debug::Log::warn("No projects found in DOMAINS_PATH", "main");
        }
    } catch (const std::exception& e) {
        Debug::Log::error(std::format("Failed to load projects: {}", e.what()));
    }
    return projects;
}

int main() {
    try {
        // Initialize GLFW and window
        GLFWwindow* window = initializeGLFW();

        // Initialize ImGui
        initializeImGui(window);

        // Initialize server
        boost::asio::io_context ioContext;
        auto projects = loadProjects();
        auto server = std::make_unique<Network::WebServer>(ioContext, projects);

        bool showLogs = true;
        ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Main loop
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Control panel window
            ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
            ImGui::Begin(WINDOW_TITLE);
            ImGui::TextColored(server->isRunning() ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1),
                               server->isRunning() ? "Server Status: Running" : "Server Status: Stopped");
            ImGui::Separator();

            ImGui::BeginDisabled(server->isRunning());
            if (ImGui::Button("Start Server", ImVec2(120, 40))) server->start();
            ImGui::EndDisabled();

            ImGui::SameLine();
            ImGui::BeginDisabled(!server->isRunning());
            if (ImGui::Button("Stop Server", ImVec2(120, 40))) server->stop();
            ImGui::EndDisabled();

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Projects", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::BeginChild("ProjectsList", ImVec2(0, 150), true);
                for (const auto& [port, path] : server->getProjects()) {
                    ImGui::Text("Port: %d, Path: %s", port, path.c_str());
                }
                ImGui::EndChild();
            }
            ImGui::End();

            // Logs window
            ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
            ImGui::Begin("Logs", &showLogs);
            ImGui::BeginChild("LogsList", ImVec2(0, 0), true);
            for (const auto& log : Debug::Log::logs) {
                ImGui::TextWrapped("%s", log.c_str());
            }
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();
            ImGui::End();

            // Render
            ImGui::Render();
            int displayW, displayH;
            glfwGetFramebufferSize(window, &displayW, &displayH);
            glViewport(0, 0, displayW, displayH);
            glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }

        // Cleanup
        server->stop(); // Ensure server is stopped before cleanup
        ImGui::DestroyContext();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
    } catch (const std::exception& e) {
        Debug::Log::error(std::format("Critical error: {}", e.what()));
        return -1;
    }
    return 0;
}