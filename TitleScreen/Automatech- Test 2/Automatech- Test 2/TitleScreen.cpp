#include "TitleScreen.h"
#include <cmath>

// Constructor to initialize assets and title screen layout
TitleScreen::TitleScreen(sf::RenderWindow& win, sf::Font& titleFont, sf::Font& bodyFont, sf::Music& music)
    : window(win), titleFont(titleFont), bodyFont(bodyFont), titleMusic(music)
{
    // Load and set the background image
    backgroundTexture.loadFromFile("Wall.jpg");
    background.setTexture(backgroundTexture);

    float scaleX = (float)window.getSize().x / backgroundTexture.getSize().x;
    float scaleY = (float)window.getSize().y / backgroundTexture.getSize().y;
    background.setScale(scaleX, scaleY);

    // Set title texts with title font
    titleLine1.setFont(titleFont);
    titleLine1.setString("Escape from");
    titleLine1.setCharacterSize(48);
    titleLine1.setFillColor(sf::Color::White);
    titleLine1.setPosition(50, 30);

    titleLine2.setFont(titleFont);
    titleLine2.setString("Biringan City");
    titleLine2.setCharacterSize(60);
    titleLine2.setFillColor(sf::Color::White);
    titleLine2.setPosition(30, 100);

    // Set button texts with body font
    playButton.setFont(bodyFont);
    aboutButton.setFont(bodyFont);
    quitButton.setFont(bodyFont);
    playButton.setString("Play");
    aboutButton.setString("About");
    quitButton.setString("Quit");
    playButton.setCharacterSize(36);
    aboutButton.setCharacterSize(36);
    quitButton.setCharacterSize(36);
    playButton.setPosition(50, 250);
    aboutButton.setPosition(50, 320);
    quitButton.setPosition(50, 390);
}

// Main game loop handling events and screen updates
TitleScreen::GameState TitleScreen::run() {
    sf::Clock clock;
    titleMusic.setLoop(true);
    titleMusic.play();

    while (window.isOpen()) {
        GameState state = handleEvents();  // Handle events like mouse clicks
        if (state != GameState::TITLE) return state;

        // Get the elapsed time for title animation
        float time = clock.getElapsedTime().asSeconds();
        animateTitles(time);  // Animate the titles

        draw();  // Draw everything on the window
    }
    return GameState::QUIT;
}

// Change button color when mouse hovers over them
void TitleScreen::handleMouseHover(const sf::Vector2f& mousePos) {
    playButton.setFillColor(playButton.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::White);
    aboutButton.setFillColor(aboutButton.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::White);
    quitButton.setFillColor(quitButton.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::White);
}

// Handle user input (mouse clicks)
TitleScreen::GameState TitleScreen::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();  // Close the window when the close button is clicked

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            // Get mouse position relative to the window
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            // Check if any of the buttons were clicked
            if (playButton.getGlobalBounds().contains(mousePos)) {
                titleMusic.stop();  // Stop background music when entering gameplay
                return GameState::GAMEPLAY;  // Go to the gameplay state
            }
            if (aboutButton.getGlobalBounds().contains(mousePos))
                return GameState::ABOUT;  // Go to the about state
            if (quitButton.getGlobalBounds().contains(mousePos)) {
                window.close();  // Close the window when Quit is clicked
                return GameState::QUIT;  // Quit the game
            }
        }
    }

    // Handle mouse hover effect
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    handleMouseHover(mousePos);
    return GameState::TITLE;  // Default state is TITLE
}

// Animate title text (floating and scaling effect)
void TitleScreen::animateTitles(float time) {
    float offsetY1 = std::sin(time * 3.0f) * 2.0f;
    float scale1 = 1.0f + std::sin(time * 2.0f) * 0.01f;
    float offsetY2 = std::sin(time * 2.5f + 1.0f) * 5.0f;
    float scale2 = 1.0f + std::sin(time * 3.5f + 0.5f) * 0.02f;

    // Apply floating effect and scaling to both title lines
    titleLine1.setPosition(50, 30 + offsetY1);
    titleLine2.setPosition(30, 100 + offsetY2);
    titleLine1.setScale(scale1, scale1);
    titleLine2.setScale(scale2, scale2);
}

// Draw everything to the window
void TitleScreen::draw() {
    window.clear();  // Clear the screen
    window.draw(background);  // Draw the background
    window.draw(titleLine1);  // Draw the first line of the title
    window.draw(titleLine2);  // Draw the second line of the title
    window.draw(playButton);  // Draw the Play button
    window.draw(aboutButton);  // Draw the About button
    window.draw(quitButton);  // Draw the Quit button
    window.display();  // Update the screen
}