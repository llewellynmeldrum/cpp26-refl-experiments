#pragma once 
#include "cfg_storage.hpp"
#include "gui.hpp"
#include "window.hpp"

#include "test_config.hpp"

struct Engine{
    Engine(const Engine &) = delete;
    Engine(Engine &&) = delete;
    Engine &operator=(const Engine &) = delete;
    Engine &operator=(Engine &&) = delete;

    static auto& get(){
        static Engine instance;
        return instance;
    }

    Window window;
    cfg::Storage<Config> config_storage{.filename = "cfg.toml"sv};

private:
    Engine() : window ("Test window", glm::ivec2{1600,900}) {}
    ~Engine()=default;
};
