#include "BackgroundManager.h"
#include <iostream>

BackgroundManager::BackgroundManager(sf::RenderWindow& win)
    : window(win) {
    fadeRect.setSize(sf::Vector2f(window.getSize()));
    fadeRect.setFillColor(sf::Color(0, 0, 0, 0));
}

void BackgroundManager::setBackground(const std::string& imagePath) {
    if (texture.loadFromFile(imagePath)) {
        sprite.setTexture(texture);
        float windowWidth = static_cast<float>(window.getSize().x);
        float windowHeight = static_cast<float>(window.getSize().y);
        float textureWidth = static_cast<float>(texture.getSize().x);
        float textureHeight = static_cast<float>(texture.getSize().y);

        float scale = std::max(windowWidth / textureWidth, windowHeight / textureHeight); // Fill entire window

        sprite.setTexture(texture);
        sprite.setScale(scale, scale);

        // center the image if it's larger than the window
        float offsetX = (textureWidth * scale - windowWidth) / 2.0f;
        float offsetY = (textureHeight * scale - windowHeight) / 2.0f;
        sprite.setPosition(-offsetX, -offsetY);
        hasBackground = true;
        isHiddenFlag = false;
        currentBackgroundPath = imagePath;
    }
    else {
        std::cerr << "Failed to load background: " << imagePath << std::endl;
        hasBackground = false;
        currentBackgroundPath = "";
    }
    
}

void BackgroundManager::update(float deltaTime) {
    if (isFadingFlag) {
        fadeElapsed += deltaTime;
        fadeOpacity = std::min(255.0f, (fadeElapsed / fadeDuration) * 255.0f);
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(fadeOpacity)));

        if (fadeElapsed >= fadeDuration) {
            isFadingFlag = false;
            isHiddenFlag = true;
        }
    }
}

void BackgroundManager::draw() {
    if (hasBackground && !isHiddenFlag) {
        window.draw(sprite);
    }
    if (isFadingFlag || isHiddenFlag) {
        window.draw(fadeRect);
    }
}

void BackgroundManager::startFadeOut(float duration) {
    fadeDuration = duration;
    fadeElapsed = 0.0f;
    fadeOpacity = 0.0f;
    isFadingFlag = true;
    isHiddenFlag = false;
}

void BackgroundManager::resetFade() {
    isHiddenFlag = false;
    fadeOpacity = 255.0f;
    fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(fadeOpacity)));
}

bool BackgroundManager::isFading() const {
    return isFadingFlag;
}

bool BackgroundManager::isHidden() const {
    return isHiddenFlag;
}

std::string BackgroundManager::getCurrentBackground() const {
    return currentBackgroundPath;
}