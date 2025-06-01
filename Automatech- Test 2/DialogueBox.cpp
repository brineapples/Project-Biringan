#include "DialogueBox.h"
#include <iostream>

DialogueBox::DialogueBox(sf::RenderWindow& win, sf::Font& fnt)
    : window(win), font(fnt), currentCharIndex(0), visible(false),
    finishedTyping(false), typeSpeed(50.0f), hasBackgroundImage(false)
{
    background.setSize(sf::Vector2f(760, 120));
    background.setFillColor(sf::Color(0, 0, 0, 180));
    background.setPosition(20, window.getSize().y - 140);
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(2);

    textDisplay.setFont(font);
    textDisplay.setCharacterSize(20);
    textDisplay.setFillColor(sf::Color::White);
    textDisplay.setPosition(30, window.getSize().y - 130);

    speakerBackground.setSize(sf::Vector2f(200, 40));
    speakerBackground.setFillColor(sf::Color(0, 0, 0, 200));
    speakerBackground.setPosition(20, window.getSize().y - 190);
    speakerBackground.setOutlineColor(sf::Color::White);
    speakerBackground.setOutlineThickness(2);

    speakerText.setFont(font);
    speakerText.setCharacterSize(18);
    speakerText.setFillColor(sf::Color::White);
    speakerText.setPosition(30, window.getSize().y - 185);
}

void DialogueBox::startDialogue(const std::vector<DialogueEntry>& entries) {
    dialogues = std::queue<DialogueEntry>();
    for (const auto& entry : entries)
        dialogues.push(entry);
    nextDialogue();
    visible = true;
}

void DialogueBox::nextDialogue() {
    if (dialogues.empty()) {
        visible = false;
        return;
    }

    DialogueEntry entry = dialogues.front();
    dialogues.pop();
    fullText = entry.text;
    speakerName = entry.speaker;
    currentCharIndex = 0;
    textDisplay.setString("");
    speakerText.setString(speakerName);
    typeClock.restart();
    finishedTyping = false;
}

void DialogueBox::update() {
    if (!visible || finishedTyping)
        return;

    float elapsed = typeClock.getElapsedTime().asSeconds();
    std::size_t targetCharIndex = static_cast<std::size_t>(elapsed * typeSpeed);

    if (targetCharIndex > currentCharIndex) {
        currentCharIndex = std::min(targetCharIndex, fullText.size());
        textDisplay.setString(fullText.substr(0, currentCharIndex));

        if (currentCharIndex >= fullText.size()) {
            finishedTyping = true;
        }
    }
}

void DialogueBox::draw() {
    if (!visible) return;
    window.draw(background);
    window.draw(textDisplay);
    if (!speakerName.empty()) {
        window.draw(speakerBackground);
        window.draw(speakerText);
    }
}

void DialogueBox::handleInput(const sf::Event& event) {
    if (!visible) return;

    if (event.type == sf::Event::MouseButtonPressed ||
        (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)) {
        if (!finishedTyping) {
            textDisplay.setString(fullText);
            finishedTyping = true;
            currentCharIndex = fullText.size();
        }
        else {
            nextDialogue();
        }
    }
}

bool DialogueBox::isVisible() const {
    return visible;
}

void DialogueBox::setBackground(const std::string& imagePath) {
    if (backgroundTexture.loadFromFile(imagePath)) {
        backgroundSprite.setTexture(backgroundTexture);
        float scaleX = static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x;
        float scaleY = static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
        hasBackgroundImage = true;
    }
    else {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
        hasBackgroundImage = false;
    }
}

void DialogueBox::drawBackground() {
    if (hasBackgroundImage) {
        window.draw(backgroundSprite);
    }
}

std::string DialogueBox::getCurrentDialogue() const {
    return fullText;
}