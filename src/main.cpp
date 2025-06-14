#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <cocos2d.h>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#endif

using namespace geode::prelude;
using namespace cocos2d;

class $modify(LevelSearchLayer) {
    bool init() {
        if (!LevelSearchLayer::init()) return false;

        auto searchInput = static_cast<CCTextInputNode*>(this->getChildByID("search-input"));
        if (!searchInput) return true;

        auto label = CCLabelBMFont::create("Paste", "goldFont.fnt");
        auto button = CCMenuItemLabel::create(label, [searchInput](CCObject*) {
            std::string text = getClipboardText();
            if (!text.empty())
                searchInput->setString(text.c_str());
        });

        button->setPosition({320.f, 202.f});
        auto menu = CCMenu::createWithItem(button);
        menu->setPosition(Vec2::ZERO);
        this->addChild(menu, 100);

        return true;
    }

    static std::string getClipboardText() {
#if defined(_WIN32)
        if (OpenClipboard(nullptr)) {
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData) {
                char* pszText = static_cast<char*>(GlobalLock(hData));
                if (pszText) {
                    std::string text(pszText);
                    GlobalUnlock(hData);
                    CloseClipboard();
                    return text;
                }
            }
            CloseClipboard();
        }
#elif defined(__APPLE__)
        PasteboardRef clipboard;
        if (PasteboardCreate(kPasteboardClipboard, &clipboard) == noErr) {
            ItemCount count;
            if (PasteboardGetItemCount(clipboard, &count) == noErr && count > 0) {
                PasteboardItemID item;
                if (PasteboardGetItemIdentifier(clipboard, 1, &item) == noErr) {
                    CFDataRef data;
                    if (PasteboardCopyItemFlavorData(clipboard, item, CFSTR("public.utf8-plain-text"), &data) == noErr) {
                        const UInt8* ptr = CFDataGetBytePtr(data);
                        CFIndex length = CFDataGetLength(data);
                        std::string text(reinterpret_cast<const char*>(ptr), length);
                        CFRelease(data);
                        CFRelease(clipboard);
                        return text;
                    }
                }
            }
            CFRelease(clipboard);
        }
#endif
        return "";
    }
};
