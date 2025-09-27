#include <format>
#include "../../../../Events/Input/KeyEvent.hpp"
#include "../../../../../Utils/Logger/Logger.hpp"
#include "../../../../Client.hpp"
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

                for (auto &troll: FlarialGUI::TextBoxes) {

                    auto& box = troll.second;
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
                        if (event.getKey() != VK_BACK && event.getKey() != VK_DELETE && event.getPressedKeysAsString() != "CTRL+V" && event.getKey() != VK_LEFT && event.getKey() != VK_RIGHT && event.getKey() != VK_HOME && event.getKey() != VK_END)
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
                                int insertPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                insertPos = std::max(0, std::min(insertPos, (int)box.text.length()));
                                box.text.insert(insertPos, event.getKeyAsString(isCapital));
                            }
                        }

                        else if (event.getKey() == VK_LEFT)
                        {
                            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                int currentPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                if (currentPos > 0) {
                                    currentPos--;

                                    while (currentPos > 0 && std::isspace(box.text[currentPos])) {
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

                            std::thread leftThread([&box]() {
                                bool firstTime = true;
                                while (box.isMovingLeft) {
                                    if (!firstTime && FlarialGUI::TextCursorPosition < (int)box.text.length()) {
                                        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                            int currentPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                            if (currentPos > 0) {
                                                currentPos--;

                                                while (currentPos > 0 && std::isspace(box.text[currentPos])) {
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

                                    if (firstTime) {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(400));
                                        firstTime = false;
                                    } else {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                    }
                                }
                            });

                            leftThread.detach();
                            box.isMovingLeft = true;
                        }

                        else if (event.getKey() == VK_RIGHT)
                        {
                            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                int currentPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                if (currentPos < (int)box.text.length()) {
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

                            std::thread rightThread([&box]() {
                                bool firstTime = true;
                                while (box.isMovingRight) {
                                    if (!firstTime) {
                                        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                            int currentPos = box.text.length() - FlarialGUI::TextCursorPosition;
                                            if (currentPos < (int)box.text.length()) {
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
                                    }

                                    if (firstTime) {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(400));
                                        firstTime = false;
                                    } else {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                    }
                                }
                            });

                            rightThread.detach();
                            box.isMovingRight = true;
                        } else if (event.getKey() == VK_HOME) {
                            FlarialGUI::TextCursorPosition = (int)box.text.length();
                        } else if (event.getKey() == VK_END) {
                            FlarialGUI::TextCursorPosition = 0;
                        } else if (event.getKey() == VK_DELETE) {
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
                                        if (charsToDelete > 0) {
                                            box.text.erase(deletePos, charsToDelete);
                                            FlarialGUI::TextCursorPosition -= charsToDelete;
                                        }
                                    }
                                } else {
                                    if (deletePos < (int)box.text.length()) {
                                        box.text.erase(deletePos, 1);
                                        FlarialGUI::TextCursorPosition -= 1;
                                    }
                                }
                            }

                            std::thread deleteThread([&box]() {
                                bool firstTime = true;
                                while (box.isForwardDeleting) {
                                    if (!box.text.empty() && !firstTime) {
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
                                                if (charsToDelete > 0) {
                                                    box.text.erase(deletePos, charsToDelete);
                                                    FlarialGUI::TextCursorPosition -= charsToDelete;
                                                }
                                            }
                                        } else {
                                            if (deletePos < (int)box.text.length()) {
                                                box.text.erase(deletePos, 1);
                                                FlarialGUI::TextCursorPosition -= 1;
                                            }
                                        }
                                    }

                                    if (firstTime) {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(400));
                                        firstTime = false;
                                    } else {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                    }
                                }
                            });

                            deleteThread.detach();
                            box.isForwardDeleting = true;
                        } else if (event.getKey() == VK_BACK) {

                            if (event.getAction() == ActionType::Pressed) {

                                std::thread t([&box]() {

                                    bool firstTime = true;
                                    while (box.isDeleting) {

                                        if (!box.text.empty() && !firstTime) {
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
                                                    if (charsToDelete > 0) {
                                                        box.text.erase(wordStart, charsToDelete);
                                                    }
                                                }
                                            } else {
                                                if (deletePos > 0) {
                                                    box.text.erase(deletePos - 1, 1);
                                                }
                                            }
                                        }

                                        if (firstTime) {
                                            std::this_thread::sleep_for(std::chrono::milliseconds(400));
                                            firstTime = false;
                                        }
                                        else {
                                            std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                        }
                                    }
                                });

                                t.detach();

                                box.isDeleting = true;

                            }

                            if (!box.text.empty()) {
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
                                        if (charsToDelete > 0) {
                                            box.text.erase(wordStart, charsToDelete);
                                        }
                                    }
                                } else {
                                    if (deletePos > 0) {
                                        box.text.erase(deletePos - 1, 1);
                                    }
                                }
                            }
                        }
                    }

                    if (box.isActive && event.getPressedKeysAsString() == "CTRL+V") {

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
