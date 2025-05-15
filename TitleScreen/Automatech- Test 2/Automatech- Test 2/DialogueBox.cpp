#include "DialogueBox.h"

DialogueBox::DialogueBox(sf::RenderWindow& win, sf::Font& fnt)
    : window(win), font(fnt), currentCharIndex(0), visible(false),
    finishedTyping(false), typeSpeed(50.0f)
{
    // Main dialogue background
    background.setSize(sf::Vector2f(760, 120));                 // Width and height of dialogue box
    background.setFillColor(sf::Color(0, 0, 0, 180));          // Semi-transparent black
    background.setPosition(20, window.getSize().y - 140);      // Bottom of window
    background.setOutlineColor(sf::Color::White);              // White border
    background.setOutlineThickness(2);                          

    // Text display
    textDisplay.setFont(font);
    textDisplay.setCharacterSize(20);
    textDisplay.setFillColor(sf::Color::White);
    textDisplay.setPosition(30, window.getSize().y - 130);

    // Speaker name box
    speakerBackground.setSize(sf::Vector2f(200, 40));
    speakerBackground.setFillColor(sf::Color(0, 0, 0, 200));       // Slightly more opaque
    speakerBackground.setPosition(20, window.getSize().y - 190);  // Just above the dialogue box
    speakerBackground.setOutlineColor(sf::Color::White);
    speakerBackground.setOutlineThickness(2);

    // Speaker name text
    speakerText.setFont(font);
    speakerText.setCharacterSize(18);
    speakerText.setFillColor(sf::Color::White);
    speakerText.setPosition(30, window.getSize().y - 185);
}

// Initializes the dialogue sequence with a list of dialogue entries
void DialogueBox::startDialogue(const std::vector<DialogueEntry>& entries) {
    dialogues = std::queue<DialogueEntry>();            // Clear previous dialogue
    for (const auto& entry : entries)                   
        dialogues.push(entry);                          // Queue each new dialogue line

    nextDialogue();                                     // Start with the first entry
    visible = true;                                     // Show the dialogue box
}

// Loads the next dialogue entry
void DialogueBox::nextDialogue() {
    if (dialogues.empty()) {
        visible = false;
        return;
    }

    DialogueEntry entry = dialogues.front();             // Get next line
    dialogues.pop();                                     // Remove it from the queue

    fullText = entry.text;                                // Full dialogue line   
    speakerName = entry.speaker;                          // Who is speaking

    currentCharIndex = 0;                                 // Reset typing index   
    textDisplay.setString("");                             // Clear display text
    speakerText.setString(speakerName);                    // Set speaker name
    typeClock.restart();                                    // Start clock for typing animation 
    finishedTyping = false;                               // Reset typing status  
}

// Handles the typewriter animation for the dialogue text
void DialogueBox::update() {
    if (!visible || finishedTyping)
        return;

    float elapsed = typeClock.getElapsedTime().asSeconds();             // Time since last update
    std::size_t targetCharIndex = static_cast<std::size_t>(elapsed * typeSpeed);        // Calculate how many characters should show

    if (targetCharIndex > currentCharIndex) {
        currentCharIndex = std::min(targetCharIndex, fullText.size());      
        textDisplay.setString(fullText.substr(0, currentCharIndex));        // Show substring of the full dialogue

        if (currentCharIndex >= fullText.size()) {
            finishedTyping = true;
        }
    }
}

// Renders the dialogue box, text, and speaker name
void DialogueBox::draw() {
    if (!visible) return;

    window.draw(background);
    window.draw(textDisplay);

    // Draw speaker name box if not empty
    if (!speakerName.empty()) {
        window.draw(speakerBackground);
        window.draw(speakerText);
    }
}

// Handles mouse or keyboard input for advancing or fast-forwarding dialogue
void DialogueBox::handleInput(const sf::Event& event) {
    if (!visible) return;

    if (event.type == sf::Event::MouseButtonPressed ||
        (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)) {

        if (!finishedTyping) {
            // Instantly show full text if typing is still in progress
            textDisplay.setString(fullText);
            finishedTyping = true;
            currentCharIndex = fullText.size();
        }
        else {
            nextDialogue();  // Proceed to next dialogue
        }
    }
}

// Returns whether the dialogue box is currently visible
bool DialogueBox::isVisible() const {
    return visible;
}