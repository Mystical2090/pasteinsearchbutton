#include <Geode/Geode.hpp>
#include <cocos2d.h>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <CoreFoundation/CoreFoundation.h>
    #include <ApplicationServices/ApplicationServices.h>
#endif

using namespace cocos2d;

class PasteButtonMod : public geode::Mod {
public:
    PasteButtonMod() : Mod("com.mystical.pastebutton") {}

    std::string getClipboardText() {
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
            ItemCount itemCount;
            if (PasteboardGetItemCount(clipboard, &itemCount) == noErr && itemCount > 0) {
                PasteboardItemID itemID;
                if (PasteboardGetItemIdentifier(clipboard, 1, &itemID) == noErr) {
                    CFDataRef data;
                    if (PasteboardCopyItemFlavorData(clipboard, itemID, CFSTR("public.utf8-plain-text"), &data) == noErr) {
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

    void onLoad() override {
        geode::hook<&SomeLevelSearchLayer_init>([](auto&& self) -> bool {
            bool result = geode::orig<&SomeLevelSearchLayer_init>(self);

            auto searchInput = static_cast<CCTextInputNode*>(self->getChildByName("searchInput"));

            auto pasteLabel = CCLabelBMFont::create("Paste", "goldFont.fnt");
            auto pasteButton = CCMenuItemLabel::create(pasteLabel, [searchInput, this](CCObject*) {
                std::string clipboardText = this->getClipboardText();
                searchInput->setString(clipboardText.c_str());
            });

            pasteButton->setPosition(Vec2(280, 20));

            auto menu = CCMenu::create(pasteButton, nullptr);
            menu->setPosition(Vec2::ZERO);
            self->addChild(menu, 1000);

            return result;
        });
    }
};

CREATE_GEODE_DLL(PasteButtonMod)
