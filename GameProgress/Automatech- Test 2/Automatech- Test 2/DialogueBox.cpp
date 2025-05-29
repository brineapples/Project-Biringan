#include "DialogueBox.h"
#include <iostream>
#include <fstream>

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

// Updates the text display each frame for typewriter effect
void DialogueBox::update() {
    if (!visible || finishedTyping)
        return;                                 // Skip if box is hidden or already done typing

    float elapsed = typeClock.getElapsedTime().asSeconds(); // Time since last update
    std::size_t targetCharIndex = static_cast<std::size_t>(elapsed * typeSpeed); // How many letters to show

    if (targetCharIndex > currentCharIndex) {
        // Update text one character at a time
        currentCharIndex = std::min(targetCharIndex, fullText.size());
        textDisplay.setString(fullText.substr(0, currentCharIndex));

        if (currentCharIndex >= fullText.size()) {
            finishedTyping = true;              // Finish typing when all characters are shown
        }
    }
}

// Draws the dialogue box and speaker label
void DialogueBox::draw() {
    if (!visible) return;                       // Only draw if visible

    window.draw(background);                    // Draw background box
    window.draw(textDisplay);                   // Draw the dialogue text

    if (!speakerName.empty()) {
        window.draw(speakerBackground);         // Draw name box
        window.draw(speakerText);               // Draw speaker name
    }
}

// Handles input to advance or fast-forward dialogue
void DialogueBox::handleInput(const sf::Event& event) {
    if (!visible) return;

    // Accept mouse click or spacebar
    if (event.type == sf::Event::MouseButtonPressed ||
        (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)) {

        if (!finishedTyping) {
            // If still typing, instantly show full text
            textDisplay.setString(fullText);
            finishedTyping = true;
            currentCharIndex = fullText.size();
        }
        else {
            nextDialogue();                     // Otherwise go to next dialogue
        }
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
