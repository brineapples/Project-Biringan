#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <cmath>
#include <string>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Escape from Biringan - Title Screen");
    window.setFramerateLimit(90);

    //Setup the Sprite with a Texture
    //Load Background Image
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("./Wall.jpg")) {
        return -1; // Error loading the background image
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    
    // Automatically scale the background image to fill the entire window
    sf::Vector2u windowSize = window.getSize(); //get the size of the window
    sf::Vector2u textureSize = backgroundTexture.getSize(); //get original size of the image

    // Calculate how much to scale the image in X and Y to match window dimensions
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
     
    backgroundSprite.setScale(scaleX, scaleY); // apply scaling so image fits perfectly
    
    //*************************************************************************************************************************
    //REMINDER - AT THE STARTING SCREEN THERE MUST ALSO BE A SCARY PERSON AT THE BOTTOM RIGHT WITH ITS EYES OPENING AND CLOSING
    //Waiting for the drawing P.S
    //*************************************************************************************************************************


    // Load font
    sf::Font font;
    if (!font.loadFromFile("BlackDahlia.ttf")) {
        return -1; // Make sure the font file exists
    }
   
    // Title text: Line 1 - "Escape from"
    sf::Text titleLine1("Escape from", font, 48);
    titleLine1.setFillColor(sf::Color::White);
    titleLine1.setStyle(sf::Text::Bold);
    titleLine1.setPosition(50, 30); // Top-left corner

    // Title text: Line 2 - "Biringan City"
    sf::Text titleLine2("Biringan City", font, 60);
    titleLine2.setFillColor(sf::Color::White);
    titleLine2.setStyle(sf::Text::Bold);
    titleLine2.setPosition(30, 100); // Below line 1

    //Play Music
    sf::Music titleMusic;
    if (!titleMusic.openFromFile("./BeginMusic.ogg")) {
        return -1; // Error loading music
    }
    titleMusic.setLoop(true); // Loop it like background music
    titleMusic.play();        // Start the music*/

    // Clock for animation
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            //When the player presses Enter, we stop the music.
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                titleMusic.stop();
                //Note i will change it and will stop when the player officialy starts the game
            }
        }
    

        float time = clock.getElapsedTime().asSeconds();

        // Spooky animation (Escape from)
        float offsetY1 = std::sin(time * 3.0f) * 2.0f; // subtle float
        float scale1 = 1.0f + std::sin(time * 2.0f) * 0.01f;
        titleLine1.setPosition(30, 30 + offsetY1);
        titleLine1.setScale(scale1, scale1);

        // Spooky animation (Biringan City)
        float offsetY2 = std::sin(time * 2.5f + 1.0f) * 5.0f; // more dramatic float
        float scale2 = 1.0f + std::sin(time * 3.5f + 0.5f) * 0.02f;
        titleLine2.setPosition(30, 100 + offsetY2);
        titleLine2.setScale(scale2, scale2);


        window.clear(sf::Color::Black);
        //Draw Sprite
        window.draw(backgroundSprite);
        //Draw title1
        window.draw(titleLine1);
        //Draw title2
        window.draw(titleLine2);
        //Display and put fourth the graphics scene.
        window.display();
    }

    return 0;
}