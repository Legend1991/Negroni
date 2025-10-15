#pragma once

#include "Base.h"
#include "Input.h"
#include "Keyboard.h"
#include "Profiler.h"

// Usefull source: https://docs.unity3d.com/6000.2/Documentation/Manual/execution-order.html

namespace Core
{
    struct GameState
    {
        std::vector<ObjectRef> objects;
    };

	class GameLoop
	{
	public:
        GameState& Update(InputEvent& input, float dt)
        {
            // NOTE: When the FPS is too high, delta time can be 0 for many frames in a row.  
            // If there is no user input, the game state remains exactly the same.  
            // Therefore, we can safely skip unnecessary calculations.  
            // PS: Even though dt is a float, comparing dt == 0 is still valid here. 
            if (input.isDirty == false && dt == 0) return state;

            static const float fixedDeltaTime = 1.0f / 60.0f;
            static float cumulativeDeltaTime = 0.0f;

            cumulativeDeltaTime += dt;
            while (cumulativeDeltaTime >= fixedDeltaTime)
            {
                for (auto& object : state.objects)
                {
                    object->script->FixedUpdate();
                }
                //physics.Update(fixedDeltaTime);
                cumulativeDeltaTime -= fixedDeltaTime;
            }

            Input::Update(input);

            {
                //ProfileBlock _("All objects script update");

                //#pragma omp parallel for num_threads(numThreads)
                for (auto& object : state.objects)
                {
                    object->script->Update(dt);
                }
            }

            return state;
        }

        GameState state;
	};
}