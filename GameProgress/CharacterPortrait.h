#ifndef CHARACTER_PORTRAIT_H
#define CHARACTER_PORTRAIT_H

#include <SFML/Graphics.hpp>
#include <string>

class CharacterPortrait {
public:
    bool load(const std::string& filename);
    void setPosition(float x, float y);
    void setScale(float scaleX, float scaleY);
    void draw(sf::RenderWindow& window);
    void setVisible(bool visible);
    bool isVisible() const;
    sf::FloatRect getGlobalBounds() const; 

private:
    sf::Sprite sprite;
    sf::Texture texture;
    bool visible = false;
};

#endif // CHARACTER_PORTRAIT_H