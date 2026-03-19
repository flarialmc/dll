#include <format>
#include "../../../../Events/Input/KeyEvent.hpp"
#include "../../../../../Utils/Logger/Logger.hpp"
#include "../../../../Client.hpp"
#include "../../ClickGUI/ClickGUI.hpp"
#include <windows.h>
#include <unknwn.h>
#include <cctype>
#include <algorithm>
#include <thread>
#include <chrono>

// TODO: LIKELY REQ A FIX
class GUIKeyListener : public Listener {
private:
    bool isCapital = false;
public:
    void onKey(KeyEvent &event)  {
        if (!Client::disable) {

            if (ModuleManager::getModule("ClickGUI")->active) {

                if (event.getKey() == 20 && event.getAction() == ActionType::Pressed ||
                        event.getKey() == VK_SHIFT)
                    isCapital = !isCapital;

                for (auto &shi: FlarialGUI::KeybindSelectors) {
                    shi.second.currentOnKeyTime = std::chrono::steady_clock::now();
                }

                if (event.getPressedKeysAsString() != "no")
                    FlarialGUI::currentKeybind = event.getPressedKeysAsString();
                else FlarialGUI::currentKeybind = "nothing";

                for (auto &textBoxEntry: FlarialGUI::TextBoxes) {

                    auto& box = textBoxEntry.second;
                    if (event.getKey() == VK_ESCAPE || ModuleManager::getModule("ClickGUI")->isKeybind(event.keys) &&
                                                               ModuleManager::getModule("ClickGUI")->isKeyPartOfKeybind(
                                                                       event.key))
                        box.isActive = false;

                    if (event.getKey() == VK_BACK)
                        if (event.getAction() == ActionType::Released) {
                            box.isDeleting = false;
                        }

                    if (event.getKey() == VK_LEFT)
                        if (event.getAction() == ActionType::Released) {
                            box.isMovingLeft = false;
                        }

                    if (event.getKey() == VK_RIGHT)
                        if (event.getAction() == ActionType::Released) {
                            box.isMovingRight = false;
                        }

                    if (event.getKey() == VK_DELETE)
                        if (event.getAction() == ActionType::Released) {
                            box.isForwardDeleting = false;
                        }

                    if (box.isActive && event.getAction() == ActionType::Pressed)
                    {
                        if (event.getKey() != VK_BACK && event.getKey() != VK_DELETE && event.getPressedKeysAsString() != "CTRL+V" && event.getKey() != VK_LEFT && event.getKey() != VK_RIGHT && event.getKey() != VK_UP && event.getKey() != VK_DOWN && event.getKey() != VK_HOME && event.getKey() != VK_END)
                        {
                            int key = event.getKey();
                            bool isAllowedChar = false;

                            if ((key >= 48 && key <= 57) || // 0-9
                                (key >= 65 && key <= 90))   // A-Z (handles both cases as getKeyAsString converts)
                            {
                                isAllowedChar = true;
                            }
                            else {
                                switch (key) {
                                case 188: // , or <
                                case 190: // . or >
                                case 192: // ` or ~
                                case 219: // [ or {
                                case 221: // ] or }
                                case 220: // \ or |
                                case 222: // ' or "
                                case 191: // / or ?
                                case 187: // = or +
                                case 189: // - or _
                                case 186: // ; or :
                                    isAllowedChar = true;
                                    break;
                                case 32: // Spacebar
                                    isAllowedChar = true;
                                    break;
                                    // Add other specific allowed symbols here
                                default:
                                    break;
                                }
                            }

                            if (isAllowedChar) {
                                // Clamp cursor position first
                                if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                    FlarialGUI::TextCursorPosition = (int)box.text.length();
                                }
                                if (FlarialGUI::TextCursorPosition < 0) {
                                    FlarialGUI::TextCursorPosition = 0;
                                }

                                int insertPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                insertPos = std::max(0, std::min(insertPos, (int)box.text.length()));
                                box.text.insert(insertPos, event.getKeyAsString(isCapital));
                            }
                        }

                        else if (event.getKey() == VK_LEFT)
                        {
                            // Clamp cursor position first
                            if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                FlarialGUI::TextCursorPosition = (int)box.text.length();
                            }
                            if (FlarialGUI::TextCursorPosition < 0) {
                                FlarialGUI::TextCursorPosition = 0;
                            }

                            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                int currentPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                if (currentPos > 0) {
                                    currentPos--;

                                    while (currentPos > 0 && currentPos < (int)box.text.length() && std::isspace(box.text[currentPos])) {
                                        currentPos--;
                                    }

                                    while (currentPos > 0 && !std::isspace(box.text[currentPos - 1])) {
                                        currentPos--;
                                    }

                                    FlarialGUI::TextCursorPosition = box.text.length() - currentPos;
                                }
                            } else {
                                if (FlarialGUI::TextCursorPosition < (int)box.text.length()) {
                                    FlarialGUI::TextCursorPosition += 1;
                                }
                            }

                            // Only start repeat thread if not already moving
                            if (!box.isMovingLeft) {
                                box.isMovingLeft = true; // Set BEFORE starting thread

                                std::thread leftThread([&box]() {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(400));

                                    while (box.isMovingLeft) {
                                        // Clamp cursor position
                                        if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                            FlarialGUI::TextCursorPosition = (int)box.text.length();
                                        }
                                        if (FlarialGUI::TextCursorPosition < 0) {
                                            FlarialGUI::TextCursorPosition = 0;
                                        }

                                        if (FlarialGUI::TextCursorPosition < (int)box.text.length()) {
                                            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                                int currentPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                                if (currentPos > 0) {
                                                    currentPos--;

                                                    while (currentPos > 0 && currentPos < (int)box.text.length() && std::isspace(box.text[currentPos])) {
                                                        currentPos--;
                                                    }

                                                    while (currentPos > 0 && !std::isspace(box.text[currentPos - 1])) {
                                                        currentPos--;
                                                    }

                                                    FlarialGUI::TextCursorPosition = box.text.length() - currentPos;
                                                }
                                            } else {
                                                FlarialGUI::TextCursorPosition += 1;
                                            }
                                        }

                                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                    }
                                });

                                leftThread.detach();
                            }
                        }

                        else if (event.getKey() == VK_RIGHT)
                        {
                            // Clamp cursor position first
                            if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                FlarialGUI::TextCursorPosition = (int)box.text.length();
                            }
                            if (FlarialGUI::TextCursorPosition < 0) {
                                FlarialGUI::TextCursorPosition = 0;
                            }

                            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                int currentPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                if (currentPos >= 0 && currentPos < (int)box.text.length()) {
                                    while (currentPos < (int)box.text.length() && !std::isspace(box.text[currentPos])) {
                                        currentPos++;
                                    }

                                    while (currentPos < (int)box.text.length() && std::isspace(box.text[currentPos])) {
                                        currentPos++;
                                    }

                                    FlarialGUI::TextCursorPosition = box.text.length() - currentPos;
                                }
                            } else {
                                if (FlarialGUI::TextCursorPosition > 0) {
                                    FlarialGUI::TextCursorPosition -= 1;
                                }
                            }

                            // Clamp after movement
                            if (FlarialGUI::TextCursorPosition < 0) {
                                FlarialGUI::TextCursorPosition = 0;
                            }

                            // Only start repeat thread if not already moving
                            if (!box.isMovingRight) {
                                box.isMovingRight = true; // Set BEFORE starting thread

                                std::thread rightThread([&box]() {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(400));

                                    while (box.isMovingRight) {
                                        // Clamp cursor position
                                        if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                            FlarialGUI::TextCursorPosition = (int)box.text.length();
                                        }
                                        if (FlarialGUI::TextCursorPosition < 0) {
                                            FlarialGUI::TextCursorPosition = 0;
                                        }

                                        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                            int currentPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                            if (currentPos >= 0 && currentPos < (int)box.text.length()) {
                                                while (currentPos < (int)box.text.length() && !std::isspace(box.text[currentPos])) {
                                                    currentPos++;
                                                }

                                                while (currentPos < (int)box.text.length() && std::isspace(box.text[currentPos])) {
                                                    currentPos++;
                                                }

                                                FlarialGUI::TextCursorPosition = box.text.length() - currentPos;
                                            }
                                        } else {
                                            if (FlarialGUI::TextCursorPosition > 0) {
                                                FlarialGUI::TextCursorPosition -= 1;
                                            }
                                        }

                                        // Clamp after movement
                                        if (FlarialGUI::TextCursorPosition < 0) {
                                            FlarialGUI::TextCursorPosition = 0;
                                        }

                                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                    }
                                });

                                rightThread.detach();
                            }
                        } else if (event.getKey() == VK_HOME || event.getKey() == VK_UP) {
                            // Home and Up arrow move cursor to beginning of text
                            FlarialGUI::TextCursorPosition = (int)box.text.length();
                        } else if (event.getKey() == VK_END || event.getKey() == VK_DOWN) {
                            // End and Down arrow move cursor to end of text
                            FlarialGUI::TextCursorPosition = 0;
                        } else if (event.getKey() == VK_DELETE) {
                            // Clamp cursor position first
                            if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                FlarialGUI::TextCursorPosition = (int)box.text.length();
                            }
                            if (FlarialGUI::TextCursorPosition < 0) {
                                FlarialGUI::TextCursorPosition = 0;
                            }

                            if (!box.text.empty()) {
                                int deletePos = box.text.length() - FlarialGUI::TextCursorPosition;
                                deletePos = std::max(0, std::min(deletePos, (int)box.text.length()));

                                if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                    int wordEnd = deletePos;
                                    if (wordEnd < (int)box.text.length()) {
                                        while (wordEnd < (int)box.text.length() && std::isspace(box.text[wordEnd])) {
                                            wordEnd++;
                                        }
                                        while (wordEnd < (int)box.text.length() && !std::isspace(box.text[wordEnd])) {
                                            wordEnd++;
                                        }
                                        int charsToDelete = wordEnd - deletePos;
                                        if (charsToDelete > 0 && deletePos >= 0 && deletePos + charsToDelete <= (int)box.text.length()) {
                                            box.text.erase(deletePos, charsToDelete);
                                            // Forward delete doesn't change cursor position relative to start
                                        }
                                    }
                                } else {
                                    if (deletePos >= 0 && deletePos < (int)box.text.length()) {
                                        box.text.erase(deletePos, 1);
                                        // Forward delete doesn't change cursor position relative to start
                                    }
                                }

                                // Clamp cursor position after deletion
                                if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                    FlarialGUI::TextCursorPosition = (int)box.text.length();
                                }
                            }

                            // Only start repeat-delete thread if not already deleting
                            if (!box.isForwardDeleting) {
                                box.isForwardDeleting = true; // Set BEFORE starting thread

                                std::thread deleteThread([&box]() {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(400));

                                    while (box.isForwardDeleting) {
                                        // Clamp cursor position
                                        if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                            FlarialGUI::TextCursorPosition = (int)box.text.length();
                                        }
                                        if (FlarialGUI::TextCursorPosition < 0) {
                                            FlarialGUI::TextCursorPosition = 0;
                                        }

                                        if (!box.text.empty()) {
                                            int deletePos = box.text.length() - FlarialGUI::TextCursorPosition;
                                            deletePos = std::max(0, std::min(deletePos, (int)box.text.length()));

                                            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                                int wordEnd = deletePos;
                                                if (wordEnd < (int)box.text.length()) {
                                                    while (wordEnd < (int)box.text.length() && std::isspace(box.text[wordEnd])) {
                                                        wordEnd++;
                                                    }
                                                    while (wordEnd < (int)box.text.length() && !std::isspace(box.text[wordEnd])) {
                                                        wordEnd++;
                                                    }
                                                    int charsToDelete = wordEnd - deletePos;
                                                    if (charsToDelete > 0 && deletePos >= 0 && deletePos + charsToDelete <= (int)box.text.length()) {
                                                        box.text.erase(deletePos, charsToDelete);
                                                    }
                                                }
                                            } else {
                                                if (deletePos >= 0 && deletePos < (int)box.text.length()) {
                                                    box.text.erase(deletePos, 1);
                                                }
                                            }

                                            // Clamp cursor position after deletion
                                            if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                                FlarialGUI::TextCursorPosition = (int)box.text.length();
                                            }
                                        }

                                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                    }
                                });

                                deleteThread.detach();
                            }
                        } else if (event.getKey() == VK_BACK) {

                            if (event.getAction() == ActionType::Pressed) {
                                // First do the immediate deletion
                                if (!box.text.empty()) {
                                    // Clamp cursor position first to prevent issues
                                    if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                        FlarialGUI::TextCursorPosition = (int)box.text.length();
                                    }
                                    if (FlarialGUI::TextCursorPosition < 0) {
                                        FlarialGUI::TextCursorPosition = 0;
                                    }

                                    int deletePos = box.text.length() - FlarialGUI::TextCursorPosition;
                                    deletePos = std::max(0, std::min(deletePos, (int)box.text.length()));

                                    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                        int wordStart = deletePos - 1;
                                        if (wordStart >= 0) {
                                            while (wordStart > 0 && std::isspace(box.text[wordStart])) {
                                                wordStart--;
                                            }
                                            while (wordStart > 0 && !std::isspace(box.text[wordStart - 1])) {
                                                wordStart--;
                                            }
                                            int charsToDelete = deletePos - wordStart;
                                            if (charsToDelete > 0 && wordStart >= 0 && wordStart + charsToDelete <= (int)box.text.length()) {
                                                box.text.erase(wordStart, charsToDelete);
                                            }
                                        }
                                    } else {
                                        if (deletePos > 0 && deletePos <= (int)box.text.length()) {
                                            box.text.erase(deletePos - 1, 1);
                                        }
                                    }

                                    // Clamp cursor position after deletion
                                    if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                        FlarialGUI::TextCursorPosition = (int)box.text.length();
                                    }
                                }

                                // Only start repeat-delete thread if not already deleting
                                if (!box.isDeleting) {
                                    box.isDeleting = true; // Set BEFORE starting thread

                                    std::thread t([&box]() {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(400));

                                        while (box.isDeleting) {
                                            if (!box.text.empty()) {
                                                // Clamp cursor position
                                                if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                                    FlarialGUI::TextCursorPosition = (int)box.text.length();
                                                }
                                                if (FlarialGUI::TextCursorPosition < 0) {
                                                    FlarialGUI::TextCursorPosition = 0;
                                                }

                                                int deletePos = box.text.length() - FlarialGUI::TextCursorPosition;
                                                deletePos = std::max(0, std::min(deletePos, (int)box.text.length()));

                                                if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                                    int wordStart = deletePos - 1;
                                                    if (wordStart >= 0) {
                                                        while (wordStart > 0 && std::isspace(box.text[wordStart])) {
                                                            wordStart--;
                                                        }
                                                        while (wordStart > 0 && !std::isspace(box.text[wordStart - 1])) {
                                                            wordStart--;
                                                        }
                                                        int charsToDelete = deletePos - wordStart;
                                                        if (charsToDelete > 0 && wordStart >= 0 && wordStart + charsToDelete <= (int)box.text.length()) {
                                                            box.text.erase(wordStart, charsToDelete);
                                                        }
                                                    }
                                                } else {
                                                    if (deletePos > 0 && deletePos <= (int)box.text.length()) {
                                                        box.text.erase(deletePos - 1, 1);
                                                    }
                                                }

                                                // Clamp cursor position after deletion
                                                if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                                                    FlarialGUI::TextCursorPosition = (int)box.text.length();
                                                }
                                            }

                                            std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                        }
                                    });

                                    t.detach();
                                }
                            }
                        }
                    }

                    if (box.isActive && event.getPressedKeysAsString() == "CTRL+V") {
                        // Clamp cursor position first
                        if (FlarialGUI::TextCursorPosition > (int)box.text.length()) {
                            FlarialGUI::TextCursorPosition = (int)box.text.length();
                        }
                        if (FlarialGUI::TextCursorPosition < 0) {
                            FlarialGUI::TextCursorPosition = 0;
                        }

                        if (OpenClipboard(nullptr)) {
                            HANDLE hData = GetClipboardData(CF_TEXT);
                            if (hData != nullptr) {
                                char *clipboardText = static_cast<char *>(GlobalLock(hData));
                                if (clipboardText != nullptr) {
                                    int insertPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                    insertPos = std::max(0, std::min(insertPos, (int)box.text.length()));
                                    box.text.insert(insertPos, clipboardText);
                                    GlobalUnlock(hData);
                                }
                            }
                            CloseClipboard();
                        }
                    }
                }

                // Page Up/Page Down for scrolling ClickGUI (only when no text box is active)
                bool anyTextBoxActive = false;
                for (const auto& box : FlarialGUI::TextBoxes) {
                    if (box.second.isActive) {
                        anyTextBoxActive = true;
                        break;
                    }
                }

                if (!anyTextBoxActive && event.getAction() == ActionType::Pressed) {
                    float pageScrollMult = Client::settings.getSettingByName<float>("pageScrollMultiplier")->value;
                    if (event.getKey() == VK_PRIOR) { // Page Up
                        ClickGUI::accumilatedPos += FlarialGUI::scrollposmodifier * pageScrollMult;
                        ClickGUI::accumilatedBarPos += FlarialGUI::barscrollposmodifier * pageScrollMult;
                    } else if (event.getKey() == VK_NEXT) { // Page Down
                        ClickGUI::accumilatedPos -= FlarialGUI::scrollposmodifier * pageScrollMult;
                        ClickGUI::accumilatedBarPos -= FlarialGUI::barscrollposmodifier * pageScrollMult;
                    }
                }

            } else {
                // fix perfomace ? Worked even without this :/
                for (auto &box: FlarialGUI::TextBoxes) {
                    box.second.isActive = false;
                }
            }
        }
    };

    GUIKeyListener() {
        Listen(this, KeyEvent, &GUIKeyListener::onKey);
    }
    ~GUIKeyListener() {
        Deafen(this, KeyEvent, &GUIKeyListener::onKey);
    }
};
