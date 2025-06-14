#define CommentType CommentTypeDummy

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

class PasteHandler : public CCObject {
public:
    CCTextInputNode* input;

    static PasteHandler* create(CCTextInputNode* input) {
        auto ret = new PasteHandler();
        ret->input = input;
        ret->autorelease();
        return ret;
    }

    void onPaste(CCObject*) {
        input->setString(getClipboardText().c_str());
    }

    static std::string getClipboardText() {
#if defined(_WIN32)
        if (OpenClipboard(nullptr)) {
            HANDLE data = GetClipboardData(CF_TEXT);
            if (data) {
                char* text = static_cast<char*>(GlobalLock(data));
                if (text) {
                    std::string result(text);
                    GlobalUnlock(data);
                    CloseClipboard();
                    return result;
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
                        CFIndex len = CFDataGetLength(data);
                        std::string result(reinterpret_cast<const char*>(ptr), len);
                        CFRelease(data);
                        CFRelease(clipboard);
                        return result;
                    }
                }
            }
            CFRelease(clipboard);
        }
#endif
        return "";
    }
};

class $modify(LevelSearchLayer) {
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;

        auto searchInput = static_cast<CCTextInputNode*>(this->getChildByID("search-input"));
        if (!searchInput) return true;

        auto label = CCLabelBMFont::create("Paste", "goldFont.fnt");
        auto handler = PasteHandler::create(searchInput);
        auto button = CCMenuItemLabel::create(label, handler, menu_selector(PasteHandler::onPaste));
        button->setPosition({320.f, 202.f});

        auto menu = CCMenu::create();
        menu->addChild(button);
        menu->setPosition(cocos2d::Vec2::ZERO);
        this->addChild(menu, 100);

        return true;
    }
};
