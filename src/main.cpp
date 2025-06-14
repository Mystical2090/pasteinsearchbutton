#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <cocos2d.h>
#import <Cocoa/Cocoa.h> // for clipboard access

using namespace geode::prelude;
using namespace cocos2d;

std::string getClipboardText() {
    NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
    NSString* text = [pasteboard stringForType:NSPasteboardTypeString];
    if (text) {
        return std::string([text UTF8String]);
    }
    return "";
}

class $modify(MySearchLayer, LevelSearchLayer) {
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = CCMenu::create();
        menu->setPosition(CCPointZero);

        auto label = CCLabelBMFont::create("Paste", "bigFont.fnt");
        auto button = CCMenuItemLabel::create(label, this, menu_selector(MySearchLayer::onPaste));
        button->setPosition({winSize.width - 50, 30});
        menu->addChild(button);

        this->addChild(menu, 10);
        return true;
    }

    void onPaste(CCObject*) {
        auto searchInput = static_cast<CCTextInputNode*>(this->getChildByID("level-search-input"));
        if (searchInput) {
            auto clip = getClipboardText();
            searchInput->setString(clip.c_str());
        } else {
            log::warn("Search input not found.");
        }
    }
};
