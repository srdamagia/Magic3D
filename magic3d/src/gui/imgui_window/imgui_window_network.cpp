/******************************************************************************
 @Copyright    Copyright (C) 2008 - 2015 by MagicTech.

 @Platform     ANSI compatible
******************************************************************************/
/*
Magic3D Engine
Copyright (c) 2008-2015
Thiago C. Moraes
http://www.magictech.com.br

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <magic3d/magic3d.h>
#include <magic3d/gui/imgui_window/imgui_window_network.h>

Magic3D::GUINetwork::GUINetwork()
{
    clearLog();
    memset(inputBuf, 0, sizeof(inputBuf));
    memset(nick, 0, sizeof(nick));
    memset(ip, 0, sizeof(ip));
    historyPos = -1;
    commands.push_back("HELP");
    commands.push_back("HISTORY");
    commands.push_back("CLEAR");

    port = 0;
    nickError = false;
}

Magic3D::GUINetwork::~GUINetwork()
{
    clearLog();
    for (int i = 0; i < items.Size; i++)
    {
        ImGui::MemFree(history[i]);
    }
}

int Magic3D::GUINetwork::stricmp(const char* str1, const char* str2)
{
    int d;
    while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
    {
        str1++;
        str2++;
    }
    return d;
}

int Magic3D::GUINetwork::strnicmp(const char* str1, const char* str2, int n)
{
    int d = 0;
    while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
    {
        str1++;
        str2++; n--;
    }
    return d;
}

char* Magic3D::GUINetwork::strdup(const char *str)
{
    size_t len = strlen(str) + 1;
    void* buff = ImGui::MemAlloc(len);
    return (char*)memcpy(buff, (const void*)str, len);
}

void Magic3D::GUINetwork::clearLog()
{
    for (int i = 0; i < items.Size; i++)
    {
        ImGui::MemFree(items[i]);
    }
    items.clear();
    scrollToBottom = true;
}

void Magic3D::GUINetwork::addLog(const char* fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf)-1] = 0;
    va_end(args);
    items.push_back(strdup(buf));
    scrollToBottom = true;
}

void Magic3D::GUINetwork::draw(const char* title, bool* opened)
{
    ImGui::ShowTestWindow();
    ImGui::SetNextWindowPos(ImVec2(5.0f, Renderer::getInstance()->getWindow()->getHeight() - 325.0f), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(480.0f, 320.0f), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin(title, opened))
    {
        ImGui::End();
        return;
    }

    /*ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
    static ImGuiTextFilter filter;
    filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::PopStyleVar();
    ImGui::Separator();*/

    ImGui::Text("Address:");
    ImGui::SameLine(75.0f);
    ImGui::PushItemWidth(150.0f);
    ImGui::InputText("##ip", ip, IM_ARRAYSIZE(ip));
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Text(":"); ImGui::SameLine();
    ImGui::PushItemWidth(100.0f);
    ImGui::DragInt("##port", &port, 1.0f, 0, 999999);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Connect", ImVec2(100.0f, ImGui::GetItemsLineHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y)))
    {
        Network::getInstance()->connect(ip, port);
    }


    ImGui::Text("Nick:");
    ImGui::SameLine(75.0f);
    ImGui::PushItemWidth(272.0f);
    ImGui::InputText("##nick:", nick, IM_ARRAYSIZE(nick));
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Disconnect", ImVec2(100.0f, ImGui::GetItemsLineHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y)))
    {
        Network::getInstance()->disconnect(false);
    }
    ImGui::Separator();



    // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
    // NB- if you have thousands of entries this approach may be too inefficient. You can seek and display only the lines that are visible - CalcListClipping() is a helper to compute this information.
    // If your items are of variable size you may want to implement code similar to what CalcListClipping() does. Or split your data into fixed height items to allow random-seeking into your list.
    ImGui::BeginChild("ScrollingRegion", ImVec2(0,-ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("Clear")) clearLog();
        ImGui::EndPopup();
    }
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
    for (int i = 0; i < items.Size; i++)
    {
        const char* item = items[i];
        /*if (!filter.PassFilter(item))
        {
            continue;
        }*/
        ImVec4 col = ImColor(255,255,255); // A better implementation may store a type per-item. For the sample let's just parse the text.
        if (strstr(item, "[error]")) col = ImColor(255,100,100);
        else if (strncmp(item, "# ", 2) == 0) col = ImColor(255,200,150);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted(item);
        ImGui::PopStyleColor();
    }
    if (scrollToBottom)
    {
        ImGui::SetScrollHere();
    }
    scrollToBottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    ImGui::Text("Send:"); ImGui::SameLine();
    ImGui::PushItemWidth(-10.0f);
    if (ImGui::InputText("##send", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &textEditCallbackStub, (void*)this))
    {
        char* input_end = inputBuf + strlen(inputBuf);
        while (input_end > inputBuf && input_end[-1] == ' ') input_end--; *input_end = 0;
        if (inputBuf[0])
        {
            execCommand(inputBuf);
        }
        strcpy(inputBuf, "");
    }
    ImGui::PopItemWidth();

    // Demonstrate keeping auto focus on the input box
    if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
    {
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    ImGui::End();

    if (nickError)
    {
        ImGui::OpenPopup("Nick Error");
    }
    if (ImGui::BeginPopupModal("Nick Error"))
    {
        ImGui::Text("You need a nick to send a message!");
        if (ImGui::Button("Close"))
        {
            nickError = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Magic3D::GUINetwork::execCommand(const char* command_line)
{
    // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
    historyPos = -1;
    for (int i = history.Size-1; i >= 0; i--)
    {
        if (stricmp(history[i], command_line) == 0)
        {
            ImGui::MemFree(history[i]);
            history.erase(history.begin() + i);
            break;
        }
    }
    history.push_back(strdup(command_line));

    if (command_line[0] == '/')
    {
        // Process command
        if (stricmp(&command_line[1], "CLEAR") == 0)
        {
            clearLog();
        }
        else if (stricmp(&command_line[1], "HELP") == 0)
        {
            addLog("Commands:");
            for (int i = 0; i < commands.Size; i++)
            {
                addLog("- %s", commands[i]);
            }
        }
        else if (stricmp(&command_line[1], "HISTORY") == 0)
        {
            for (int i = history.Size >= 10 ? history.Size - 10 : 0; i < history.Size; i++)
            {
                addLog("%3d: %s\n", i, history[i]);
            }
        }
    }
    else
    {
        if (stricmp(nick, "") == 0)
        {
            nickError = true;
        }
        else
        {
            Network::getInstance()->sendText(nick, command_line);
        }
    }
}

int Magic3D::textEditCallbackStub(ImGuiTextEditCallbackData* data)
{
    GUINetwork* console = (GUINetwork*)data->UserData;
    return console->textEditCallback(data);
}

int Magic3D::GUINetwork::textEditCallback(ImGuiTextEditCallbackData* data)
{
    //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch (data->EventFlag)
    {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                {
                    break;
                }
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < commands.Size; i++)
            {
                if (strnicmp(commands[i], word_start, (int)(word_end-word_start)) == 0)
                {
                    candidates.push_back(commands[i]);
                }
            }

            if (candidates.Size == 0)
            {
                // No match
                addLog("No match for \"%.*s\"!\n", (int)(word_end-word_start), word_start);
            }
            else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
                data->DeleteChars((int)(word_start-data->Buf), (int)(word_end-word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else
            {
                // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                int match_len = (int)(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                    {
                        if (i == 0)
                        {
                            c = toupper(candidates[i][match_len]);
                        }
                        else if (c != toupper(candidates[i][match_len]))
                        {
                            all_candidates_matches = false;
                        }
                    }
                    if (!all_candidates_matches)
                    {
                        break;
                    }
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end-word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                addLog("Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                {
                    addLog("- %s\n", candidates[i]);
                }
            }

            break;
        }

        case ImGuiInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = historyPos;
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (historyPos == -1)
                {
                    historyPos = history.Size - 1;
                }
                else if (historyPos > 0)
                {
                    historyPos--;
                }
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (historyPos != -1)
                {
                    if (++historyPos >= history.Size)
                    {
                        historyPos = -1;
                    }
                }
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != historyPos)
            {
                snprintf(data->Buf, data->BufSize, "%s", (historyPos >= 0) ? history[historyPos] : "");
                data->BufDirty = true;
                data->CursorPos = data->SelectionStart = data->SelectionEnd = (int)strlen(data->Buf);
            }
        }
    }
    return 0;
}
