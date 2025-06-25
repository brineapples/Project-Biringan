#include "TitleScreen.h"
#include <cmath>
#include <iostream>

// Constructor to initialize assets and layout
TitleScreen::TitleScreen(sf::RenderWindow& win, sf::Font& titleFont, sf::Font& bodyFont, SoundManager& soundManager)
    : window(win), titleFont(titleFont), bodyFont(bodyFont), soundManager(soundManager)
{
    backgroundTexture.loadFromFile("newfront.png");
    background.setTexture(backgroundTexture);

    float scaleX = static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x;
    float scaleY = static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y;
    background.setScale(scaleX, scaleY);

    // Load Baybayin button images
    playTexture.loadFromFile("start1.jpg");
    loadTexture.loadFromFile("buhayin.png");
    aboutTexture.loadFromFile("gabay.png");
    quitTexture.loadFromFile("exit.png");

    playSprite.setTexture(playTexture);
    loadSprite.setTexture(loadTexture);
    aboutSprite.setTexture(aboutTexture);
    quitSprite.setTexture(quitTexture);

    // Scale down
    float targetWidth = 130.f;
    float scalePlay = targetWidth / playTexture.getSize().x;
    float scaleLoad = targetWidth / loadTexture.getSize().x;
    float scaleAbout = targetWidth / aboutTexture.getSize().x;
    float scaleQuit = targetWidth / quitTexture.getSize().x;

    playSprite.setScale(scalePlay, scalePlay);
    loadSprite.setScale(scaleLoad, scaleLoad);
    aboutSprite.setScale(scaleAbout, scaleAbout);
    quitSprite.setScale(scaleQuit, scaleQuit);

    // 🔄 Center origin of each sprite (so we can align center to circles)
    playSprite.setOrigin(playTexture.getSize().x / 2, playTexture.getSize().y / 2);
    loadSprite.setOrigin(loadTexture.getSize().x / 2, loadTexture.getSize().y / 2);
    aboutSprite.setOrigin(aboutTexture.getSize().x / 2, aboutTexture.getSize().y / 2);
    quitSprite.setOrigin(quitTexture.getSize().x / 2, quitTexture.getSize().y / 2);

    // 🔧 Position aligned with circles
    float xCenter = 1260; // Align center X with glowing dots
    float yStart = 160;   // First circle center Y
    float spacing = 120;  // Distance between dots

    playSprite.setPosition(xCenter, yStart);
    loadSprite.setPosition(xCenter, yStart + 1 * spacing);
    aboutSprite.setPosition(xCenter, yStart + 2 * spacing);
    quitSprite.setPosition(xCenter, yStart + 3 * spacing);
}

TitleScreen::GameState TitleScreen::run() {
    sf::Clock clock;
    soundManager.playMusic("BeginM.ogg", true);

    while (window.isOpen()) {
        GameState state = handleEvents();
        if (state != GameState::TITLE) return state;

        float time = clock.getElapsedTime().asSeconds();
        animateTitles(time);

        draw();
    }

    return GameState::QUIT;
}

void TitleScreen::handleMouseHover(const sf::Vector2f& mousePos) {
    auto handleHover = [&](sf::Sprite& sprite) {
        if (sprite.getGlobalBounds().contains(mousePos))
            sprite.setColor(sf::Color(255, 100, 100));  // reddish hover
        else
            sprite.setColor(sf::Color::White);
        };

    handleHover(playSprite);
    handleHover(loadSprite);
    handleHover(aboutSprite);
    handleHover(quitSprite);
}

TitleScreen::GameState TitleScreen::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (playSprite.getGlobalBounds().contains(mousePos)) {
                soundManager.stopMusic();
                return GameState::GAMEPLAY;
            }
            if (loadSprite.getGlobalBounds().contains(mousePos))
                return GameState::LOAD;
            if (aboutSprite.getGlobalBounds().contains(mousePos))
                return GameState::ABOUT;
            if (quitSprite.getGlobalBounds().contains(mousePos)) {
                window.close();
                return GameState::QUIT;
            }
        }
    }

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    handleMouseHover(mousePos);

    return GameState::TITLE;
}

void TitleScreen::animateTitles(float time) {
    // Optional animations if you ever bring text back
}

void TitleScreen::draw() {
    window.clear();
    window.draw(background);
    window.draw(playSprite);
    window.draw(loadSprite);
    window.draw(aboutSprite);
    window.draw(quitSprite);
    window.display();
}