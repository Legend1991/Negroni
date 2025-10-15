#pragma once

#include "../Core/Gui.h"
#include "imgui.h"

#include <format>
#include <functional>

using namespace Core;

namespace ImGui
{
    class ImGuiProfiler : public GuiLayer
    {
    public:
        ImGuiProfiler();
        ~ImGuiProfiler();

        virtual void Draw() override;

    private:
        u32 DrawProfilerEntry(u32 index = 0);
    };
}

ImGui::ImGuiProfiler::ImGuiProfiler()
{
}

ImGui::ImGuiProfiler::~ImGuiProfiler()
{
}

void ImGui::ImGuiProfiler::Draw()
{
#ifndef PROFILER_ENABLED
    return;
#endif

    ImGuiWindowFlags windowFlags = 0;
    //windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
    windowFlags |= ImGuiWindowFlags_NoNavInputs;
    bool open = true;

    ImGui::Begin("Profiler", NULL /*&open*/, windowFlags);
    {
        static float values[100] = {};
        static int values_offset = 0;
        static float maxPlotY = 0.0f;
        static double refresh_time = ImGui::GetTime();
        if (Profiler::size > 1 && ((ImGui::GetTime() - refresh_time) > (1.0f / 60.0f)))
        {
            for (int i = 0; i < Profiler::size; ++i)
            {
                auto& entry = Profiler::entries[i];
                if (entry.indent == 0)
                {
                    values[values_offset] += entry.elapsed / 1000.0f;
                }
            }
            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            refresh_time = ImGui::GetTime();
        }

        static str overlayText = "Total elapsed";
        float max = *std::max_element(values, values + IM_ARRAYSIZE(values));
        maxPlotY = (max > maxPlotY) || (max < maxPlotY / 2) ? (max * 2) : maxPlotY;

        const ImVec2 graphSize(ImGui::GetContentRegionAvail().x, 100.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.00f, 0.85f, 0.20f, 1.00f));
        ImGui::PlotLines("##call_graph", values, IM_ARRAYSIZE(values), values_offset, overlayText.c_str(), 0.0f, maxPlotY, graphSize);
        ImGui::PopStyleColor();

        if (ImGui::BeginTable("Call Graph", 2, ImGuiTableFlags_PadOuterX))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 350.0f);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
            //ImGui::TableHeadersRow();

            DrawProfilerEntry();

            ImGui::EndTable();
        }

        //ImVec2 windowSize = ImGui::GetWindowSize();
        //ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        //ImVec2 newPos = ImVec2(viewportSize.x - windowSize.x - 10.0f, 10.0f);
        //ImGui::SetWindowPos(newPos);
    }
    ImGui::End();
}

u32 ImGui::ImGuiProfiler::DrawProfilerEntry(u32 index)
{
    for (int i = index; i < Profiler::size - 1; ++i)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        auto& entry = Profiler::entries[i];
        str entryName = std::format("{}  ", entry.name.c_str());
        const bool isFolder = entry.indent < Profiler::entries[i + 1].indent;
        const bool isFinite = Profiler::entries[i + 1].indent < entry.indent;

        static ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_DrawLinesFull;
        if (i != 0)
            nodeFlags &= ~ImGuiTreeNodeFlags_LabelSpanAllColumns;

        if (isFolder)
        {
            bool open = ImGui::TreeNodeEx(entryName.c_str(), nodeFlags);
            if ((nodeFlags & ImGuiTreeNodeFlags_LabelSpanAllColumns) == 0)
            {
                ImGui::TableNextColumn();
                ImGui::Text("%.3f ms", entry.elapsed / 1000.0f);
            }

            if (open)
            {
                i = DrawProfilerEntry(i + 1);
                ImGui::TreePop();
            }
            else
            {
                for (int n = i + 1; n < Profiler::size - 1 && Profiler::entries[n].indent > entry.indent; ++n)
                {
                    i++;
                }
            }
        }
        else
        {
            ImGui::TreeNodeEx(entryName.c_str(), nodeFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            ImGui::TableNextColumn();
            ImGui::Text("%.3f ms", entry.elapsed / 1000.0f);

            if (isFinite) return i;
        }

        const u32 lastEntryIndex = Profiler::size - 1;
        const bool isNextEntryLast = i + 1 == lastEntryIndex;
        const bool drawLastEntry = isNextEntryLast && Profiler::entries[i + 1].indent == entry.indent;
        if (drawLastEntry)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            auto& entry = Profiler::entries[i + 1];
            str name = std::format("{}  ", entry.name.c_str());
            ImGui::TreeNodeEx(name.c_str(), nodeFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            ImGui::TableNextColumn();
            ImGui::Text("%.3f ms", entry.elapsed / 1000.0f);

        }

        index = i;
    }

    return index;
}
