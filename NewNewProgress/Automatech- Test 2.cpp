#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <cmath>
#include <string>

// Include custom headers for game screens and dialogue system
#include "TitleScreen.h"
#include "GameState.h"
#include "AboutScreen.h"
#include "PlayIntro.h"
#include "DialogueBox.h"
#include "BackgroundManager.h"
#include "SoundManager.h"
#include "LoadScreen.h"
#include "SaveManager.h"
#include "GameProgress.h"
#include "chapterManager.h"
#include "CharacterPortrait.h"
#include "ChoiceBox.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Escape from Biringan");

    // Load fonts
    sf::Font titleFont;
    if (!titleFont.loadFromFile("BlackDahlia.ttf")) {
        std::cerr << "Failed to load BlackDahlia.ttf\n";
        return -1;
    }

    sf::Font bodyFont;
    if (!bodyFont.loadFromFile("Railway.ttf")) {
        std::cerr << "Failed to load Railway.ttf\n";
        return -1;
    }

    // Initialize SoundManager 
    SoundManager& soundManager = SoundManager::getInstance();

    // Load sound effects
    if (!soundManager.loadSound("ECH1", "ECH1.ogg")) {
        std::cerr << "Failed to load ECH sound effect.\n";
    }

    // Play background music for the title screen
    if (!soundManager.playMusic("BeginM.ogg", true)) {
        std::cerr << "Failed to load background music.\n";
        return -1;
    }

    TitleScreen::GameState state = TitleScreen::GameState::TITLE;
    bool loadedFromSave = false;  // Track if "Load" was chosen

    while (window.isOpen()) {
        if (state == TitleScreen::GameState::TITLE) {
            TitleScreen titleScreen(window, titleFont, bodyFont, soundManager);
            state = titleScreen.run();
            loadedFromSave = false;  // Reset between transitions
        }
        else if (state == TitleScreen::GameState::ABOUT) {
            AboutScreen aboutScreen(window, titleFont, bodyFont);
            if (aboutScreen.run() == AboutScreen::AboutState::BACK) {
                state = TitleScreen::GameState::TITLE;
            }
        }
        else if (state == TitleScreen::GameState::LOAD) {
            LoadScreen loadScreen(window, titleFont, bodyFont);
            LoadScreen::LoadState loadState = loadScreen.run();
            if (loadState == LoadScreen::LoadState::BACK) {
                state = TitleScreen::GameState::TITLE;
            }
            else if (loadState == LoadScreen::LoadState::LOAD_SUCCESS) {
                GameProgress progress;
                if (SaveManager::loadProgress("savegame.dat", progress)) {
                    loadedFromSave = true;  // Set flag
                    state = TitleScreen::GameState::GAMEPLAY;
                }
                else {
                    std::cerr << "Failed to load saved game progress.\n";
                    state = TitleScreen::GameState::TITLE;
                }
            }
        }
        else if (state == TitleScreen::GameState::GAMEPLAY) {
            std::cout << "Starting gameplay...\n";

            BackgroundManager bgManager(window);
            DialogueBox dialogueBox(window, bodyFont);
            CharacterPortrait middlePortrait;

            if (!middlePortrait.load("mac.png")) {
                std::cerr << "Failed to load mysterious_character.png\n";
            }
            middlePortrait.setScale(0.6f, 0.6f);
            middlePortrait.setVisible(false);

            if (loadedFromSave) {
                GameProgress progress;
                if (SaveManager::loadProgress("savegame.dat", progress)) {
                    bgManager.setBackground(progress.backgroundImage);
                    std::vector<DialogueEntry> resume = {
                        {progress.dialogue, progress.speaker}
                    };
                    dialogueBox.startDialogue(resume);
                }
            }
            else {
                bgManager.setBackground("bgs.png");
                std::vector<DialogueEntry> dialogues = {
                    {"Dito ba talaga ang daan?", "???"},
                    {"Dapat talaga nagpasama ko. Masyadong madilim ang kalsada.", "???"},
                    {"Kaliwa ba o Kanan yung pupuntahan?", "???"},

                    {"Apo, puntahan mo nga muna bahay nila Auntie Amelita mo. Bigyan mo sya nang \nnatirang maja.", "Flashback(Mother)"},
                    {"Saan bahay ni Auntie?", "???"},
                    {"Tapat bahay nya sa municipal hall. Pangatlong bahay na may third floor. Naintindihan\nmo ?", "Flashback(Mother)"},
                    {"Opo!", "Flashback(???)"},
                    {"Sa tapat daw ng municipal hall....", "???"},
                    {"Malapit ka nang kumanan nang napansin mo na maraming nalabas na usok sa part na\nyun.", "???"},
                    {"Di mo ito ganong inisip at marami pa namang nagbebenta sa gabi.", "???"},
                    {"Try to make the smoke disperse using hand.", "???"},
                    {"Close your eyes.", "???"},
                    {"Ang dilim.", "???"},
                    {"Rub eyes.", "???"},
                    {"Dito ba talaga yun? Masyadong marangya parang lang kila tita. Hindi kaya mali\nyung liko ko?", "???"},
                    {"Looks back", "???"},
                    {"TANGINA- nasan– may– nasaan na yung mga bahay dito?", "???"},
                    {"Step back", "???"},
                    {"Fuck", "???"},
                    {"Look Back.\nPan on the city of Biringan", "???"}
                };
                dialogueBox.startDialogue(dialogues);
            }

            bool showDialogue = true;
            bool fadeTriggered = false;
            bool fadeCompleted = false;
            bool chapter2Started = false;
            sf::Clock clock;

            ChoiceBox choiceBox(window, bodyFont);
            bool choicePresented = false;

            while (window.isOpen()) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                        state = TitleScreen::GameState::TITLE;
                        break;
                    }
                    /*if (!choicePresented)
                        dialogueBox.handleInput(event);
                    else
                        choiceBox.handleEvent(event);*/
                    if (choiceBox.isVisible()) {
                        choiceBox.handleEvent(event);
                    }
                    else {
                        dialogueBox.handleInput(event);
                    }
                
                }

                float deltaTime = clock.restart().asSeconds();
                bgManager.update(deltaTime);
                dialogueBox.update();
                choiceBox.update();

                window.clear(sf::Color::Black);
                bgManager.draw();

                std::string currentDialogue = dialogueBox.getCurrentDialogue();

                if (showDialogue) {
                    if (currentDialogue == "You have entered the City of Biringan.") {
                        middlePortrait.setVisible(true);
                    }
                    else {
                        middlePortrait.setVisible(false);
                    }

                    // Center character portrait
                    if (middlePortrait.isVisible()) {
                        float centerX = window.getSize().x / 2.0f - middlePortrait.getGlobalBounds().width / 2.0f;
                        float posY = window.getSize().y * 0.2f;
                        middlePortrait.setPosition(centerX, posY);
                        middlePortrait.draw(window);
                    }

                    dialogueBox.draw();
                    choiceBox.draw(window);

                    if (currentDialogue == "Ang dilim." && !fadeTriggered) {
                        bgManager.setBackground("BlackBG.jpg");  
                        fadeTriggered = true;
                    }

                    if (currentDialogue == "Rub eyes." && !fadeCompleted) {
                        bgManager.setBackground("Biringan2.jpg");  
                        fadeCompleted = true;
                    }

                    if (currentDialogue == "Looks back") {
                        soundManager.playSound("ECH1");
                    }

                    if (currentDialogue == "Look Back.\nPan on the city of Biringan" && !chapter2Started) {
                        chapter2Started = true;

                        ChapterManager::transitionToChapter(window, bodyFont, "Chapter 2: Biringan");
                        ChapterManager::transitionToChapter(window, bodyFont, "TIME: 9:00 PM");
                        bgManager.setBackground("Biringan2.jpg");

                        std::vector<DialogueEntry> chapter2Dialogues = {
                            {"Mga naglalakihang mga gusali, maaayos ang mga daan at kay rami ang mga halaman\nang nakikita ko. Ibang-iba ito sa lugar na dapat na nakatayo duon. Parang isang\nbustling metropolitan kung sasabihin ng iba. Nararamdaman ko ang taas ng mga\nbuhok ko sa balahibo habang segu-segundo akong nakatayo sa gitna ng kalsadang. ", "???"},
                            {"nito. Tuloy tuloy ang galaw ng mga tao sa iba't ibang direksyon na parang normal lang\nang nangyayari.Na parang walang kakaiba ang napapagmasdan ko. Nabaling ang\ntingin ko sa gitna ng daan. Marami ang nagtitipon at pinapaganda ang daanan.", "???"},
                            {"Naglalagay ng mga ilaw, bandana, balloons at mga kung ano-ano sa mga streetlights\nna katulad duon sa Jones Bridge. May mga tao na may dala-dalang mga basket na\npuno ng pagkain at mga halaman. Halatang may pagdiriwang ang nangyayari.\nMaraming naka-suot ng pormal at may mga iba na parang buwan ng wika ang atake " ,"???"},
                            {"Rinig na rinig ang mga paguusap, ang tawa ng paglalaro ng mga bata at ang mga\nkanta at ang kasiyahan.Pinagmamasdan ko sila, Mahahaba ang kanilang buhok at iba\niba ang kanilang kulay, mga puti o kayumanggi ang balat, matatangkad at ang\nga-ganda at gwapo na mapapabaling tingin ka ulit at iisipin kung ibang." , "???"},
                            {"mundo ba sila", "???"},
                            {"Ibang mundo?", "???"},
                            {"Magaganda", "???"},
                            {"Para akong nabuhusan ng tubig nang maintindihan ko ang lahat ng nangyayari.","???"},
                            {"Akala ko sabi-sabi lang ito. Mito kuno ng mga matatanda pang bigay takot.", "???"},
                            {"Napahakbang ako pabalik ng marinig ko ang tunog ng kampana. Nagsitahimikan\nang mga tao.", "???"},
                            //SOUND EFFECTS HERE  DUM DUM DUM DUM
                            {"Kada tunog ng kampana ay padagdag ng padagdag ang takot sa puso ko. Pinwersa\nko ang sarili ko na tignan ang nangyayari sa likod. Nakita kong binubuksan ang\npinto ng isang lumang gusali. Namangha ako sa Nakita kong display. Marangya lang\nang mad-describe ko sa nakita ko. Gintong mga karwahe, mga mamahaling kotse,", "???"},
                            //Background image 
                            {"mga karong ginto na may puting lace na punong puno ng pagkain, gamit at\nkayamanan.Mayroon rin silang mga bantay nakasuot ng mga lumang kasuotan at--", "???"},
                            {"UNGGOY BA YUN!? BA'T ANLAKI?", "???"},
                            {"Bigla may humugot sakin at--", "???"},
                            {"Ah, parade nga pala tas nasa gitna ako ng daanan.", "???"},
                            {"Bata, di ka ba aware sa nangyayari!?", "Stranger"},
                            {"Isang matangkad na babae na na parang nasa bandang late 30s na. Mahaba at\nmaitim buhok at Morena at napakaganda. Pinoy na pinoy-- bilog ang mukha at\npango. Pero bakit parang may mali sa features nya. ", "???"},
                            {"Nasa gitna ka nang daan, di mo ba napansin na palapit na sayo yung mga karwahe!?\nHindi mo ba Nakita na nagmake-way kami at gumilid?", "Stranger"},
                            {"Nararamdaman ko ang tingin ng iba.", "???"},
                            {"Hindi po, sorry po. Paumanhin ko. ", "???"},
                            {"Ay nako, mga bata nga naman ngayon. Iling nya na may kasamang tsk.", "Stranger"},
                            {"San ka ba galing ha. Tignan mo suot mo! Nakapambahay lang. Akala mo di ka\npinalaki ng maayos ng magulang.", "Stranger"},
                            {"Sorry po talaga. Sabi ko pero nasa iba na ang atensyon ko.", "???"},
                            {"Ngayon ko lang napansin ang mga hiyawan at ingay ng mga tao. Nagpatuloy ang\nparada at iba-ibang spetakulo ang lumabas sa gusali. May mga taong nakikinuod\nrin ang lumabas.", "???"},
                            {"Nak, umuwi ka na.", "Stranger"},
                            {"'po?' Naputol ang aking panood at tumingin sa Ale.", "???"},
                            {"Kung ayaw mong manirahan ng habang buhay dito, umuwi ka na agad. Hanggang\nmaaga pa at wala pang masyadong impluwensya sila sayo.", "Stranger"},
                            {"Nakilabutan ako sa sinabi niya. Andaming tanong ang nasa isip ko. Sinong sila?\nBakit kailangan maaga? at paano ako makakauwi?", "???"},
                            {"Kung ano-ano ang pumapasok sa utak ko. Pero, alam ko na ang pinaka kailangan\nkong gawin ay tanungin kung pano makauwi.", "???"},
                            {"'Pano po?' Mabilis kong tanong.", "???"},
                            {"Mukhang alanganin na sumagot yung Ale at tingin ng tingin sa mga katabi namin.\nDun ko napansin ang mga tingin nila. Parang mga madarangit ang tingin nila.\nNakakapangliit at nakakatakot. nararamdaman ko ang mga titig nila sa likod ko. Kelan\npa sila nakatitig? Kanina pa nang mapunta ako o  Gusto kong tumakbo.", "???"},
                            {"Takbo kaya ako? Pero saang direksyon? Kaliwa? Kanan? Kung tatakbo ba ko\nmakakabalik kaya ako? Pano? Marami silang nandito, ano gagawin ko", "???"},
                            {"Bigla hinawakan ni Ale yung kamay ko. Hinimas-himas nya ang palad ko at\nnararamdaman ko may sinusulat sya sa kamay ko. Yumuko ako para mabasa ko ang\nsinusulat nya.", "???"}, //IMAGE PATH REMEMBER
                            {"'Walong Lagusan'", "???"},
                            {"'Hanap Mapa'", "???"},
                            {"'Dalaketnon Wag sama'", "???"},
                            {"Iniintindi ko pa ang sinusulat nya ng hinatak sya ng isang matangkad babae.Tumingala\nako at nakita na ang nerbyos at takot ng Ale. Hawak-hawak nung babae ang wrist niya\nat pwersahang pinaharap niya si Ale sa kanya. Niyakap sya patalikod at sinubukan ni\nAle na lumayo pero masyadong malakas ang yakap nung babae at di maka-alis si Ale.", "???"},
                            {"Nak, mauuna na kami ng asawa ko. Malapit na magsimula ang pagdaraos, may mga\nkailangan pa kaming ayusin at alagaang mga bata. Mag-ingat ka ah, wag magpagutom.\nSabi niya.", "Stranger 2"},
                            {"'Ale', Nerbyos kong sabi,", "???"},
                            {"Gusto ko tumulong. Gusto ko ilayo si Ale at halatang pinupwersa sya..", "???"},
                            {"'Okay ka lang ba?' Kailangan mo ba ng tulong’ gusto kong sabihin sa kanya ngunit\nparang alam nya na. Umiling sya at at kumaway. Tumingin ako sa isang babae at dun\nko nakita yung talas ng tingin nya sakin. Na akala mo sing tulis ng kutsilyo ang tingin.\nMas nakakapanindig balahibo pa ang kanyang mga titig kesa sa mga naramdaman ko", "???"},
                            {"ngayon. Gusto nya ko patayin. Galit sya. Halatang ayaw niya na may lumalapit o\nkumakausap dun kay Ale.", "???"},
                            //IMAGE OF THE EYE
                            {"Gusto ko gumalaw at umalis pero para akong naging estatwa na walang magawa\nkundi pagmasdan sila. Duon ko napansin ang mata nya. Baliktad ang nire-reflect ang\nimahe ko sa mga mata nya. Wala rin syang philtrum Tinignan ko ng mabilis ulit ang Ale\nat nakita ko sa kanyang mata na katulad yung sa kanya sa babae at pati sya ay wala", "???"},
                            {"ring philtrum.", "???"},
                            {"Huh. Maniwala pa rin ba ako sa sinabi niya? Kung parehas naman sila.", "???"},
                            {"Ngumiti at kumaway ulit si Ale at umalis na sila. Pero bago sya umalis ay binigkas\nang sa tingin ay ang salitang ‘Wag kang kumain’ at ngumiti ulit sya pero ngayon\nmay halong lungkot na. Na parang wala na syang magagawa pa. Napakaway na\nlang rin ako.", "???"},
                            {"Parang mas magandang mamatay, kung ganyan rin naman pala ang katumbas ng\npagpili mong manatili sa paraisong ganto. Paraiso nga ba talaga toh o isang hawla\npara sa mga naakit?", "???"},
                            {"Kailangan ko ng umuwi agad. ", "???"},
                            {"Andaming tanong ang pumapasok sa utak ko. Muli akong tumingin at nakita kong\npatapos na ang parada. Wala na rin ang tingin ng ibang mga tao sa amin. Ni-try ko\npakingan ang mga pag-uusap ng mga nakapaligid sakin. ", "???"},
                            {"Saan mo gusto pumunta? Alas dose pa dadating ang parada sa Madyas.", "Stranger 1"},
                            {"Pede tayo dumaan muna sa Sanmalan at kunin yung mga inorder mo. Pede na rin\ntayo kumain dun love.", "Stranger 2"},
                            {"Pede tas sunduin muna rin si Aye at yung mga kapatid mo sa may Mall", "Stranger 1"},
                            {"Kain tayo Tapul", "Stranger 1"},
                            {"Tawagan mo nga si Princess, kanina pa tayo nagi-intay.", "Stranger 1"},
                            {"Naglakad-lakad ako at naghanap kung mga may nakapaskil na mapa. Napa-isip ako\nsa dati kong napanood sa KMJS. sang ultramodern metropolis na mga engkanto ang\nnakatira. Konti lang ang naalala ko sa episode na yan. Ang pinakana-alala ko ay isa\nitong siyudad ng engkanto at dahil sa mitong toh nakilala yung bayan na kinalakihan\nnila mama.", "???"},
                            {"Naalala ko yung mga warnings sa sinabi ng Ale. Wag ka kumain ng kung anong\nibigay nila. Wag kang tumanggap ng kahit ano at isipin mo muna kung ano ang\nsasabihin mo at pede nila gamitin yun sayo at sabihin na may tinanggap ka galing sa\nkanila.", "???"},
                            {"Parang tao lang rin sila pero mas makapangyarihan. Volatile sila at kahit anong spin\nang gawin para gawing romantiko, may malaking rason bakit binabalaan ang\nmga nakikinig.", "???"},
                            //Background Image
                            {"May nahanap na rin akong board na parang may nakalagay na mapa. Katabi nito\nay ang isang shed na parang sa bus stop. Maayos at mas magandang bus stop.", "???"},
                            {"Binilisan ko ang paglakad at tinignan ang nakapaskil.", "???"},
                            //PICTURE OF THE MAP HERE NEEDED REMEMBER THIS
                            {"Natawa na lang ako.", "???"},
                            {"PUTANGINA. Ba’t ba ako nag-expect na naka-tagalog o bisaya ang nakasulat dito.\nParang baybayin ang nakasulat pero iba ang estilo at alam ko iba na ang ibig sabihin\nng mga salita.", "???"},
                            {"Habang nakatulala ako nakatingin sa mapa, kinapa ko kung dala ko ba yung phone ko.\nAnd lo and behold, di ko dala. Iniwanan ko sa higaan ko yun na naka-charge kasi akala\nko dyan lang. ", "???"},
                            {"Takteng buhay nga naman. Wala pa kong dalang pencil o kung anong pedeng\nmakatulong at ang dala dala ko lang ay yung pagkain na pinadala ni lola na di ko alam\nkung pano ko hindi pa toh natapon.", "???"},
                            {"Mamatay kaya ako dito. Mamatay ba ako, mastuck ako dito habang buhay. Change\nmuna ng thoughts. ", "???"},
                            //MAP HERE AGAIN
                            {"Tumingin ulit ako sa mapa, at ni-try ko itatak sya sa memorya ko. May apat silang\ndistrikto, nasa una ata akong distrikto. Di ko alam kung ano ang ibang simbolo, pero\niisipin ko base sa shape ay yung pabilog ay isa sa mga lagusan.", "???"},
                            {"Sa distrikto kung nasaan ako, may dalawang lagusan at mukhang yung sinasabing\nmalaking puno malapit sa city hall ang isa. ", "???"},
                            {"Pumunta ba ako sa isa o bumalik ulit dun at i-try bumalik?", "???"}
                            //CHOICES HERE


                        };
                        
                        dialogueBox.startDialogue(chapter2Dialogues);
                    }

                    

                    //PROBLEM HERE IF I CLICK PUMUNTA SA LAGUSAN IT WILL NOT GO TO THE NEXT DIALGOUE AND REMOVE THE CHOICES ALSO IT IS NOT CCENTERED RIGHT
                    if (currentDialogue == "Pumunta ba ako sa isa o bumalik ulit dun at i-try bumalik?" && !choiceBox.isVisible()) {
                        std::vector<Choice> choices = {
                            {"Pumunta", [&]() {
                                std::vector<DialogueEntry> choice1Dialogue = {
                                    //MAP WILL SHOW AGAIN
                                    {"Mas maganda kung itry ko muna pumunta sa isa pa. Tinignan ko muli ang mapa\nat sinundan ang direksyon na nakalagay.", "???"}, // MAPA Will Appear Again
                                    //BACKGROUN HERE ULIT
                                    {"Habang naglalakad ako, pinagmamasdan ko ang architecture ng distrikto. Totoo\nang sinabi nila na itim ang kanilang mga gusali.", "???"}, //BACKGROUND HERE
                                    {"Akala ko dahil gabi pero itim nga talaga ang kulay nila at mailaw.", "???"},
                                    {"Wala ako makitang billboards o advertisement. May artista kaya sila o may istasyon\nsila dito para panuorin yung mga pinapalabas satin. May wifi o signal kaya dito\npara makaka contact sa labas.", "???"},
                                    {"May engineer ba dito. Kailangan ba nila yun.", "???"},
                                    {"Kalahating oras pa ang nakalipas hanggang sa nakapunta na ako sa lugar ng isang\nlagusan. Tago sa loob ng isang malaking market.", "???"}, //BACKGROUND IMAGE
                                    {"Kitang kita ko ang rami ng tao (o engkanto) sa loob ng lugar.", "???"},
                                    {"Makakaya ko bang makalabas sa lugar na toh ng buo at intact ang kaluluwa ko?", "???"},
                                    {"Bahala na si Batman. Wala na akong oras para mag-alinlangan.", "???"},
                                    {"Sa kaliwa ako dumaan at napahanga ako sa mga bilihan. Para akong nasilaw ng\npagpasok ko. Ibang iba sya sa loob. Sari-sari ang binebenta at napakaraming pagkain,\nmga damit, laruan at kung ano-ano pa na binebenta sa mga ganitong market ang\nnandito. Parang bumalik lang ako sa Salcedo Market. Malalaki ng hinahaing pagkain\nat ang market ay hindi katulad sa labas na kulay black ang mga gusali.", "???"}, //BACKGROUND MAYBE
                                    {"Dito, makukulay at punong puno ng mga halaman at mga malalagong bulaklak.\nPunong puno ng buhay. May mga magagandang insekto tulad ng butterfly at mga\nmaliliit na ang hayop na nandito. May mga mukhang alaga at may iba na malayang\nnapasok at gumagala.", "???"},
                                    {"Napatingin ako sa mga binebentang pagkain. Chicken, Lobster, Seafoods, Lechon,\nCake at andami pa. Maraming puti at black na pagkain ditong binebenta. May kanin at\nparang pinipig na binebenta seperately. Di ko alam pero isa yan sa mga narinig kong\npagkain papunta dito, Tapul ata ang isa tawag nila.", "???"},
                                    {"Huminto ako sa isa sa mga stalls na nagbe-benta ng mga alahas at napansin ko na\niba ang way nila na pangbayad. Ang pinapangbayad nila ay maliliit ang gintong bilog.\nHindi ko alam kung proportional ba ang laki ng bilog o ang bigat nito sa presyo. ", "???"},  // MAYBE BACGKROUND 
                                    {"'Bago ka lang ba?'", "Male Stranger"},  // CHARACTER NAME MYSTERIOUS ???
                                    {"Napatingin ako bigla sa harapan ko. Sya yung isa sa mga natingin ng mga alahas sa\nstall na toh. Matangkad na chinitong lalaki.  Kanina nya pa kausap yung nagtitinda at\nnaghahanap ng iba’t ibang diyamante.", "???"},  //MACA PICTURE WILL MAYBE POP UP HERE 
                                    {"'Po?' nerbyos kong sagot.", "???"},
                                    {"Ngayong buwan ka lang ba binago?", "Male Stranger"},
                                    {"Binago? May ritwal ba na kailangan? I mean, alangan meron.", "???"},
                                    {"Nilahad niya ang kamay at di ko alam kung ano sasabihin ko o gagawin ko.", "???"}
                                };
                                dialogueBox.startDialogue(choice1Dialogue);
                                showDialogue = true; // Fix works now
                            }},
                            {"Bumalik", [&]() {
                                std::vector<DialogueEntry> choice2Dialogue = {
                                    {"Bumalik ako sa lugar kung saan ako nagmula. Baka may na-miss akong senyales.", "???"}
                                };
                                dialogueBox.startDialogue(choice2Dialogue);
                                showDialogue = true; // Fix works now
                            }}
                        };
                        choiceBox.startChoices(choices);
                    }

                    if (currentDialogue == "Nilahad niya ang kamay at di ko alam kung ano sasabihin ko o gagawin ko." && !choiceBox.isVisible()) {
                        std::vector<Choice> choices = {
                            {"Makipagkamay", [&]() {
                                std::vector<DialogueEntry> choice1Dialogue = {
                                    {"Para hindi maging awkward at ma-offend sya ay nakipagkamay ako at tinignan ko ang\nmata nya. Baliktad ang reflection at wala syang philtrum.", "???"}, // MAYBE A CLOSE UP TO HIS EYE
                                    {"'Kawawa ka naman.' Sabi niya na may awa sa mga mata niya. Nabahala ako sa\nsinabi nya.", "Male Stranger"},
                                    {"'Mukhang ang bata mo pa para matali dito sa mundong toh.'", "Male Stranger"},  //Stranger message
                                    {"Bata? Hindi ba dati may nakuha pa na mas bata sakin?", "???"},
                                    {"Hindi po ba mas mukha po kayong bata kesa sakin?", "???"},
                                    {"50 na ko, (Iha, Iho).", "???"}, //Stranger
                                    {"Sadyang di na ako tatanda simula nang binago ang anyo ko.", "Male Stranger"}, //Stranger
                                    {"Ilang taon po kayo nun?", "???"},
                                    {"23, Ilang taon ka na ba (Iha, Iho)?", "Male Stranger"}, //Stranger
                                    {"18 po.", "???"},
                                    {"Masyado ka pang bata.", "Male Stranger"},
                                    {"Sabi niya at dumating yung nagtitinda kasama ng binili niya.", "???"},
                                    {"'Salamat mare, sige maauna na kami.' Sabi niya habang kumakaway paalis at ginabay\nniya ako sa table nila.  For some reason, ang gaan ng pakiramdam ko sa kanya at\nparang gusto kong sumama.", "???"},
                                    {"May tatlong tao ang nakaupo sa table na yun. Isang babae at isang lalaki. Parehas\nmahaba ang buhok at compared sa iba ay mas casual at moderno ang suot.", "???"}, //MAYBE AN IMAGE
                                    {"Pagkalapit namin ay duon ko na mas nakita na ng maayos ang dalawa. Yung isa\ndun ay pamilyar sakin. Si Maca yun ah", "???"}, //TRANSITION TO A FLASHBACK IMAGE AFTER THIS IMAGE
                                    {"Pumunta kayo ng mga pinsan nyo sa covered court para manood ng mga maglalaro. ", "???"},
                                    {"Naghahanap kayo ng mauupuan nang napansin mo ang isang maglalaro.", "???"}, 
                                    {"Sa lahat ng maglalaro, sa kanya na pupunta ang atensyon ng lahat. Pano ba naman?\nSa tindig at galawa pa lang, ibang-iba na ang aura nya. Isama mo pa ang tangkad at\nkagwapuhan nya. May something sa kanya na parang nangaakit at hindi mo matanggal\nang iyong mga mata.", "???"}, //PICTURE OF MACA and background
                                    {"'[Name]!' Tawag sayo ng pinsan mo. Nabaling ang tingin mo sa pinsan na nakahanap\nna ng upuan at ilang beses ka nang tinatawag. ", "???"},
                                    {"'Wait lang!' Pasigaw mong sabi. Mabilis kang pumunta sa pwesto at umupo katabi\nnila.", "???"},
                                    {"'Insan, sino yung lalaking may mahabang buhok?' Tinuro mo yung lalaki.", "???"},
                                    {"Sya ? Si Maca. Apo ni Aleng Lina.Bumibisita rin katulad mo. Mga– Ilang taon ka na ba? ", "Pinsan"},
                                    {"19 po!", "???"},
                                    {"Kasing age mo lang. Mayayaman yang mga yan. Rinig ko nasa bandang manila yan\nnag-aaral. Di ko maalala kung anong school.", "Pinsan"},
                                    {"Ganun ba", "???"},
                                    {"Mhmm-mm", "???"},
                                    {"Marami pang nagsipasukan sa court, at napansin yun ni Mica. Lumapit sya sa iba sa\nkanila. Mukhang manonood yung pamilya nya sa mga palaro.", "???"},
                                    ///VESPER NIGHT
                                    {"Pinagayos at pinapunta kami ni Lola sa isa mga sa okasyon gabi bago mag-fiesta.\nVesper Night Socials yung pangalan ng event at gusto ni Lola na makisosyo sa mga\nkasing edad namin. ", "???"},
                                    {"Nainis pa si Lola nun at kalahating oras na ang nakalipas nang nag-start yung event\nat hindi pa kami tapos. ", "???"},
                                    {"Nang nakarating na kami sa event, iilan lang sa mga pinsan ko ang nakisosyo at yung\nmga nakipagusap lang ay yong nakatira na talaga dito habang yung iba kong pinsan\nna dumayo rin galing sa ibang lugar ay nakaupo at busy sa mga phone nila.", "???"},
                                    {"'Ano ba kayo dyan, puro selpon na lang inaatupag nyo!' Inis na sabi ni Lola.", "???"},
                                    {"Nasa party tayo tas nakayuko lang kayo dyan!", "Lola"},
                                    {"[Name], Yasmin, maki-sosyo kayo. Sumama kayo kay [Pinsan 1]. Maraming kakilala\nyan na kasing edad nyo!", "Lola"},
                                    {"Opo La.", "???"},
                                    {"Napatayo at napalapit na lang kami sa isa namin pinsan kesa suwayin pa si Lola.\nNang napansin na lang kami ng pinsan namin na palapit sa kanya, natawa na lang sya. ", "???"},
                                    {"'Pagpasensyahan nyo na si Lola,' patawa nyang sabi.", "Yasmin"},
                                    {"Ganyan talaga sya, bossy.", "Maki"},
                                    {"'Mga kaibigan ko nga pala.' Habang tinuturo at pinapakilala nya ang mga kaibigan.", "Yasmin"},
                                    {"Ngumiti na lang ako at kumaway.", "???"},
                                    {"Nagtuloy-tuloy lang ang usapan nila at mukhang nage-enjoy si Yasmin at nakikisama\nsya sa mga talakayan nila.", "???"},
                                    {"May upuan dun na tago medyo malapit sa buffet area, di ka gano mapapansin ni Lola.\nBanggit nya sakin.", "Yasmin"},
                                    {"Sige, dun muna ko Insan.", "???"},
                                    {"Sabi ko sa kanya at tinapik nya lang balikat bago siya nagfocus ulit sa pinag uusapan\nnila. ", "???"},
                                    {"Umaalis na ko at balak ko sanang dumiretso na dun sa upuan para maglaro pero\nnaramdaman ko ang gutom kaya dinaanan ko muna ang buffet area at kumuha ng\npaper plate and utensils. sa lang yung nagmamano sa table at busy sya na kuhaan\nyung mga bata na nakisama kaya ako na lang nagserve nang para sakin.", "???"},
                                    {"Tinitignan ko ng maigi yung pagkain, hindi ako pamilyar sa ibang putahe at di ko rin\nmatanong si kuyang nagmamano.", "???"},
                                    {"[Name ng putahe] yan.", "???"},
                                    {"Nagulat ako bigla sa nagsalita sa likod ko. Muntikan na mahulog yung kanin na nasa\npaper plate ko", "???"},
                                    {"Lumingon ako at napahanga ako sa nakita ko. Mas kulot ang buhok nya ngayon at\nnaka-suot sya ng kulay grey na dress shirt at nakatupi ang mga mangas nya at dalawa\nang di nakabutones. Sinamahan nya ito ng black na pants at kino-complement nito\nang figure niya. Tumingala ako para makita  siya ng maayos. ", "???"},
                                    {"Ano yun? Tanong ko", "???"},
                                    {"[Explanation kung ano yung food.] Sabi niya.", "???"},
                                    {"'Salamat.'  Sabi ko habang nagbow sa kaniya.", "???"},
                                    {"Mahina syang tumawa at parang natutuwa sya sa ginawa ko. ", "???"}, 
                                    {"Kanino kang Apo?", "Stranger"},
                                    {"Kay Lola Irina.", "???"},
                                    {"Ah.. ikaw yung apo nya na galing maynila.", "Stranger"},
                                    {"Oo, nabangit sakin ng pinsan ko na sa maynila ka rin naga-aral, tama ba?", "???"}, 
                                    {"Parang nagulat sya ng konti sa sinabi ko pero mabilis nyang inayos ang ekspresyon\nnya.", "???"},
                                    {"'Oo, Maca nga pala.' Sabi niya sabay nilahad nya yung kamay niya.", "Maca"},
                                    {"'[Name]' Sabi ko at nakipagkamay sa kanya. ", "???"},
                                    {"Sa buong gabing yun, sya lang yung kinausap ko. Isa yun sa mga masayang nangyari\nnung araw na yun. Di ko na sya nakita pa pagkatapos nung gabi yan. ", "???"}, //END OF FLASHBACK
                                    {"Pinaupo nya muna ako bago ipinakilala sila isa-isa.", "???"},
                                    {"Ako si Juan at eto ang anak ko at inaanak ko.", "Juan"}, //juan
                                    {"'Si Mary Grace at si Maca.' Turo nya sa dalawa nyang anak. Napatitig ako kay Maca.\nMay mga nabago sa itsura sa huli kong kita sa kanya. Wala rin syang philtrum at may\ntattoo sya sa may noo. Hindi ko mabasa ang mukha niya. ", "???"},
                                    {"Nagkati-tigan lang kami at para bang nawalan ng ingay sa parte namin at ang bigat ng\npakiramdam namin.", "???"},
                                    {"Ang nagdala sakin ang bumasag ng katahimikan.", "???"},
                                    {"'Maca, kilala mo ba sya?' Sabi niya at halata sa boses nya ang hinala at alala.\nNakatingin rin saming dalawa yung anak nya.", "???"},
                                    {"Oo, usap lang kami ng mabilisan To’ Sabi niya at bigla nya akong hinatak hanggang\nmakalabas kami sa market at nasa mas bakanteng lugar. Tinignan nya muna kung\nmay malapit na engkanto bago sya nagsalita.", "???"}, //TRANSITION TO ANOTHER BACKGROUND IMAGE
                                    {"'May Kinain ka na ba?'", "Maca"},
                                    {"Wala pa.", "???"},
                                    {"May nagbigay na ba sayo ng gift o nagkumbinsi na sumama ka sa kanila?", "Maca"},
                                    {"Wala at yung tito mo pa lang sinundan ko.", "???"},
                                    {"Napabuntong sya ng hininga sa sinabi ko pero napahaplos sya ng noo nya.", "???"},
                                    {"Di ko alam kung matatakot ako para sa iyo o magpapasalamat na yung tito ko pa lang.", "Maca"},
                                    {"Nalito ako sa sinabi niya. Ano ba kadalasan ang nangyayari? Ano pa ba ang pedeng\nmangyari na masama?", "???"},
                                    {"May mga nagba-bala ba sayo nung nakarating ka dito na umalis ka agad?", "Maca"},
                                    {"Uh..", "???"},
                                    {"Di ko alam kung ano sasabihin ko pero mukhang sapat na ang paga-alangan ko para\nmalaman niya ang totoo. ", "???"},
                                    {"Umupo sya sa isa sa mga bangko at sinenyasan nya kong tumabi sa kanya. Sumunod\nako at sa tingin ko may malalaman ako na makakatulong sakin na umalis.", "???"},
                                    {"Hindi lang ikaw ang tao dito, may pistang bukas, parang valentines day sa mundo nyo.", "Maca"},
                                    {"Sa bisperas ng pista, may selebrasyon na nangyayari. Pinapakita ang siyudad sa mga\npiling tao. Ang mga taong iyan ay naibigan ng mga engkanto na nakatira sa Biringan.", "Maca"},
                                    {"Aakitin ka nila gamit ang siyudad, ipapakita ang karangyaan, ang kaginhawaan pag\nnanatili ka dito at susuyuin ka para mag-stay at iwan ang dati mong buhay para sa\nkanila. ", "Maca"},
                                    {"Gagamit sila ng matatamis na salita, mga ginto, dyamante, at kung anong materyal\nna bagay pa,", "Maca"},
                                    {"Nabanggit yan sa mito.", "???"},
                                    {"Nabanggit rin ba nila kung bakit nila ginagawa ito?", "Maca"},
                                    {"Dahil mahal ka nila?”", "???"},
                                    {"Isa yan sa mga rason. Ang isa ay ang pagliit ng populasyon ng mga Engkanto. Sa\ndating daang libo, kumonti ng kumonti hanggang kakaramput na lang ang natira.\nAng mga natira ay nagpundar ng siyudad. Nagsama-sama sila para maligtas pa nila\nang natitira.", "Maca"},
                                    {"Bakit? Di ba immortal kayo?", "???"},
                                    {"Immortal lang kami hanggang may kagubatan, hanggang may kalikasan. Ang mga\nengkanto na nawalan na ng tinitirahan, kailangan maghanap agad ng bagong teritoryo\nbago sila matuyo at maging buhangin.", "Maca"},
                                    {"Para maparami ulit, kahit na ang laki na ang pinsala na ginawa nila, sa tao umasa\nang engkanto dahil hindi kaya makagawa ng engkanto ng anak sa isa pang engkanto.", "Maca"},
                                    {"Bakit tao?", "???"},
                                    {"Hindi maiipagkaila na dati pa may natural na atraksyon ang engkanto sa tao at mas\nnapatibay ito dahil dyan.", "Maca"},
                                    {"Paano yung sinasabi na kinukuha niyo ang kaluluwa?", "???"},
                                    {"Maraming engkanto ang nabiktima noon ng pag-ibig dahil ginamit sila ng tao at iniwan\nHindi kaya magmove-on ng isang engkanto, isa at isa lang ang mamahalin nya kahit\nna ano pang pagtataksil ay tatangapin namin.", "Maca"},
                                    {"Halata naman na may precaution di ba?", "Maca"},
                                    {"Oo", "???"},
                                    {"Mali bang protektahan namin ang sarili namin? Mali bang mag-mahal habang\nsinisiguradong hindi kami magagamit?", "Maca"},
                                    {"Hindi pero..", "???"},
                                    {"Maikli lang ang buhay ng tao kaya ginagawa silang engkanto para magkasama\nsila ng habang buhay. Ang paraan para maging engkanto ay ang pagkuha ng kaluluwa\nnila at i-bind silang dalawa.", "Maca"},
                                    {"Tumingin sya sakin at parang nabasa nya ang iniisip ko.", "???"},
                                    {"Wala namang masasaktan sa proseso, babaguhin lang ang anyo, maayos parin ang\nkaluluwa nyo.", "Maca"},
                                    {"Pero hindi parin ako nakukumbinsi.", "???"},
                                    {"Ang pagmamahal ng isang Engkanto ay hindi mo kayang sukatin ang lalim. Hindi\nnila sasaktan ang mga minamahal nila", "Maca"},
                                    {"Physically, maybe.", "???"},
                                    {"Hindi ka ba flattered? May isang engkanto ang nagkagusto sayo. Mahal na mahal ka\nto the point na gusto ka nyang maging kayo habang buhay. Wala kang mahahanap na\nganyan sa inyo.", "Maca"},
                                    {"Ayoko, hindi ko sya kilala at ayokong magasawa na isang araw ko pa lang nakilala.", "???"},
                                    {"'Bakit di mo muna kilalanin? May time ka pa, as long as makauwi ka bago maghating\ngabi, pede mo sila kausapin at kilalanin baka magustuhan mo pa sila.", "Maca"},
                                    {"At pag hindi?", "???"},
                                    {"Ako mismo ang maghahatid sayo sa bahay nyo at sisiguraduhing hindi mo sya\nmakikita.", "Maca"},
                                    {"Ano sa tingin mo? Sinabi niya habang nilahad nya yung palad niya.", "???"}
                                };
                                dialogueBox.startDialogue(choice1Dialogue);
                                showDialogue = true; // Fix works now
                            }},
                            {"Tumitig", [&]() {
                                std::vector<DialogueEntry> choice2Dialogue = {
                                    {"Hindi ko alam kung ano dapat gawin ko at matagal kong tinitigan ang nilahad niyang\nkamay.", "???"},
                                    {"Dumilim ang kanyang mukha nang tumagal pa ng nakatingin lang ako sa kamay niya.\nBinaba niya ang kanyang kamay,", "???"},
                                    {"Wala ka ring modo, noh?", "Stranger"},
                                    {"Muli kong naramdaman ang mga titig nila,now with more than ever. Nanlalamig na\nang katawan ko at tumataas na ang balahibo ko.", "???"},
                                    {"Pakonti-konti kong tinaas ang aking mga mata at para kong naubusan ng dugo.\nNakakapangilabot ang kanyang mga mata. Mas malala pa ang kaniyang titig kesa sa \nnaunang engkanto.", "???"},
                                    {"Wala ka bang pinagaralan ha!? Di ka ba tinuruan ng mga magulang mo nang\nmagandang asal!?", "Stranger"},
                                    {"Sorry po, hindi ko po--", "???"},
                                    {"Napakataas ba ng tingin mong tao ka na hindi ka marunong gumalang sa isang\nengkanto pag nasa lugar ka nila!? Eh Kung isumpa kaya kita dito para matuto ka ng\nleksyon ha!? Pota, [indecipherable words]--", "Stranger"},
                                    {"Bago pa niya tapusin ang salita niya, may parang alon ng asul na nakinang ang\nhumawak sa kamay niya at naghugis tao.", "???"},
                                    {"Napa-atras ako ng mabilis.", "???"},
                                    {"Tito, [indecipherable words].", "Stranger 2"},
                                    {"Mas umayos ang anyo niya at mas Lalo ako natakot.", "???"},
                                    //SHOW MACA
                                    //Flashback
                                    {"Tumakbo ako agad. Paalis sa lugar na toh. Paalis sa walang hiyang mundo na hindi\nko naman ginustong mapunta.", "???"},
                                    {"Matagal akong tumakbo hanggang sa mapagod. Hindi ko alam kung saan o anong\ndireksyon ang pinuntahan ko. Napaluhod na lang ako sa simento dahil sa pagod.", "???"},
                                    {"Okay ka lang ba?", "Maca"},
                                    {"Napahiyaw ako at napatingin sa likod habang umaatras.", "???"},
                                    {"Nandun sya, naka-squat sa tabi ko at may mukha na nag-aalala.", "???"},
                                    {"Sorry sa tito ko. Ganyan talaga ang attitude nila kadalasan, pabago-bago.", "Maca"},
                                    {"Ganun ka rin ba?", "???"},
                                    {"Tanong ko na halatang kabado. ", "???"},
                                    {"Maasahan mo ako [Name]. Iba ako sakanila,", "Maca"},
                                    {"Iba? Sa tingin mo maniniwala ako sayo?", "???"},
                                    {"Oo.", "Maca"},
                                    {"Tumayo siya at umupo sa isa sa mga banko. at sinenyasan nya kong tumabi sa kanya.\nHindi ako umalis sa inuupuan ko.", "???"},
                                    {"Hindi lang ikaw ang tao dito, may pistang bukas, parang valentines day sa mundo nyo,", "Maca"},
                                    {"Sa bisperas ng pista, may selebrasyon na nangyayari. Pinapakita ang siyudad sa mga\npiling tao. Ang mga taong iyan ay naibigan ng mga engkanto na nakatira sa Biringan.\nAakitin ka nila gamit ang siyudad, ipapakita ang karangyaan, ang kaginhawaan pag\nnanatili ka dito at susuyuin ka para mag-stay at iwan ang dati mong buhay para sa\nkanila.", "Maca"},
                                    {"Gagamit sila ng matatamis na salita, mga ginto, dyamante, at kung anong materyal\nna bagay pa.", "Maca"},
                                    {"Nabanggit yan sa mito.", "???"},
                                    {"Nabanggit rin ba nila kung bakit nila ginagawa ito?", "Maca"},
                                    {"Dahil mahal ka nila?", "???"},
                                    {"Isa yan sa mga rason. Ang isa ay ang pagliit ng populasyon ng mga Engkanto. Sa\ndating daang libo, kumonti ng kumonti hanggang kakaramput na lang ang natira.\nAng mga natira ay nagpundar ng siyudad. Nagsama-sama sila para maligtas pa nila\nang natitira.", "Maca"},
                                    {"Bakit? Di ba immortal kayo?", "???"},
                                    {"Immortal lang kami hanggang may kagubatan, hanggang may kalikasan. Ang mga\nengkanto na nawalan na ng tinitirahan, kailangan maghanap agad ng bagong teritoryo\nbago sila matuyo at maging buhangin.", "Maca"},
                                    {"Muli siyang sumenyas na umupo ako. Gusto ko pa malaman ang mga sasabihin\nniya kaya umupo ako.", "???"},
                                    {"Para maparami ulit, kahit na ang laki na ang pinsala na ginawa nila, sa tao umasa\nang engkanto dahil hindi kaya makagawa ng engkanto ng anak sa isa pang engkanto.", "Maca"},
                                    {"Bakit tao?", "???"},
                                    {"Hindi maiipagkaila na dati pa may natural na atraksyon ang engkanto sa tao at mas\nnapatibay ito dahil dyan.", "???"},
                                    {"Paano yung sinasabi na kinukuha niyo ang kaluluwa?", "???"},
                                    {"Maraming engkanto ang nabiktima noon ng pag-ibig dahil ginamit sila ng tao at iniwan\nHindi kaya magmove-on ng isang engkanto, isa at isa lang ang mamahalin nya kahit\nna ano pang pagtataksil ay tatangapin namin.", "???"},
                                    {"Halata naman na may precaution di ba?", "???"},
                                    {"Oo", "???"},
                                    {"Mali bang protektahan namin ang sarili namin? Mali bang mag-mahal habang\nsinisiguradong hindi kami magagamit?", "???"},
                                    {"Hindi pero..", "???"},
                                    {"Maikli lang ang buhay ng tao kaya ginagawa silang engkanto para magkasama\nsila ng habang buhay. Ang paraan para maging engkanto ay ang pagkuha ng kaluluwa\nnila at i-bind silang dalawa.", "???"},
                                    {"Tumingin sya sakin at parang nabasa nya ang iniisip ko.", "???"},
                                    {"Wala namang masasaktan sa proseso, babaguhin lang ang anyo, maayos parin ang\nkaluluwa nyo.", "???"},
                                    {"Pero hindi parin ako nakukumbinsi.", "???"},
                                    {"Ang pagmamahal ng isang Engkanto ay hindi mo kayang sukatin ang lalim. Hindi\nnila sasaktan ang mga minamahal nila", "???"},
                                    {"Physically, maybe.", "???"},
                                    {"Hindi ka ba flattered? May isang engkanto ang nagkagusto sayo. Mahal na mahal ka\nto the point na gusto ka nyang maging kayo habang buhay. Wala kang mahahanap na\nganyan sa inyo.", "???"},
                                    {"Ayoko, hindi ko sya kilala at ayokong magasawa na isang araw ko pa lang nakilala.", "???"},
                                    {"'Bakit di mo muna kilalanin? May time ka pa, as long as makauwi ka bago maghating\ngabi, pede mo sila kausapin at kilalanin baka magustuhan mo pa sila.", "???"},
                                    {"At pag hindi?", "???"},
                                    {"Ako mismo ang maghahatid sayo sa bahay nyo at sisiguraduhing hindi mo sya\nmakikita.", "???"},
                                    {"Ano sa tingin mo? Sinabi niya habang nilahad nya yung palad niya.", "???"}

                                };
                                dialogueBox.startDialogue(choice2Dialogue);
                                showDialogue = true; // Fix works now
                            }}
                        };
                        choiceBox.startChoices(choices);
                    }

                    if (currentDialogue == "Ano sa tingin mo? Sinabi niya habang nilahad nya yung palad niya." && !choiceBox.isVisible()) {
                        std::vector<Choice> choices = {
                            {"Pumayag", [&]() {
                                std::vector<DialogueEntry> choice1Dialogue = {
                                    {"Kinuha ko ang palad nya at kinaway niya ito.", "???"},
                                    {"Sige, pero may mga katanungan ako.", "???"},
                                    {"I'm all ears.", "Maca"},
                                    {"Sino sya?", "???"},
                                    {"Mas gu-gustuhin nilang sila mismo ang magpakilala.", "???"},
                                    {"Bakit ako? Hindi naman ako (Gwapo/Maganda).", "???"},
                                    {"Hindi lang panlabas ang tinitignan nila, kundi pati ang panloob. Sa personality at\nactions sila nagkakagusto.", "Maca"},
                                    {"Tumayo sya and patted his back.", "???"},
                                    {"Magtanong ka habang papunta tayo sa kanila. Minsan ka lang makapunta dito, why\nnot sulitin at bigyan kita ng tour?", "Maca"},
                                    {"Malayo pa ba papunta sa kanila?", "???"},
                                    {"Malayo-layo. Tara na at magpaalam muna tayo.", "Maca"},
                                    {"Okay.", "???"},
                                    {"Bumalik kami sa loob ng market at nagpaalam sya sa tito’t pinsan niya.", "???"},
                                    {"Alam mo na gagawin, magingat kayo.” Paalala nya samin.", "???"},
                                    {"Bago kami umalis may binili muna syang mga kakanin. ", "???"},
                                    {"Hindi tayo pede pumunta dun ng walang dalang gift.", "Maca"},
                                    {"Habang hinihintay niya ang binili niya na i-pack, may mga pinakita syang mga bilog\nna ginto.", "???"},
                                    {"May tatlong klase ng pagbabayad dito, ang ginto, barter o gift.", "Maca"},
                                    {"Pamilyar naman sayo ang mga kwento ng mga engkanto na nagbibigay ng mga\nkapangyarihan o kagustuhan ng tao?", "Maca"},
                                    {"Ganun ang isa sa mga sistema dito, kung saan ang mga materyal na bagay ay\nwalang kadalasang saysay sa engkanto, mga pabor o regalo ang paraan, mas\nmahalagaang relasyon.", "Maca"},
                                    {"Paano nagagamit ang ginto dito?", "???"},
                                    {"Nagkaron ng pormal na ekonomiya nung rumami na ang tao na naging engkanto.\nGinamit ang ginto bilang salapi at gumawa sila ng paraan para mapagsama ang tatlo.", "Maca"},
                                    {"Dumating na ang nagti-tinda at ibinigay na ang binili namin.", "???"},
                                    {"Salamat po, Sabi niya habang nag-bow. Nag-bow rin ako.", "???"},
                                    {"Kailangan natin magpakita ng magandang asal, ayaw na ayaw namin ang walang\nmodo.", "Maca"},
                                    {"Tara na. Marami ka pang dapat makita.", "Maca"},
                                    {"Umalis na kami sa market at naglakad. Sinusundan ko lang sya habang ipinapaliwanag\nniya ang mga nakikita namin.", "???"},
                                    {"Nakita mo na ba yung mapa?", "Maca"},
                                    {"Oo", "???"},
                                    {"Masyadong malaki ang biringan at hindi mo kakayanin na lakarin ito, kaya sasakay tayo.", "Maca"},
                                    {"Alam mo yung nakikita sa palabas na spaceship?", "Maca"},
                                    {"Oo", "???"},
                                    {"May isang tao na mahilig sa ganyan at isa sa mga kanyang ginawa at ayun ang\nsasakyan natin.", "Maca"},
                                    {"Talaga?", "???"},
                                    {"Oo, isipin mong parang public transpo.", "Maca"},
                                    {"Bakit may public transpo? Di nyo ba kayang mag-teleport?", "???"},
                                    {"Napatawa sya sa sinabi ko.", "???"},
                                    {"Pag nagteleport lang tayo, di mo na makikita yung ibang mga lugar, okay lang ba\nsayo yun?", "???"},
                                    {"Hindi.", "???"},
                                    {"Mas lalo syang natawa sa ekspresyon na pinakita ko.", "???"},
                                    {"Kaya namin, pero minsan gusto namin ng iba at yung mga dating tao ay gusto parin\nnila manirahan na parang tao.", "Maca"},
                                    {"May mga iba rin na hindi agad nagiging engkanto kaya kailangan nila para\nmakapag-libot o makapamasyal.", "Maca"},
                                    {"Huminto kami sa isang malaking puting building.", "???"},  //Background here
                                    {"Eto ang hub at tawag namin sa transpo na toh ay Lunawan.", "Maca"},
                                    {"Dumiretso kami papasok at napakalawak sa loob. Para akong na transport sa future.\nFuturistic na futuristic sya. Puti at pabilog ang style niya. Mailaw at may malaking\nmetal sculpture sa gitna na napapalibutan ng mga escalator. Maraming mga\nengkanto ang nandito.", "???"},
                                    {"Tinuro nya sakin ang parang help desk nila. Holographic ang gamit nilang gadyet\nkatulad sa nakikita sa film.", "???"},
                                    {"Hindi lahat ng engkanto ay mahilig sa gantong set up na futuristic kaya may help\ndesk dito para tulungan sila.", "Maca"},
                                    {"Kaya ba marami kayong division?", "???"},
                                    {"Oo, kadalasan nakatira ang mga engkanto sa panahon na kasanayan nila kaya kung\nmapapansin mo, malaki ang sakop ng Sanmalan. Dyan nakatira ang mga engkantong\nnabuhay habang sakop ng Espanyol.", "Maca"},
                                    {"Lumapit kami sa isa sa hologram na nagpapakita ng mapa.", "???"},
                                    {"Ang nasa kaliwa ay Irraya, ang division nasan tayo ay Ybalon at ang pupuntahan natin\nay Madyas. Walang hub sa Madyas kaya baba tayo sa entrance nito. Mga nakatira dun\nay ayaw sa mga ganto.", "Maca"},
                                    {"Umakyat kami hanggang third floor gamit ang escalator. Pagkarating namin sa floor\nna yun, ay naghanap kami ng bakanteng sasakyan. May pinindot syang button katabi\nng pintuan at bumukas ito at nagulat ako sa sasakyan namin.", "???"},
                                    {"Ang anak ko nga pala HAHA.", "Maca"},  //MACA
                                    {"[Description ng space shuttle, Di ko alam tawag sa kanila]", "???"}, 
                                    {"Ang Ganda nya, pinagawa mo?", "???"},
                                    {"Oo, mas mapapaganda ka sa loob.", "Maca"},
                                    {"Pagpasok namin sa lunawan, ang bumungad agad ang deck. May dalawang control\nwheel,sa harap nito ay ang panel at salamin. May mga holographic display rin at may\ndouble doors sa likod. Pag pasok mo sa likod, may living room, kitchen, at bathroom.\nMay mga control rooms at rin pero di sakin pinakita at exit sas sa likod.", "???"},
                                    {"Hinayaan niya ko magikot-ikot habang pinapaandar yung sasakyan. ", "???"},
                                    {"Tinawag nya ako at pinaupo.", "???"},
                                    {"Wala pang kalahating oras, nandun na tayo.", "Maca"},
                                    {"Mabilis ang byahe at tama sya, wala pang kalahating oras ay nakababa na agad kami.\nPinark nya sa isang garahe sa hub at ginabay nya ako sa entrance nang division.", "???"},
                                    {"Hanggang dito lang ako.", "Maca"},
                                    {"Bakit?", "???"},
                                    {"Pag pumasok pa ko kasama mo, baka may makita kang di maganda.", "Maca"},
                                    {"Pero…", "???"},
                                    {"Tawagin mo lang ako.", "Maca"},
                                    {"Isang tawag mo lang, pupunta agad ako at ililigtas kita.", "Maca"},
                                    {"Sure?", "???"},
                                    {"Deretso kana kay toa na mag huwat na ang imong gihigugma\n(Dumiretso ka na, naghihintay na yung nagmamahal sayo)", "Maca"}
                                
                                };
                                dialogueBox.startDialogue(choice1Dialogue);
                                showDialogue = true; // Fix works now
                            }},
                            {"Bumalik", [&]() {
                                std::vector<DialogueEntry> choice2Dialogue = {
                                    {"Gusto ko umuwi.", "???"},
                                    {"'Then, uuwi tayo.' Sabi niya habang tumatayo sya. Tumingin-tingin muna sya bago\nlumapit sakin at may binulong.", "???"},
                                    {"Makinig ka sakin, bago tayo umalis dapat kalmado ka lang, wag ka gumawa na kahit\nanong expresyon, at wag kang lalayo sakin. Naiintindihan ba?", "Maca"},
                                    {"Oo", "???"},
                                    {"Okay lang ba sayo na hawakan kita?", "Maca"},
                                    {"Oo", "???"},
                                    {"Good then, humawak ka sa kamay ko at wag na wag na wag mong bitawan.", "Maca"},
                                    {"Nilahad nya ang kamay niya at kinuha ko yun at inangat niya ako sa pagupo. Mahigpit\nang hawak niya sakin.", "???"},
                                    {"Di tayo dadaan sa portal malapit sa may palenque, marami masyadong maghahadlang\nna makauwi ka.", "Maca"},
                                    {"Pupunta muna tayo sa bahay ko, may kailangan ako kunin para siguradong makakauwi\nka.", "Maca"},
                                    {"Ready ka na ba, magteleport?", "Maca"},
                                    {"Wait-", "???"},
                                    //wind sound
                                    //Blink animation
                                    {"Wow", "???"},
                                    {"Ang bilis ng pagteleport niya tapos di sya katulad na pinapakita sa mga movies.", "???"},
                                    {"Isang malaking mansyon ang nagpakita. Agad akong tumingin sa kanya,", "???"},
                                    {"Bahay mo ba talaga to?", "???"},
                                    {"Yes, ganda di ba?", "Maca"},
                                    {"Nanguna sya pagpasok", "???"},
                                    {"Tayo lang ang nandito, matagal na kong bumukod kila Inay kaya ligtas tayo.", "Maca"},
                                    {"Pumasok kami sa bahay niya at napakaganda ng loob. Pumunta kami sa satingin ko na\nliving room at pinaupo ako. ", "???"},
                                    {"Pede ka tumingin diyan kung gusto mo, may kukunin lang ako.", "Maca"},
                                    {"Umakyat siya sa taas. Naglakad lakad ako at tinignan ang mga gamit sa kwarto.\nMalaking tv, plush sofas, mamahaling mga dekorasyon, pero ang mas nakatutok\nako ay ang koleksyon ng mga libro niya. Iba-iba ang lenggwahe, may english,\nfilipino at ang sa tingin ko lenggwahe nila.", "???"},
                                    {"Chineck ko ang mga libro at mukhang normal lang na mga textbooks at nobela ang\nlenggwahe sa english at filipino. Ang sa lenggwahe nila ay mas luma ang mga pahina\nat mas mabigat.", "???"},
                                    {"Hindi ko mabasaang nakasulat pero kung iba-base ko sa mga larawan, may mga libro\ntungkol sa kasaysayan, pagkain at mga novels.", "???"},
                                    {"Gusto ko ialok ng maiinom at ilang oras na nandtito ka, pero alam ko hindi pede.", "Maca"},
                                    {"Kakababa niya lang at may bag na sya ngayon na dala.", "???"},
                                    {"Ano oras na ba?", "???"},
                                    {"[Real Time]", "Maca"},
                                    {"Ang bilis naman.", "???"},
                                    {"Ganun dito, hindi mo mapapansin ang oras kung gaano na ito katagal.", "Maca"},
                                    {"Maganda ba talaga manirahan dito?", "???"},
                                    {"Depende kung pano ka dinala.", "Maca"},
                                    {"Depende?", "???"},
                                    {"Napabuntong hininga sya.", "???"},
                                    {"Sasabihin ko sayo ng maayos kung bakit", "Maca"},
                                    {"May dalawang klase ng tao ang dinadala dito, ang katulong at ang minamahal. ", "Maca"},
                                    {"Kung katulong ka, you are bound for eternity to serve. Pede magbago ang trabaho\nmo pero hanggang katulong ka lang. Hindi ka magiging engkanto, pero gagawin\nkang immortal. May pera, makukuha mo lahat ng gusto mong materyal na bagay\npero you are going to be at their beck and call.", "Maca"},
                                    {"Bakit may katulong?", "???"},
                                    {"To do their bidding. To do their chores and all those they can't be bother to do.", "Maca"},
                                    {"Pano?", "???"},
                                    {"Either part as punishment o nagustuhan nila ang gawa mo at inakit ka para magsilbi\nsa kanila. ", "Maca"},
                                    {"Kung mahal ka ng engkanto, bihag ka. Hindi ka nila hahayaan na makawala sa\npaningin nila.", "Maca"},
                                    {"Nabanggit ko na sayo, na hindi nila kayang magmahal ng iba.", "Maca"},
                                    {"Hindi ba masyadong matindi?", "???"},
                                    {"Hindi katulad nyong mga tao na hindi natututo sa mga nakaraan, kami oo. Tandaan\nmo,marami samin ang buhay pa simula pa nang hindi pa kolonya ang pilipinas. Marami\nsa aming sinabihan at tinuruan para hindi maging katulad nila.", "Maca"},
                                    {"Bihag ka lang naman sa mga unang ilang taon na hindi ka pa ginagawang engkanto,\nang mga taon kung saan hindi pa napapamahal ang mga na-akit sa mga engkanto.\nKung saan may takot pa sila sa kanila at hindi alam kung anong gagawin sa kanila\nng mga engkanto pag masyadong ginalit nila. ", "Maca"},
                                    {"Nakakabaliw rin naman na isang tao lang kasama mo at di ka makaalis ng bahay at\nhindi mo pede kausapin ang mga tao sa dati mong buhay at ang ibang engkanto.\nMahirap rin ang makatakas, kasi walang tutulong sayo dito pag pumayag ka na. Wala\nnang balikan at nagdesisyon ka ng hindi ka pinipwersa. May marka ka na.", "Maca"},
                                    {"Kung katulad namin ang nagkagusto sayo, hindi ka nila pwe-pwersahin na um-oo.\nIlalabag namin ang batas kung gayon at pangit ang kakalabasan sa magkabilang panig.", "Maca"},
                                    {"Ngayon kasi, dinala pa lang, pede pa kita matulungan na makauwi. Walang rule para\nhindi tumulong sa tao na makauwi nang hindi nakikilala ang nagmamamahal sa kanya\npero discourage samin yon. " , "Maca"},
                                    {"Naiintindihan mo ba?", "Maca"},
                                    {"Oo", "???"},
                                    {"May katanungan ka?", "Maca"}
                                                      
                                };
                                dialogueBox.startDialogue(choice2Dialogue);
                                showDialogue = true; // Fix works now
                            }},
                            
                        };
                        choiceBox.startChoices(choices);
                    }

                    if (currentDialogue == "May katanungan ka?" && !choiceBox.isVisible()) {
                        std::vector<Choice> choices = {
                            {"Magtanong pa", [&]() {
                                std::vector<DialogueEntry> choice1Dialogue = {                 
                                    {"Marami.", "???"},
                                    {"Una, Anong marka?", "???"},
                                    {"May nilalagay kaming marka pagkatapos pumayag ng isang tao. Simbulo ito ng\nkasunduan na ginawa nila na hindi pwersahan o sinaktan ka. Ang makakapansin lang\nnito ang mga elemento at ang markang ito ay para siguraduhin na ligtas ka sa iba pa.", "Maca"},
                                    {"Bakit hindi pa sila ginagawang engkanto?", "???"},
                                    {"Hindi ko pa na explain sayo kung ano kadalasan nangyayari pagkatapos mo pumayag.\nPagkatapos mo pumayag na manirahan dito, magkatipan na kayo at maninirahan sa\nisang bubong. Sa susunod na mga araw, paghahandaan ang kasal nyo at ang mismong\nkasal ay magaganap ng tatlong araw. ", "Maca"},
                                    {"Maraming ritwal ang mangyayari sa araw na yan. Sa araw na yan, ib-bind nila ang\nkaluluwa mo sa kanila pero hindi ka gagawing engkanto agad pagkatapos nyan, tanda\nmo ba yung sinabi ko kanina? Kulang kami sa tao. Kaya magbago ng engkanto ng\nhugis at itsura para tulungan ang mga tao na mabuntis.", "Maca"},
                                    {"Nasa sa kanila kung ilan pero kadalasan, tatlo hanggang lima ang anak nila bago sila\ngawing engkanto. Pagkatapos nang kasal, dyan na nababasag ang illusion na binigay\nsa kanila ng mga engkanto. Nakikita nila ang consequence ng pinili nila at marami ang\nnatakot dun. ", "Maca"},
                                    {"Ang kinagandahan lang, hindi ka nila pisikal na sasaktan at hindi ka ip-pwersa na\nmakipagtalik pero hindi lahat ng engkanto kayang hindi ipakita ang pagmamahal nila\nat yung stress at emotion magta-take ng toll sa tao and aapekto sa mental state nila.", "Maca"},
                                    {"Ano ang mangyayri pag lumala?", "???"},
                                    {"Ano sa tingin mo? Mas hihigpit ang pagkabihag. May mga clinic kami dito na specialze\nsa therapy at counselling pero kadalasan mahika ang gamit para mas maging\nkalmado at kalmado sila. Bibigyan nang malaking dose hanggang tuloy tuloy na mas\nokay ang kanilang pakiramdam at pakonti-konti na lang ibibigay.", "Maca"},
                                    {"Sa panahong yan, mas susceptible silang mapaibig to the point na sakanila sila aasa. ", "Maca"},
                                    {"Hindi ba yan nalabag sa sina-sabi mo na hindi ipw-pwersa?", "???"},
                                    {"Hindi naman sila pwin-pwersa. Pag napa-ibig na nila dun sila maga-aya. Pagkatapos\nnapaibig at nagka-pamilya, dun na sila gagawing engkanto sa ritwal ng bagong\nKatauhan.", "Maca"},
                                    {"Ano ibig mong sabihin na katulad nyo?", "???"},
                                    {"Maraming klase ang engkanto pero hindi lahat ay nakatira dito. May Dalaketnon,\nMahamomay, Tamawo, Agnoy, Banwaanon, Lewenris at iba pa.", "Maca"},
                                    {"Ang kailangan mo lang malaman ay wag kang sasama sa mga itim na engkanto.  Sila\nay pinaka-malulupit at mararahas sa lahat ng engkanto. Umaastang mga diablo.\nMahahalata mo sila sa mata. Kulay itim ang kanilang mata at wala silang anino. ", "Maca"},
                                    {"Ano ka?", "???"},
                                    {"Dalaketnon ang kalahati sa engkanto dito. Wag kang mag alala, pro-protektahan kita.", "Maca"},
                                    {"May mga tanong ka pa?", "Maca"},
                                    {"Wala.", "???"}
                                                                        
                                };
                                dialogueBox.startDialogue(choice1Dialogue);
                                showDialogue = true; // Fix works now
                            }},
                            {"Wala na", [&]() {
                                std::vector<DialogueEntry> choice2Dialogue = {
                                    {"Wala", "???"},
                                    {"Kung wala ka nang tanong, suotin mo tong gahum na toh. Pang-proteksyon yan.", "Maca"},
                                    {"May sinuot sya sa akin na parang agimat.", "???"},
                                    {"Wag na wag mong tatangalin yan. Aalis na tayo.", "Maca"},
                                    {"Ginabay nya ako papunta sa isa sa kanilang mga garage. Pagkapunta namin ay\nlumabas ang dose-dosenang mga sports car.", "???"},
                                    {"Tinuro niya ang isa sa mga sasakyan at ayun daw ang gagamitin namin.", "???"},
                                    {"Sumakay na kami at bago nya paandarin ang kotse ay binigyan niya ko ng takip sa\nmataat headset.", "???"},
                                    {"Suotin mo yan at wag mong tatangalin kahit anong marinig mo. Pag makalabas na\ntayo, Ako mismo magtatangal nyan. Humawak ka lang sa braso ko.", "Maca"},
                                    {"Ginawa ko ang sinabi niya at nagsimula na syang magmaneho. Mabilis ang kanyang\npatakbo.", "???"},
                                    {"Ang mga unang minuto ay peaceful ang drive pero hindi nagtagal ay nagiba ang\nnangyayari sa byahe.", "???"},
                                    {"Naririnig ko ang lakas ng pagkatok sa bintana, ang mga sigaw at ang tunog ng\nsasakyan.", "???"},
                                    //10 to 20 second sound effects of tunog ng mga sasakyan
                                    {"Wag mo sila pansinin, pauwi ka na.", "Maca"},
                                    {"Paulit ulit nyang sinasabi at minsan ay hinihimas niya ang kamay ko para sabihin na\nnandito lang sya. Nagpagaan ito ng konti.", "???"},
                                    {"Mas lumakas nang lumakas ang mga sigaw hanggang may marinig ako na basag nang\nsalamin. Napa-lingon ako pero sumigaw sya.", "???"},
                                    {"Wag kang lumingon! YUKO!", "Maca"},
                                    {"Ginawa ko ang sinabi niya pero iba na ang nangyayari. Halatang may nakapasok sa\nsasakyan na galit na galit sa ginagawa namin", "???"},
                                    {"[indecipherable words]\n[indecipherable words]\n[indecipherable words]\n[indecipherable words]", "Stranger"},
                                    {"Hindi ko kailangan sumunod kung alam kong hindi siya sasaya sa piling MO!", "Maca"},
                                    {"Pagkasabi yun ni Maca, parang sumabog sa galit yung kausap nya. Anlakas ng\nkaniyang sigaw at nakikita ko ang hinanakit na kaniyang dala-dala.", "???"},
                                    {"HINDI SIYA SASAYA!? AKO LANG ANG KAYA MAGBIGAY NG PAGMAMAHAL NA\nKARAPAT-DAPAT PARA SA KANIYA! AKOO LANG ANG NASA TABI NIYA NANG\nKAILANGAN NIYA NANG KASAMA AT NANDUN KA BA!? WALA!", "Stranger"},
                                    {"WAALAAA! WALA ANG SINASABI MONG KAIBIGAN O PAMILYA! NI-ISA SA INYO AY\nWALA NANG KAILANGAN NIYA! HINDI NIYO NAKITA ANG SAKITA NA DINARANAS\nNIYA. AKO LANG ANG NAGPR - PROTEKTA AT NAGMA - MAHAL SA KANIYA!", "Stranger"},
                                    //Habang nagsasalita sya, kailangan nagalaw ang tingin ang perspective kasi naalog yung sasakyan at rinig yung iyak at sigaw pati na rin yung pagmamaneho. 
                                    {"[Name]...", "Stranger"},
                                    {"[Nickname]..", "Stranger"},
                                    {"Aking iniirog… Ayaw mo na ba sakin…? Minamahal ko… ayaw mo na ba ako makita?", "Stranger"},
                                    {"Wag mo siya pansinin [Name]!", "Maca"},
                                    {"Pero..", "???"},
                                    {"[Nickname], si Aele toh. Di mo na ba ako maalala? Hindi mo na ba maalala ang\nating pagmamahalan?", "Aele"},
                                    {"Aele? Aele… Ikaw ba talaga yan?", "???"},
                                    {"Buhay ka pa?", "???"},
                                    //FLASHBACK
                                    {"Mahal?", "???"},
                                    {"Bilis bilis kong tinangal ang takip at lumingon sa likod.", "???"},
                                    {"Nandun sya si Aele pero iba na sya.", "???"},
                                    {"Aele..?", "???"},
                                    {"[Name]!", "Aele"},
                                    {"Iba na ang kanyang mukha at pananamit. Gusto ko sya hawakan. Gusto ko\nmakita kung siya nga ba talaga yan. Bago ko pa mahaplos ang mukha niya,\nkinuha ni maca ang kamay ko at pwinersa akong paharapin.", "Aele"},
                                    {"[Name]! Matagal nang patay si Aele!", "Maca"},
                                    {"At anong alam mo!? Di mo siya kilala! Hindi mo siya nakita masaksak at makita ang\nkatawan niya na iburol!", "???"},
                                    {"Alam ko na yang Banwaanon na yan ang rason bakit siya namatay!", "Maca"},
                                    {"Wag ka makinig sa kanya mahal, buhay na buhay ako dito.", "Aele"},
                                    {"Hindi ba kahina-hinala na yung unang mga pagkakilala mo sa kanya nang ilang\nbeses ay wala syang pake sayo tas umuwi lang sya nang probinsya ay grabe na ang\npaghahayag niya nang pag-ibig sayo at nanligaw!?", "Maca"},
                                    {"Hindi ka ba naghinala bakit mo nagustuhan agad sya at naging kayo nang\ntatlong buwan?", "Maca"},
                                    {"Sabi niya hindi niya lang naintidihan agad kung ano feelings niya until mawalay kami.", "???"},
                                    {"Bullshit!", "Maca"},
                                    {"Pinatay sya nang Banlawaan dahil sa selos at kinuha ang kaniyang mukha at.\nGusto mo ba ituro ko pa kung nasaan yung bangkay niya!?", "Maca"},
                                    {"Dalaketnon", "???"},
                                    {"Pinatay niya lang yung illusion na pinakita niya sayo para maghanda siya sa pagdaos\npagnakapunta ka na dito.", "Maca"},
                                    {"Minapula niya ang pamilya mo para pumunta sila dito.", "Maca"},
                                    {"At ikaw alam mo ha? Parehas lang tayo, dalakitnon.", "Maca"},
                                    {"Iba ako, hindi katulad mo!", "???"},
                                    {"Parehas lang tayo nagmamahal!", "Maca"},
                                    {"Wtf? Natameme ako sa sinabi niya. Huh? Parehas?", "???"},
                                    {"Malapit na tayo [Name], makinig ka sakin please lang..", "Maca"},
                                    {"Napatingin ako sa harap. Nakikita ko ang portal palabas. ", "???"},
                                    {"[Nickname], mahal ko.. Sumama ka sakin… Kaya ko tuparin ang mga gusto mo.\nKahit ano pa hingilin mo. Sumama ka lang, mamahalin kita ng lubos,\nhindi mo ba naalala ang pangako natin na magmamahalan nang habang buhay?", "Aele"},
                                    {"Ano ba gagawin ko? ", "???"},
                                    {"Sorry Aele…", "???"},
                                    {"Kita ang sakit at galit sa mukha niya. May binubulong syang parang enchantment.", "???"},
                                    {"Makinig ka sakin. Ilagay mo ulit yang takip sa mata mo at sabayan mo ko.", "Maca"},
                                    {"Tinatangihan ko kung ano man ang gusto mo ibigay sa akin at tinatangihan ko ang\niyong pagmamahal at ayokong sumama sayo,", "[Indecipherable name]"},
                                    {"Tinatangihan ko kung ano man ang gusto mo ibigay sa akin at tinatangihan ko ang\niyong pagmamahal at ayokong sumama sayo,", "[Indecipherable name]"},
                                    {"Ginawa ko lahat ng sinabi niya at mas lumakas ang pag-galaw ng kotse. ", "???"},
                                    {"[Indecipherable word]", "Aele"},
                                    {"Paalam muna ngayon mahal.", "Aele"},
                                    {"Parang nabawasan ang bigat ng nararamdaman ko. Nawala na ang mga hiyaw at\nang mga katok.", "???"},
                                    {"Wag mo munang tanggalin. Palabas na tayo.", "Maca"},
                                    ////Sound na papasok at palabas sa portal. 25 secs
                                    {"Nakalabas na tayo, pede mo na tanggalin.", "Maca"},
                                    {"Tinanggal ko na ang takip at nakita ko na nasa tapat kami ng municipal.", "???"},
                                    {"Tangina!", "???"},
                                    {"Tangina talaga, papatayin niya ko.", "???"},
                                    {"Sorry", "???"},
                                    {"Hayaan mo na, pagkauwi mo maligo ka ng malamig na tubig na may asin at ikwento\nmo kay lola para madala ka sa albularyo bukas na bukas", "Maca"},
                                    {"Oo", "???"},
                                    {"Alam ba nila na?", "???"},
                                    {"Alam ni lola at iba pa na matatanda. Wala silang pake at nakita nila ko na lumaki.", "Maca"},
                                    {"Sinimulan niya na pagalawin ang kotse.", "???"},
                                    {"Kailangan ko toh irepare paguwi. Masyado malaki ang nasira.", "Maca"},
                                    {"Tinignan ko ang likod at nakita ko pagkabrabe ng sira sa likod.", "???"},
                                    {"Sorry, may pede ba akong–", "???"},
                                    {"Wag na, kaya ko. Kailangan mo na umuwi sa bahay niyo. Alam kong may katanungan\nka pa pero kailangan mo ng tulog. Bibisitahin kita after para ma sagot ko ang tanong mo.", "Maca"},
                                    {"Thank you Maca.", "???"},
                                    //PICTURE OF MACA
                                    {"Walang anuman.”", "Maca"}
                                    //TITLE SCREEN NAKALAGAY GOOD END NA NAKAUWI
                                
                                 
                                };
                                dialogueBox.startDialogue(choice2Dialogue);
                                showDialogue = true; // Fix works now
                            }}
                        };
                        choiceBox.startChoices(choices);
                    }



                    if (!dialogueBox.isVisible() && !choiceBox.isVisible()) {
                        showDialogue = false;
                    }

                    // Save progress
                    GameProgress currentProgress;
                    currentProgress.backgroundImage = bgManager.getCurrentBackground();
                    currentProgress.speaker = dialogueBox.getCurrentSpeaker();
                    currentProgress.dialogue = dialogueBox.getCurrentDialogue();
                    SaveManager::saveProgress("savegame.dat", currentProgress);
                }
                else {
                    sf::Text gameplayText;
                    gameplayText.setFont(bodyFont);
                    gameplayText.setString("Gameplay is running...\nPress ESC to return.");
                    gameplayText.setCharacterSize(30);
                    gameplayText.setFillColor(sf::Color::White);
                    gameplayText.setPosition(50, 200);
                    window.draw(gameplayText);
                }

                window.display();

                if (state == TitleScreen::GameState::TITLE)
                    break;
            }

            soundManager.stopMusic();
        }
        else if (state == TitleScreen::GameState::QUIT) {
            std::cout << "Quitting the game...\n";
            window.close();
        }
    }

    return 0;
}
