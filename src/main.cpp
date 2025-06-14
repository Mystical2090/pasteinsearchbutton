#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/utils/cocos.hpp>
#import <Cocoa/Cocoa.h>

using namespace geode::prelude;

class $modify(wow, LevelSearchLayer) {
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;

        auto searchInput = reinterpret_cast<CCTextInputNode*>(this->getChildByID("search-input"));
        if (!searchInput) return true;

        auto label = CCLabelBMFont::create("Paste", "bigFont.fnt");
        auto pasteButton = CCMenuItemLabel::create(
            label,
            this,
            menu_selector(PasteSearchMod::onPasteClicked)
        );

        pasteButton->setPosition({280.f, 20.f});

        auto menu = CCMenu::create();
        menu->addChild(pasteButton);
        menu->setPosition({0.f, 0.f});
        searchInput->getParent()->addChild(menu);

        return true;
    }

    void onPasteClicked(CCObject*) {
        auto searchInput = reinterpret_cast<CCTextInputNode*>(this->getChildByID("search-input"));
        if (!searchInput) return;

        std::string clipboardText = getClipboardText();
        searchInput->setString(clipboardText.c_str());
    }

    std::string getClipboardText() {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        NSString* content = [pasteboard stringForType:NSPasteboardTypeString];
        if (content) {
            return std::string([content UTF8String]);
        } else {
            return "";
        }
    }
};
