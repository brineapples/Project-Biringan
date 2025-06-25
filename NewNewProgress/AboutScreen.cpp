
#include "AboutScreen.h"
#include <iostream>
//sets up the screen content
AboutScreen::AboutScreen(sf::RenderWindow& win, sf::Font& titleFnt, sf::Font& bodyFnt)
    : window(win), titleFont(titleFnt), bodyFont(bodyFnt)
{
    // Load the background texture from file
    if (backgroundTexture.loadFromFile("TitleSC.png")) {
        background.setTexture(backgroundTexture);

        // Scale the background to fill the window while keeping the aspect ratio
        float windowWidth = static_cast<float>(window.getSize().x);
        float windowHeight = static_cast<float>(window.getSize().y);
        float textureWidth = static_cast<float>(backgroundTexture.getSize().x);
        float textureHeight = static_cast<float>(backgroundTexture.getSize().y);

        float scale = std::max(windowWidth / textureWidth, windowHeight / textureHeight);
        background.setScale(scale, scale);

        // Optional: center if it overflows
        float offsetX = (textureWidth * scale - windowWidth) / 2.0f;
        float offsetY = (textureHeight * scale - windowHeight) / 2.0f;
        background.setPosition(-offsetX, -offsetY);
    }
    else {
        std::cerr << "Failed to load simba.png\n";
    }

    // Set up the About text (information about the game)
    aboutText.setFont(bodyFont);
    aboutText.setString("This is a story-driven visual novel...\nMade with SFML and C++.");
    aboutText.setCharacterSize(50);   // Medium size
    aboutText.setFillColor(sf::Color::Black);
    aboutText.setPosition(90, 140);   // Position on the screen

    // Use body font for back button
    backButton.setFont(bodyFont);
    backButton.setString("Back");
    backButton.setCharacterSize(40);  // Slightly larger text for a button
    backButton.setFillColor(sf::Color::Black);
    backButton.setPosition(80, 700);  // Placed near the bottom

}

// Main loop for the About screen
AboutScreen::AboutState AboutScreen::run() {
    while (window.isOpen()) {
        // Check for input events
        AboutState state = handleEvents();

        // If the player chooses to go back, return to previous state
        if (state != AboutState::ABOUT)
            return state;

        draw();
    }
    // If window closes or exits loop unexpectedly, default to BACK
    return AboutState::BACK;
}

// Handle hover effect on "Back" button
void AboutScreen::handleMouseHover(const sf::Vector2f& mousePos) {
    // If mouse is over the back button, highlight it red; otherwise white
    backButton.setFillColor(backButton.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::Black);
}

// Handle input events (click on "Back" button or close window)
AboutScreen::AboutState AboutScreen::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (backButton.getGlobalBounds().contains(mousePos))
                return AboutState::BACK;
        }
    }
    // Update hover color based on current mouse position
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    handleMouseHover(mousePos);
    return AboutState::ABOUT;
}

// Draw the About screen
void AboutScreen::draw() {
    window.clear();
    window.draw(background);
    window.draw(aboutText);
    window.draw(backButton);
    window.display();
}