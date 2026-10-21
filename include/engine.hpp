#pragma once 
#include "cfg_storage.hpp"
#include "gui.hpp"
#include "window.hpp"

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
    cfg::Storage<Config> config{.filename = "cfg.toml"sv};

private:
    Engine() : window ("Test window") {}
    ~Engine()=default;
};
