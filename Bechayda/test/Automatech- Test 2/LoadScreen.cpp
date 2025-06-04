#include "LoadScreen.h"
#include <iostream>

LoadScreen::LoadScreen(sf::RenderWindow& window, sf::Font& titleFont, sf::Font& bodyFont)
    : window(window), titleFont(titleFont), bodyFont(bodyFont), animationClock() {
    // Background color or texture
    background.setSize(sf::Vector2f(window.getSize()));
    background.setFillColor(sf::Color(30, 30, 60)); // Dark blue background

    // Title
    //title.setFont(titleFont);
    //title.setString("Load");
    //title.setCharacterSize(64);
   // title.setFillColor(sf::Color::White);
    //title.setPosition(220, 80);

    // Load button
    loadButton.setFont(bodyFont);
    loadButton.setString("Load Saved Game");
    loadButton.setCharacterSize(35);
    loadButton.setFillColor(sf::Color(200, 200, 200));
    loadButton.setPosition(50, 20);
    // Back button
    backButton.setFont(bodyFont);
    backButton.setString("Back");
    backButton.setCharacterSize(30);
    backButton.setFillColor(sf::Color(200, 200, 200));
    backButton.setPosition(50, 500);
}

LoadScreen::LoadState LoadScreen::run() {
    while (window.isOpen()) {
        sf::Time elapsed = animationClock.getElapsedTime();
        animateTitle(elapsed.asSeconds());

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        handleMouseHover(mousePos);

        LoadState state = handleEvents(mousePos);
        if (state != LoadState::LOAD_SUCCESS) return state;

        draw();
    }
    return LoadState::BACK;
}

LoadScreen::LoadState LoadScreen::handleEvents(const sf::Vector2f& mousePos) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return LoadState::BACK;
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (loadButton.getGlobalBounds().contains(mousePos)) {
                std::cout << "Loading saved game...\n";
                return LoadState::LOAD_SUCCESS;
            }
            if (backButton.getGlobalBounds().contains(mousePos)) {
                return LoadState::BACK;
            }
        }
    }
    return LoadState::LOAD_SUCCESS;
}

void LoadScreen::handleMouseHover(const sf::Vector2f& mousePos) {
    // Hover color change
    if (loadButton.getGlobalBounds().contains(mousePos))
        loadButton.setFillColor(sf::Color::Red); // Red
    else
        loadButton.setFillColor(sf::Color(200, 200, 200));

    if (backButton.getGlobalBounds().contains(mousePos))
        backButton.setFillColor(sf::Color::Red); // Red
    else
        backButton.setFillColor(sf::Color(200, 200, 200));
}

void LoadScreen::animateTitle(float time) {
    float offset = std::sin(time * 2) * 5; // Sine wave for smooth bobbing
    title.setPosition(220, 80 + offset);
}

void LoadScreen::draw() {
    window.clear();
    window.draw(background);
    window.draw(title);
    window.draw(loadButton);
    window.draw(backButton);
    window.display();
}