#ifndef LOADSCREEN_HPP
#define LOADSCREEN_HPP

#include <SFML/Graphics.hpp>

class LoadScreen {
public:
    enum class LoadState { LOAD_SUCCESS, BACK };

    LoadScreen(sf::RenderWindow& window, sf::Font& titleFont, sf::Font& bodyFont);
    LoadState run();

private:
    sf::RenderWindow& window;
    sf::Font& titleFont;
    sf::Font& bodyFont;

    sf::RectangleShape background;
    sf::Text title;
    sf::Text loadButton;
    sf::Text backButton;

    sf::Clock animationClock;

    LoadState handleEvents(const sf::Vector2f& mousePos);
    void handleMouseHover(const sf::Vector2f& mousePos);
    void animateTitle(float time);
    void draw();
};

#endif // LOADSCREEN_HPP