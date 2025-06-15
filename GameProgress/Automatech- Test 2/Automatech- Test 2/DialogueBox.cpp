#include "DialogueBox.h"
#include <iostream>
#include <fstream>
#include <cstdio>

// Constructor: sets up the initial visuals for dialogue box and speaker name
DialogueBox::DialogueBox(sf::RenderWindow& win, sf::Font& fnt)
    : window(win), font(fnt), currentCharIndex(0), visible(false),
    finishedTyping(false), typeSpeed(50.0f), hasBackgroundImage(false)
{
    // Main dialogue box setup
    background.setSize(sf::Vector2f(760, 120));                 // Dimensions of dialogue box
    background.setFillColor(sf::Color(0, 0, 0, 180));           // Semi-transparent black color
    background.setPosition(20, window.getSize().y - 140);       // Position at bottom of window
    background.setOutlineColor(sf::Color::White);               // White border
    background.setOutlineThickness(2);                          // Border thickness

    // Main text setup
    textDisplay.setFont(font);                                  // Set font
    textDisplay.setCharacterSize(20);                           // Font size
    textDisplay.setFillColor(sf::Color::White);                 // Text color
    textDisplay.setPosition(30, window.getSize().y - 130);      // Slightly inside the dialogue box

    // Speaker name background setup
    speakerBackground.setSize(sf::Vector2f(200, 40));           // Box size for speaker's name
    speakerBackground.setFillColor(sf::Color(0, 0, 0, 200));    // More opaque background
    speakerBackground.setPosition(20, window.getSize().y - 190); // Just above the dialogue box
    speakerBackground.setOutlineColor(sf::Color::White);
    speakerBackground.setOutlineThickness(2);

    // Speaker name text
    speakerText.setFont(font);
    speakerText.setCharacterSize(18);
    speakerText.setFillColor(sf::Color::White);
    speakerText.setPosition(30, window.getSize().y - 185);

    // Setup speed icon rectangle (button)
    speedIcon.setSize(sf::Vector2f(40, 40));
    speedIcon.setFillColor(sf::Color(100, 100, 100, 180)); // semi-transparent dark gray
    speedIcon.setOutlineColor(sf::Color::White);
    speedIcon.setOutlineThickness(2);
    // Position it at bottom right inside dialogue box with margin
    speedIcon.setPosition(background.getPosition().x + background.getSize().x - 50,
        background.getPosition().y + background.getSize().y - 50);

    // Setup auto advance icon rectangle (button)
    autoAdvanceIcon.setSize(sf::Vector2f(40, 40));
    autoAdvanceIcon.setFillColor(sf::Color(100, 100, 100, 180)); // semi-transparent dark gray
    autoAdvanceIcon.setOutlineColor(sf::Color::White);
    autoAdvanceIcon.setOutlineThickness(2);
    // Position next to speed icon with 10px gap
    autoAdvanceIcon.setPosition(speedIcon.getPosition().x - 50, speedIcon.getPosition().y);
}

// Starts a dialogue sequence from a list of DialogueEntry
void DialogueBox::startDialogue(const std::vector<DialogueEntry>& entries) {
    allDialogues = entries;                  // Store all dialogues
    //currentDialogueIndex = 0;
    dialogues = std::queue<DialogueEntry>();    // Clear previous dialogues

    for (const auto& entry : entries)
        dialogues.push(entry);                 // Queue new dialogues

    loadGameState(); //LOOK HERE
    visible = true;                             // Make the box visible
}

void DialogueBox::saveGameState() { //LOOK HERE
    std::ofstream saveFile("savegame.txt");
    if (saveFile.is_open()) {
        // Save the number of dialogues left as a simple example
        saveFile << currentDialogueIndex << std::endl;
        std::cout << "[SAVE] Saved index: " << currentDialogueIndex << std::endl;
        // You can add more state info as needed
        saveFile.close();
    }
}

void DialogueBox::loadGameState() { //LOOK HERE
    std::ifstream saveFile("savegame.txt");
    if (saveFile.is_open()) {
        saveFile >> currentDialogueIndex;
        std::cout << "[LOAD] Loaded index: " << currentDialogueIndex << std::endl;
        saveFile.close();
        // Rebuild the queue from the index
        dialogues = std::queue<DialogueEntry>();
        for (size_t i = currentDialogueIndex; i < allDialogues.size(); ++i)
            dialogues.push(allDialogues[i]);
        nextDialogue();
        visible = true;
    }
}

// Move to the next dialogue line
void DialogueBox::nextDialogue() {
    if (dialogues.empty()) {
        visible = false;                        // If no more dialogues, hide the box
        return;
    }

    DialogueEntry entry = dialogues.front();    // Get the next entry
    dialogues.pop();                            // Remove it from the queue

    fullText = entry.text;                      // Full text to type
    speakerName = entry.speaker;                // Who's speaking

    currentCharIndex = 0;                       // Reset typing animation
    textDisplay.setString("");                  // Clear current text
    speakerText.setString(speakerName);         // Show speaker name
    typeClock.restart();                        // Reset typing clock
    finishedTyping = false;                     // Reset typing status
    currentDialogueIndex++; //LOOK HERE
    saveGameState(); //LOOK HERE
}

// Update function - handle automatic advancing
void DialogueBox::update() {
    if (!visible) return;
    if (!finishedTyping) {
        float elapsed = typeClock.getElapsedTime().asSeconds();
        std::size_t targetCharIndex = static_cast<std::size_t>(elapsed * typeSpeed);
        if (targetCharIndex > currentCharIndex) {
            currentCharIndex = std::min(targetCharIndex, fullText.size());
            textDisplay.setString(fullText.substr(0, currentCharIndex));
            if (currentCharIndex >= fullText.size()) {
                finishedTyping = true;
                typeClock.restart();  // restart clock for auto advance timer
            }
        }
    }
    else if (autoAdvanceEnabled) {
        // Auto advance after 1.5 seconds (adjust as needed)
        if (typeClock.getElapsedTime().asSeconds() > 1.5f) {
            nextDialogue();
        }
    }
}

// Draws the dialogue box and speaker label
void DialogueBox::draw() {
    if (!visible) return;

    window.draw(background);
    window.draw(textDisplay);

    if (!speakerName.empty()) {
        window.draw(speakerBackground);
        window.draw(speakerText);
    }

    // Draw speed icon rectangle as button background
    window.draw(speedIcon);

    // Draw the number inside the icon that updates after clicking
    sf::Text speedText;
    speedText.setFont(font);
    speedText.setCharacterSize(20);
    speedText.setFillColor(sf::Color::White);
    speedText.setStyle(sf::Text::Bold);
    speedText.setString(std::to_string(currentSpeedMultiplier));

    // Center the text inside the speedIcon
    sf::FloatRect iconBounds = speedIcon.getGlobalBounds();
    sf::FloatRect textBounds = speedText.getLocalBounds();

    speedText.setPosition(
        iconBounds.left + (iconBounds.width - textBounds.width) / 2 - textBounds.left,
        iconBounds.top + (iconBounds.height - textBounds.height) / 2 - textBounds.top
    );

    window.draw(speedText);

    // Draw auto advance icon
    window.draw(autoAdvanceIcon);
    sf::Text autoText;
    autoText.setFont(font);
    autoText.setCharacterSize(16);
    autoText.setFillColor(autoAdvanceEnabled ? sf::Color::Green : sf::Color::Red);
    autoText.setString(autoAdvanceEnabled ? "A" : "X");
    sf::FloatRect autoBounds = autoAdvanceIcon.getGlobalBounds();
    sf::FloatRect autoTextBounds = autoText.getLocalBounds();
    autoText.setPosition(
        autoBounds.left + (autoBounds.width - autoTextBounds.width) / 2 - autoTextBounds.left,
        autoBounds.top + (autoBounds.height - autoTextBounds.height) / 2 - autoTextBounds.top
    );
    window.draw(autoText);
}


// Handles input to advance or fast-forward dialogue
void DialogueBox::handleInput(const sf::Event& event) {
    if (!visible) return;
    if (event.type == sf::Event::MouseButtonPressed) {
        float mx = static_cast<float>(event.mouseButton.x);
        float my = static_cast<float>(event.mouseButton.y);
        // Check icons clicks first
        if (handleSpeedIconClick(mx, my) || handleAutoAdvanceIconClick(mx, my)) {
            return;  // Clicking icons doesn't advance dialogue
        }
        if (!finishedTyping) {
            textDisplay.setString(fullText);
            finishedTyping = true;
            currentCharIndex = fullText.size();
        }
        else {
            nextDialogue();
        }
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            if (!finishedTyping) {
                textDisplay.setString(fullText);
                finishedTyping = true;
                currentCharIndex = fullText.size();
            }
            else {
                nextDialogue();
            }
        }
        if (event.key.code == sf::Keyboard::Z) {
            exitAndResetGame();
        }
        // Keyboard speed control (optional)
        if (event.key.code == sf::Keyboard::Num1) Forward(1);
        else if (event.key.code == sf::Keyboard::Num2) Forward(2);
        else if (event.key.code == sf::Keyboard::Num3) Forward(3);
    }
}

// Returns whether the dialogue box is currently shown
bool DialogueBox::isVisible() const {
    return visible;
}

// Loads and scales a background image to fill the window
void DialogueBox::setBackground(const std::string& imagePath) {
    if (backgroundTexture.loadFromFile(imagePath)) {
        backgroundSprite.setTexture(backgroundTexture);

        // Scale image to fit window size exactly
        float scaleX = static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x;
        float scaleY = static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);

        hasBackgroundImage = true;              // Mark that background is successfully loaded
    }
    else {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
        hasBackgroundImage = false;
    }
}

// Draws the background image (must call before draw() to appear behind dialogue box)
void DialogueBox::drawBackground() {
    if (hasBackgroundImage) {
        window.draw(backgroundSprite);
    }
}

void DialogueBox::exitAndResetGame() { // Reset Load State
    // Delete the save file to reset saved progress
    if (std::remove("savegame.txt") == 0) {
        std::cout << "[EXIT] Save file deleted successfully." << std::endl;
    }
    else {
        std::cout << "[EXIT] No save file to delete or deletion failed." << std::endl;
    }
    // Reset internal game state variables
    currentDialogueIndex = 0;
    dialogues = std::queue<DialogueEntry>();
    visible = false;
    std::cout << "[EXIT] Game state reset and exited." << std::endl;
}

// Forward : Change the speed of text
void DialogueBox::Forward(int multiplier) {
    if (multiplier >= 1 && multiplier <= 3) {
        currentSpeedMultiplier = multiplier;
        typeSpeed = 50.0f * multiplier;
        std::cout <<  "[SPEED] Typing speed set to " << multiplier <<  "x." << std::endl;
    }
}

// Function to handle speed icon clicks and cycle speed
bool DialogueBox::handleSpeedIconClick(float mouseX, float mouseY) {
    if (speedIcon.getGlobalBounds().contains(mouseX, mouseY)) {
        // Cycle speed 1 -> 2 -> 3 -> 1
        int nextSpeed = currentSpeedMultiplier + 1;
        if (nextSpeed > 3) nextSpeed = 1;
        Forward(nextSpeed);
        return true; // Click was on speed icon
    }
    return false; // Click was not on speed icon
}

// Toggle
void DialogueBox::ToggleAutoAdvance() {
    autoAdvanceEnabled = !autoAdvanceEnabled;
    std::cout << "[AUTO ADVANCE] Auto advance is now " << (autoAdvanceEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

// Handle clicks on auto advance icon; return true if handled
bool DialogueBox::handleAutoAdvanceIconClick(float mouseX, float mouseY) {
    if (autoAdvanceIcon.getGlobalBounds().contains(mouseX, mouseY)) {
        ToggleAutoAdvance();
        return true;
    }
    return false;
}



