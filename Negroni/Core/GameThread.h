#pragma once

#include "Clock.h"
#include "Logger.h"
#include "Gui.h"
#include "GameLoop.h"

#include <mutex>
#include <thread>

namespace Core
{
    typedef std::function<void(const GameState&)> UpdateFunc;

    class GameThread
    {
    public:
        GameThread(GameLoop& game, UpdateFunc onUpdate) : game(game), onUpdate(onUpdate)
        {
            thread = std::thread(Start, this);
        }

        ~GameThread()
        {
            Stop();
        }

        void Update(std::vector<KeyStatus>& input)
        {
            std::lock_guard<std::mutex> guard(key);
            this->input = input;
        }

        void Stop()
        {
            //std::lock_guard<std::mutex> guard(key);
            quit = true;

            if (thread.joinable())
            {
                thread.join();
                log_info("stopped");
            }
        }

    private:
        static void Start(GameThread* self)
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

                float dt = DeltaTime();
                std::lock_guard<std::mutex> guard(self->key);
                //auto& gameState = self->game.Update(self->input, dt);
                //self->onUpdate(gameState);
                self->input.clear();
            }
        }

        static float DeltaTime()
        {
            static u64 timeStart = Clock::now();
            static u64 timeCur = Clock::now();
            static float t = (timeCur - timeStart) / 1000.0f;
            static float prev_t = t;

            timeCur = Clock::now();
            t = (timeCur - timeStart) / 1000.0f;
            float dt = t - prev_t;
            prev_t = t;

            return dt;
        }

        GameLoop& game;
        UpdateFunc onUpdate;
        std::thread thread;
        std::mutex key;
        std::vector<KeyStatus> input;
        bool quit = false;
    };
}
