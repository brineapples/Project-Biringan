#ifndef ABOUTSCREEN_HPP
#define ABOUTSCREEN_HPP

#include <SFML/Graphics.hpp>

class AboutScreen {
public:
    // Enum to indicate what to do after About screen (returning back)
    enum class AboutState { ABOUT, BACK };

    // Constructor to set up the screen
    AboutScreen(sf::RenderWindow& window, sf::Font& titleFont, sf::Font& bodyFont);

    // Main loop for the About screen
    AboutState run();

private:
    sf::RenderWindow& window;
    sf::Font& titleFont;
    sf::Font& bodyFont;

    sf::Texture backgroundTexture;
    sf::Sprite background;
    sf::Text aboutText;
    sf::Text backButton;

    void draw();  // Draw everything
    AboutState handleEvents();  // Handle mouse clicks
    void handleMouseHover(const sf::Vector2f& mousePos);  // Highlight "Back" button on hover
};

#endif // ABOUTSCREEN_HPP