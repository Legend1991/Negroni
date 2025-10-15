#pragma once

#include "Renderer.h"
#include "Logger.h"
#include "Gui.h"
#include "GameLoop.h"

#include <mutex>
#include <thread>

namespace Core
{
    class RenderThread
    {
    public:
        RenderThread(Renderer& renderer, Gui& gui) : renderer(renderer), gui(gui)
        {
            memset(&gameState, 0, sizeof(GameState));
            thread = std::thread(Start, this);
        }

        ~RenderThread()
        {
            Stop();
        }

        void Draw(const GameState& gameState)
        {
            std::lock_guard<std::mutex> guard(key);
            this->gameState = gameState;
        }

        void Stop()
        {
            std::lock_guard<std::mutex> guard(key);

            quit = true;

            if (thread.joinable())
            {
                thread.join();
                log_info("stopped");
            }
        }

    private:
        static void Start(RenderThread* self)
        {
            log_info("started");

            const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

            while (true)
            {
                if (self->quit)
                {
                    log_info("quit");
                    break;
                }

                self->renderer.Clear((rgba)color);
                {
                    std::lock_guard<std::mutex> guard(self->key);
                    self->renderer.Draw(self->gameState);
                }
                self->gui.Draw();
                self->renderer.Present();
            }
        }

        std::thread thread;
        std::mutex key;

        Renderer& renderer;
        Gui& gui;

        GameState gameState;

        bool quit = false;
    };
}
