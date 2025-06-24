// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <exception>
#include <memory>

#include <SDL3/SDL.h>
#include <cxxopts.hpp>
#include <imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_sdlrenderer3.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "cocoa/utilities.hpp"
#include "chocboy/config.hpp"

int main(int argc, char** argv)
try {
    std::unique_ptr<cxxopts::Options> parser
        = std::make_unique<cxxopts::Options>(argv[0], "- testing");
    bool version = false;
    constexpr size_t max_width = 90;
    auto& options = *parser;
    options.set_width(max_width).set_tab_expansion().add_options()(
        "v,version", "version info", cxxopts::value<bool>(version)
    );
    auto result = options.parse(argc, argv);

    if (result.count("version") != 0U) {
        fmt::print("{}\n", cocoboy::PROGRAM_VERSION);
    }

    fmt::print("{} {}\n", cocoboy::PROGRAM_NAME, cocoboy::PROGRAM_VERSION);
    fmt::print("{}\n\n", cocoboy::PROGRAM_DESCRIPTION);

    std::shared_ptr<spdlog::logger> logger
        = spdlog::stdout_color_mt(cocoboy::PROGRAM_NAME.data()); // NOLINT
    logger->set_level(spdlog::level::trace);
    logger->info("This is a simple info message");
    logger->trace("This is a simple trace message");
    logger->debug("This is a simple debug message");
    logger->warn("This is a simple warning message");
    logger->error("This is a simple error message");
    logger->critical("This is a simple critical error message");

    constexpr int winWidth = 600;
    constexpr int winHeight = 400;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* window = SDL_CreateWindow("cocoboy", winWidth, winHeight, SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& gio = ImGui::GetIO();
    gio.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        bool some_panel = true;
        ImGui::Begin("Some panel", &some_panel);
        ImGui::Text("Hello world");
        ImGui::End();

        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // NOLINT
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
catch (const spdlog::spdlog_ex& error) {
    fmt::print("{}\n", error.what());
    return 1;
}
catch (const cxxopts::exceptions::exception& error) {
    fmt::print("{}\n", error.what());
    return 1;
}
catch (const std::exception& error) {
    fmt::print("{}\n", error.what());
    return 1;
}
