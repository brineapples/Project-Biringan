#include "LoadScreen.h"
#include <iostream>

LoadScreen::LoadScreen(sf::RenderWindow& window, sf::Font& titleFont, sf::Font& bodyFont)
    : window(window), titleFont(titleFont), bodyFont(bodyFont), animationClock() {
    if (loadScreenTexture.loadFromFile("TitleSC.png")) {
        background.setTexture(loadScreenTexture);
    

        // Scale the background to fill the window while keeping the aspect ratio
        float windowWidth = static_cast<float>(window.getSize().x);
        float windowHeight = static_cast<float>(window.getSize().y);
        float textureWidth = static_cast<float>(loadScreenTexture.getSize().x);
        float textureHeight = static_cast<float>(loadScreenTexture.getSize().y);

        float scale = std::max(windowWidth / textureWidth, windowHeight / textureHeight);
        background.setScale(scale, scale);

        //center if overflow
        float offsetX = (textureWidth * scale - windowWidth) / 2.0f;
        float offsetY = (textureHeight * scale - windowHeight) / 2.0f;
        background.setPosition(-offsetX, -offsetY);
    }
    else {
        std::cerr << "Failed to load simba.png\n";
    }

    // Title
    //title.setFont(titleFont);
    //title.setString("Load");
    //title.setCharacterSize(64);
   // title.setFillColor(sf::Color::White);
    //title.setPosition(220, 80);

    // Load button
    loadButton.setFont(bodyFont);
    loadButton.setString("Load Saved Game");
    loadButton.setCharacterSize(50);
    loadButton.setFillColor(sf::Color::Black);
    loadButton.setPosition(50, 70);
    // Back button
    backButton.setFont(bodyFont);
    backButton.setString("Back");
    backButton.setCharacterSize(40);
    backButton.setFillColor(sf::Color::Black);
    backButton.setPosition(80, 700);
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
    if (loadButton.getGlobalBounds().contains(mousePos))
        loadButton.setFillColor(sf::Color::Red);
    else
        loadButton.setFillColor(sf::Color::Black);  // Restore to black

    if (backButton.getGlobalBounds().contains(mousePos))
        backButton.setFillColor(sf::Color::Red);
    else
        backButton.setFillColor(sf::Color::Black);  // Restore to black
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