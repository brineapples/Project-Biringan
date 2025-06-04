#ifndef BACKGROUND_MANAGER_H
#define BACKGROUND_MANAGER_H

#include <SFML/Graphics.hpp>
#include <string>

class BackgroundManager {
public:
    BackgroundManager(sf::RenderWindow& window);
    void setBackground(const std::string& imagePath);
    void update(float deltaTime);
    void draw();
    void startFadeOut(float duration);
    void resetFade();
    bool isFading() const;
    bool isHidden() const;

private:
    sf::RenderWindow& window;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape fadeRect;

    bool hasBackground = false;
    bool isFadingFlag = false;
    bool isHiddenFlag = false;

    float fadeOpacity = 255.0f;
    float fadeDuration = 1.0f;
    float fadeElapsed = 0.0f;
};

#endif // BACKGROUNDMANAGER_HPP