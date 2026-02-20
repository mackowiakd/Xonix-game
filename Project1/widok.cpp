#include "xonix.h"

std::istream& operator>>(std::istream& is, score& s) {
    return is >> s.name >> s.area >> s.enem_count >> s.datetime;
};



auto comp_scores = [](const score& a, const score& b) {

    if (a.enem_count == b.enem_count)
        return a.area > b.area;
    else {
        return a.enem_count > b.enem_count;
    }
};


std::string File:: get_current_datetime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    char buffer[100];
    std::tm timeinfo;  // local structure for safe time storage


    if (localtime_s(&timeinfo, &now_time_t) != 0) {
        return "Invalid time";
    }

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H:%M:%S", &timeinfo);
    return std::string(buffer);
};

void File::open_file (std::string name, int cov_area, int area, int enem_count) {

    if (!fs::exists(folder)) {
        fs::create_directory(folder);
    }
    std::ofstream file(filepath, std::ios::app); 
    if (file)
        file << name << " " <<cov_area<< " " << enem_count << " " << get_current_datetime() << "\n";
};


void File::read_scores(std::vector<score>& scores) {

    std::ifstream file(filepath); // lub std::ofstream file(filepath.string());

    std::ranges::copy(
        std::istream_iterator<score>{file},
        std::istream_iterator<score>{},
        std::back_inserter(scores)
    );
    // Sortuj: najpierw po name, potem po area malejąco
    std::sort(scores.begin(), scores.end(), [](const score& a, const score& b) {
        if (a.name != b.name)
            return a.name < b.name;           
        if (a.enem_count == b.enem_count)
            return a.area > b.area;
        else
            return a.enem_count > b.enem_count;
        });

    // usuń duplikaty graczy (pozostawiając najlepszy wynik)
    auto it = std::ranges::unique(scores, {}, &score::name).begin();
    scores.erase(it, scores.end());

    std::sort(scores.begin(), scores.end(), comp_scores);

    //zapisz nowe wartosci 
  
    std::ofstream file_out(filepath, std::ios::trunc);
    for (const auto& s : scores) {
        file_out << s.name << " " << s.area << " " << s.enem_count <<" " << s.datetime <<"\n";
    }

};

//sprawdza czy uztkownik moze wejsc na koleny poziom trudnosci (wiecej wrogow)
unsigned int File:: find_user(std::vector<score>& scores, std::string player_name, unsigned int enem_count) {
    auto it = std::ranges::find_if(scores, [&](const score& s) {
        return s.name == player_name;
        });

    if (it != scores.end()) {

        if (it->area*1.0 > 0.75*N*M)
            return ++it->enem_count;
        
    }
    
    return enem_count;

}


/*@DISPLAY*/
Text display:: make_text(Text& text, std::string disp, int char_size,float x_pos, float y_pos) {

    text.setFont(font);
    text.setString(disp);
    text.setCharacterSize(char_size); 
    text.setFillColor(Color::Blue);
    text.setPosition(x_pos, y_pos);
   
    return text;

}


void display::print_text(std::string s, RenderWindow& window, std::vector<score>&v, std::string name, sf::Sound& end_sound) {
    while (window.isOpen()) {
        sf::Event event;

        if (end_sound.getStatus() != sf::Sound::Playing) {
            end_sound.play();
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
                window.close();

            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num1)
                print_scores( v,window, name);
            
        }

        window.clear(sf::Color::Black);
        window.draw(make_text(text1,s, char_sizeB, window.getSize().x /3.5,Y_pos_def));
        window.draw(make_text(text2,"press 1 to see scores", char_sizeS, window.getSize().x / 4, Y_pos_def + 3 * char_sizeB));
        window.draw(make_text(text2, "press ENTER to exit", char_sizeS, window.getSize().x / 4, Y_pos_def + 4 * char_sizeB));

        window.display();
    }
}


void display:: make_user_name(RenderWindow& window) {

   
    
    make_text(text1, "Welcome to XONIX\n\n", char_sizeB, window.getSize().x/4.5, Y_pos_def);
    make_text(text2, "enter user name:", char_sizeS, X_pos_def, Y_pos_def + char_sizeB + char_sizeS);
    make_text(text3, "", char_sizeS, X_pos_def + 3 * char_sizeB, Y_pos_def + 2 * char_sizeB + char_sizeS);

};

std::string display::display_name(RenderWindow& window) {

 
    std::string input;
    std::regex valid("^[a-zA-Z0-9]{3,12}$");

    window.clear(sf::Color::Black);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Obsługa tekstu
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8 && !input.empty()) {
                    input.pop_back(); // Backspace
                }
                else if (event.text.unicode >= 32 && event.text.unicode < 127) {
                    char c = static_cast<char>(event.text.unicode);
                    if (isalnum(c) && input.size() < 12)
                        input += c;
                }
            }

            // Enter = zakończ wprowadzanie
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (std::regex_match(input, valid)) {

                    return input;
                }
                else {
                    input.clear(); // reset jeśli błędna
                }
            }
        }
        text3.setString(input);

        window.clear();

        window.draw(text1);
        window.draw(text2);
        window.draw(text3);

        window.display();

    }

};

void display::draw_bar(RenderWindow& window, int tile_size) {

    sf::RectangleShape topBar;
    topBar.setSize(sf::Vector2f(window.getSize().x, 2 * tile_size));  // szerokość okna, wysokość np. 40px
    topBar.setFillColor(sf::Color::White);
    topBar.setPosition(0, window.getSize().y- 2 * tile_size);
    

    make_text(infoText, "", tile_size, window.getSize().x/5, topBar.getPosition().y+tile_size);
    window.draw(topBar);

};

void display::update_covArea(RenderWindow& window, float progress1, float area) {

    std::stringstream ss;
    ss << "   PROGRESS: " << (progress1 / area) * 100 << "%";
    infoText.setString(ss.str());
    window.draw(infoText);
    progress = progress1;
};

Sounds::Sounds() {
    if (!buff_lose.loadFromFile("SFX/synth_misc_02.ogg")) {
        std::cerr << " sound  cannot be loaded\n";
    }
    lose.setBuffer(buff_lose);

    if (!buff_end_lose.loadFromFile("SFX/arcade_machine.ogg")) {
        std::cerr << " sound  cannot be loaded\n";
    }
    end_lose.setBuffer(buff_end_lose);

    if (!buff_end_win.loadFromFile("SFX/arcade_level_completed.ogg")) {
        std::cerr << " sound  cannot be loaded\n";
    }
    end_win.setBuffer(buff_end_win);
    if (!buff_win.loadFromFile("SFX/synth_beep_02.ogg")) {
        std::cerr << " sound 1 cannot be loaded\n";
    }
    win.setBuffer(buff_win);

}



