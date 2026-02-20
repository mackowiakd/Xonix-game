#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <regex>
#include <ranges>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <concepts>






//testy zmian + popr wysw cover area podczas gry
using namespace sf;
namespace fs = std::filesystem;

const int M = 30;
const int N = 45;




struct score {
    std::string name;
    int area;
    int enem_count;
    std::string datetime;
};



class Grid
{
private:
    int grid[M][N] = { 0 };
    int tileSize;
    Texture tileTexture;
    Sprite tileSprite;
    int capturedTiles = 0;

public:

    Grid(int ts) :tileSize(ts) {

        tileTexture.loadFromFile("images/tiles.png");
        tileSprite.setTexture(tileTexture);
    }
    int get_ts() {
        return tileSize;
    };

    void initialize();
    void drop(int y, int x);
    void fillEnclosedArea();
    void draw(RenderWindow& window1);

    void setTile(int x, int y, int value) { 
        grid[y][x] = value; }

    int getTile(int x, int y) {
        return grid[y][x]; }

    int get_capturedT() {
        return capturedTiles;
    }
};




class Entity {
protected:
    int x, y, dx, dy;
    Sprite EntitySprite;
    Texture EntityTexture;
    Grid* g = nullptr;

public:
    Entity(Grid* ptr) : g(ptr) {
        x = y = dx = dy = 0;
    };
    virtual void draw(RenderWindow& window1) {};

    int get_x() {
        return x;
    }
    int get_y() {
        return y;
    }
    void set_x(int x1) {
        x = x1;
    }
    void set_y(int y1) {
        y = y1;
    }
    void set_dy(int d) {
        dy = d;
    }
    void set_dx(int d) {
        dx = d;
    }
};


class Player : public Entity
{
public:
   
    float timer, delay;
    std::string name;

    Player(Grid* g) : Entity(g)
    {
        timer = 0;
        delay = 0.07;
        EntityTexture.loadFromFile("images/tiles.png");
        EntitySprite.setTexture(EntityTexture);
        EntitySprite.setTextureRect(IntRect(36, 0, g->get_ts(), g->get_ts()));
    }

    void handleInput();
    bool move(float elapsedTime);

    void  draw(RenderWindow& window1) override
    {
        EntitySprite.setPosition(x * g->get_ts(), y * g->get_ts());
        window1.draw(EntitySprite);
    }

    void set_name(std::string n) {
        name = n;
    }

    std::string get_name() {
        return name;
    }
};





class Enemy :public Entity
{
public:


    Enemy(Texture& enemyTexture, Grid* g) : Entity(g)
    {
        x = y = 300;
        dx = 4 - rand() % 8;
        dy = 4 - rand() % 8;
        EntitySprite.setTexture(enemyTexture);
        EntitySprite.setOrigin(20, 20);
    }

    void move();
    void draw(RenderWindow& window1) override;
};





class display {
    
    sf::Font font;
    Text text1;
    Text text2;
    Text text3;
    Text infoText;

    int char_sizeS = 15;
    int char_sizeB = 32;
    int X_pos_def = 275;
    int Y_pos_def = 70;

    unsigned int progress = 0;

public:
    display() {

        if (!font.loadFromFile("fonts/PressStart2P.ttf")) {
            std::cerr << "Nie można załadować czcionki!\n";
        }
    };

    template <typename T>
        requires std::ranges::range<T>&&
        requires(typename T::value_type s) {
            { s.name } -> std::convertible_to<std::string>;
            { s.enem_count } -> std::convertible_to<int>;
            { s.area } -> std::convertible_to<int>;
    }
    void print_scores(const T& scores, RenderWindow& window, std::string player_name) {
        sf::Event event;
        std::ostringstream temp;
        float y = 50; // Starting y position
        int x = 10;
        int i = 1;


        for (const auto& player : scores) {

            temp << std::to_string(i) + ". Name: " + player.name + "  enemies: " + std::to_string(player.enem_count) + " area: " + std::to_string(player.area) + '/' + std::to_string(N * M) + '\n';
            i++;
        }
        make_text(text1, temp.str(), char_sizeS, x, y);

        while (window.isOpen()) {

            while (window.pollEvent(event))
                if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
                    window.close();
            window.clear(sf::Color::Black);
            window.draw(text1);
            window.display();

        }

    };

    unsigned int get_progress() {
        return progress;
    };

    Text make_text(Text& text, std::string disp, int char_size, float x_pos, float y_pos); // for captions : WIN/ GAME_OVER / LOSE
    void print_text(std::string s, RenderWindow& window, std::vector< score>& v, std::string name, sf::Sound& end_sound);
    void make_user_name(RenderWindow& window);
    std::string display_name(RenderWindow& window);
    void draw_bar(RenderWindow& window, int tile_size);
    void update_covArea(RenderWindow& window, float progress1, float area);
};


class Sounds {

    sf::SoundBuffer buff_lose;
    sf::Sound lose;
    sf::SoundBuffer buff_end_lose;
    sf::Sound end_lose;
    sf::SoundBuffer buff_end_win;
    sf::Sound end_win;
    sf::SoundBuffer buff_win;
    sf::Sound win;

public:
    Sounds();

    void get_win() {
        win.play();
    }
   
    void get_lose() {
        lose.play();
    }
    sf::Sound& get_end_win() {
        return end_win;
    }

    sf::Sound& get_end_lose() {
        return end_lose;
    }

};

/*@FILE*/

class File {
    fs::path folder = "wyniki"; // katalog na wyniki
    fs::path filename = "gracz.txt";
    fs::path filepath = folder / filename;
    std::vector<score> scores;

public:
    std::string get_current_datetime();

    void open_file(std::string name, int cov_area, int area, int enem_count);
    void read_scores(std::vector<score>& scores);
    unsigned int find_user(std::vector<score>& scores, std::string player_name, unsigned int enem_count);
};


class Game_view {

    RenderWindow window1;
    Texture enemyTexture, gameOverTexture;
    Sprite gameOverSprite;
    display  D;
    File F;
    Sounds S;
    Grid* grid = nullptr;
    Player* player = nullptr;
    std::vector<Enemy> enemies;
    std::vector<score> scores;
    bool isRunning = true;
    unsigned int enemy_count = 4;
    float requiredCapture =0;

    
public:
    Game_view(int ts, int enem) ;

    void draw();

    std::vector<Enemy>& get_enem() {
        return enemies;
    }

    Player* get_player() {
        return player;
    }

    Game_view* get_ptr() {
        return this;
    }
    Grid* get_grid() {
        return grid;
    }
    RenderWindow* get_window() {
        return &window1;
    }

    bool get_isRun() {
        return isRunning;
    }

    void set_isRun(bool b) {
        isRunning = b;
    }

    ~Game_view() {
        if (player != nullptr)
            delete(player);
        if (grid != nullptr)
            delete(grid);
    }
};





class Game
{
private:

    Game_view* ptr = nullptr;
    Clock clock;


public:
    Game(Game_view* ptr) :ptr(ptr) {};


    void run();
    void handleEvents();
    void update(float elapsedTime);
    void reset();

    

};

