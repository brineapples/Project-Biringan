#ifndef PLAYINTROSCREEN_HPP
#define PLAYINTROSCREEN_HPP

#include <SFML/Graphics.hpp>

class PlayIntroScreen {
public:
    PlayIntroScreen(sf::RenderWindow& window, sf::Font& titleFont, sf::Font& bodyFont);
    enum class IntroState { DONE, QUIT };
    IntroState run();

private:
    sf::RenderWindow& window;
    sf::Font& titleFont;
    sf::Font& bodyFont;
    sf::Text message;
    int messageIndex;

    std::vector<std::string> messages;
};

#endif // PLAYINTROSCREEN_HPP