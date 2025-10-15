#pragma once

#include "Queue.h"

#include <thread>

namespace Core
{
    template<typename MessageType>
    class Thread
    {
    private:
        struct QueueItem
        {
            bool quit;
            MessageType message;
        };

    public:
        Thread()
        {
            thread = std::thread(start, std::ref(messages));
        }

        ~Thread()
        {
            messages.send({ true });
            if (thread.joinable())
            {
                thread.join();
            }
        }

        void send(MessageType& message)
        {
            messages.send({ false, message });
        }

        virtual void execute(MessageType& message) = 0;

    private:
        void start(Queue<QueueItem>& in)
        {
            while (true)
            {
                QueueItem qItem;
                bool received = in.tryReceive(std::ref(qItem));

                if (!received) continue;

                if (qItem.quit)
                    break;

                execute(std::ref(qItem.message));
            }
        }

        std::thread thread;
        Queue<QueueItem> messages;
    };
}
