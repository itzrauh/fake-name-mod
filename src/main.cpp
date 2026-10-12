#include <Geode/Geode.hpp>
#include <Geode/modify/CommentCell.hpp>
#include <Geode/modify/GJScoreCell.hpp>
#include <Geode/modify/GJUserCell.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/ProfilePage.hpp>

using namespace geode::prelude;

namespace {
    std::string getFakeName() {
        return Mod::get()->getSettingValue<std::string>("fake-name");
    }

    bool isOwnAccount(int accountID) {
        if (accountID <= 0) return false;
        auto account = GJAccountManager::get();
        return account && account->m_accountID > 0 && account->m_accountID == accountID;
    }

    bool getInkCenterX(CCLabelBMFont* label, float& centerX) {
        auto children = label->getChildren();
        if (!children || children->count() == 0) return false;

        float minX = 1e30f, maxX = -1e30f;
        bool found = false;
        for (unsigned int i = 0; i < children->count(); ++i) {
            auto sprite = typeinfo_cast<CCSprite*>(children->objectAtIndex(i));
            if (!sprite) continue;
            float half = sprite->getContentSize().width * 0.5f;
            float cx = sprite->getPositionX();
            if (cx - half < minX) minX = cx - half;
            if (cx + half > maxX) maxX = cx + half;
            found = true;
        }
        if (!found) return false;
        centerX = (minX + maxX) * 0.5f;
        return true;
    }

    void setStringKeepCenter(CCLabelBMFont* label, std::string const& text) {
        if (!label || !label->getParent()) {
            if (label) label->setString(text.c_str());
            return;
        }

        float oldCenterLocal = 0.f;
        bool measured = getInkCenterX(label, oldCenterLocal);
        CCPoint desiredParent;
        if (measured) {
            desiredParent = label->getParent()->convertToNodeSpace(
                label->convertToWorldSpace(ccp(oldCenterLocal, 0.f)));
        }

        label->setString(text.c_str());

        if (!measured) return;
        float newCenterLocal = 0.f;
        if (!getInkCenterX(label, newCenterLocal)) return;

        CCPoint currentParent = label->getParent()->convertToNodeSpace(
            label->convertToWorldSpace(ccp(newCenterLocal, 0.f)));
        label->setPosition(label->getPosition() + (desiredParent - currentParent));
    }

    void replaceName(CCNode* node, std::string const& from, std::string const& to) {
        if (!node || from.empty()) return;
        if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) {
            if (label->getString() && from == label->getString()) {
                label->setString(to.c_str());
            }
        }
        if (auto children = node->getChildren()) {
            for (unsigned int i = 0; i < children->count(); ++i) {
                replaceName(static_cast<CCNode*>(children->objectAtIndex(i)), from, to);
            }
        }
    }

    void replaceOwnName(CCNode* node, std::string const& realName) {
        auto fake = getFakeName();
        replaceName(node, realName, fake);
        if (auto account = GJAccountManager::get()) {
            if (account->m_username != realName) {
                replaceName(node, account->m_username, fake);
            }
        }
    }
}

class $modify(FakeNameMenuLayer, MenuLayer) {
    void updateUserProfileButton() {
        MenuLayer::updateUserProfileButton();
        auto account = GJAccountManager::get();
        if (account && account->m_accountID > 0) {
            setStringKeepCenter(m_profileLabel, getFakeName());
        }
    }
};

class $modify(FakeNameProfilePage, ProfilePage) {
    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);
        if (!m_ownProfile && !(score && isOwnAccount(score->m_accountID))) return;

        auto fake = getFakeName();
        if (m_usernameLabel) {
            m_usernameLabel->setString(fake.c_str());
        }
        if (m_arrayWithUsernameLabel) {
            for (unsigned int i = 0; i < m_arrayWithUsernameLabel->count(); ++i) {
                if (auto label = typeinfo_cast<CCLabelBMFont*>(m_arrayWithUsernameLabel->objectAtIndex(i))) {
                    label->setString(fake.c_str());
                }
            }
        }
        if (score) {
            replaceOwnName(this, score->m_userName);
        }
    }
};

class $modify(FakeNameCommentCell, CommentCell) {
    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);
        if (comment && isOwnAccount(comment->m_accountID)) {
            replaceOwnName(this, comment->m_userName);
        }
    }
};

class $modify(FakeNameGJScoreCell, GJScoreCell) {
    void loadFromScore(GJUserScore* score) {
        GJScoreCell::loadFromScore(score);
        if (score && isOwnAccount(score->m_accountID)) {
            replaceOwnName(this, score->m_userName);
        }
    }
};

class $modify(FakeNameGJUserCell, GJUserCell) {
    void loadFromScore(GJUserScore* score) {
        GJUserCell::loadFromScore(score);
        if (score && isOwnAccount(score->m_accountID)) {
            replaceOwnName(this, score->m_userName);
        }
    }
};
