#include "CharacterPortrait.h"

bool CharacterPortrait::load(const std::string& filename) {
    if (!texture.loadFromFile(filename)) {
        return false;
    }
    sprite.setTexture(texture);
    return true;
}

void CharacterPortrait::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void CharacterPortrait::setScale(float scaleX, float scaleY) {
    sprite.setScale(scaleX, scaleY);
}

void CharacterPortrait::draw(sf::RenderWindow& window) {
    if (visible) {
        window.draw(sprite);
    }
}

void CharacterPortrait::setVisible(bool vis) {
    visible = vis;
}

bool CharacterPortrait::isVisible() const {
    return visible;
}

sf::FloatRect CharacterPortrait::getGlobalBounds() const {
    return sprite.getGlobalBounds(); 
}