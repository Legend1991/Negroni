#pragma once

#include "Core/Base.h"
#include "Core/Logger.h"
#include "Core/Keyboard.h"
#include "Core/Mouse.h"

using namespace Core;

namespace Negroni
{
    class EditorScript : public Script
    {
    public:
        EditorScript(const std::vector<ObjectRef>& objects) : objects(objects)
        {
            log_info("editor script consturctor");
        }

        void FixedUpdate() override
        {
        }

        void Update(float dt) override
        {
            if (Mouse::IsDown("PickObject"))
            {
                //log_info("Mouse left button click: {} x {}", Mouse::X(), Mouse::Y());

                ID objectID = objects[2]->id; //objectPicker->Pick(Mouse::X(), Mouse::Y());

                ObjectRef oldSelectedObj = *std::find_if(objects.begin(), objects.end(),
                    [](const ObjectRef& o) { return o->selected == true; }
                );

                if (oldSelectedObj)
                {
                    oldSelectedObj->selected = oldSelectedObj->id == objectID;
                }

                if (objectID == ID::None) return;

                ObjectRef newSelectedObj = *std::find_if(objects.begin(), objects.end(),
                    [&objectID](const ObjectRef& o) { return o->id == objectID; }
                );

                if (!newSelectedObj) return;

                newSelectedObj->selected = true;

                log_info("Select Object(ID={})", (u32)newSelectedObj->id);
            }
        }

        virtual str Name() override { return "EditorScript"; }

    private:
        const std::vector<ObjectRef>& objects;
    };
}
