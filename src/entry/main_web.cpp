// nbody web main file

// SDL includes
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <spdlog/spdlog.h>

#include <memory>

#include "cliapp.hpp"

// shared state for SDL app
// i know i know global state is bad
namespace {
std::unique_ptr<CLIApp> cli_app{nullptr};
}  // namespace

auto SDL_AppInit(void ** /*appstate*/, int argc, char **argv) -> SDL_AppResult {
    spdlog::info("Initializing web visualization...");

    // create CLI app
    cli_app = std::make_unique<CLIApp>(argc, argv);

    // continue running
    return SDL_APP_CONTINUE;
}

auto SDL_AppIterate(void * /*appstate*/) -> SDL_AppResult {
    // draw app
    cli_app->viz_app->draw();

    // since we are dont using the built-in glviskit loop
    // we need to manually call Render
    glviskit::Render();

    // continue running
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void * /*appstate*/, SDL_AppResult /*result*/) {
    // cleanup app
}

auto SDL_AppEvent(void * /*appstate*/, SDL_Event *event) -> SDL_AppResult {
    // acquire glviskit manager singleton
    auto &manager = glviskit::Manager::GetInstance();

    // process events
    return manager.ProcessEvent(*event) ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}