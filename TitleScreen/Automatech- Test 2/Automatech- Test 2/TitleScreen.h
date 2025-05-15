#ifndef TITLESCREEN_HPP
#define TITLESCREEN_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class TitleScreen {
public:
    TitleScreen(sf::RenderWindow& win, sf::Font& titleFont, sf::Font& bodyFont, sf::Music& music);
    void draw();
    enum class GameState { TITLE, ABOUT, GAMEPLAY, QUIT };
    GameState run();

private:
    sf::RenderWindow& window;
    sf::Font& titleFont;
    sf::Font& bodyFont;
    sf::Music& titleMusic;

    sf::Texture backgroundTexture;
    sf::Sprite background;
    sf::Text titleLine1;
    sf::Text titleLine2;
    sf::Text playButton;
    sf::Text aboutButton;
    sf::Text quitButton;

    void animateTitles(float time);
    GameState handleEvents();
    void handleMouseHover(const sf::Vector2f& mousePos);
};

#endif // TITLESCREEN_HPP