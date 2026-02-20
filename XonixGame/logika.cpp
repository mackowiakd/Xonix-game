#include "xonix.h"

// Initializing Boundaries
void Grid::initialize()
{
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;
};

// Filling Enclosed Areas
void Grid::drop(int y, int x)
{
    if (y < 0 || y >= M || x < 0 || x >= N) return;
    if (grid[y][x] != 0) return;
    grid[y][x] = -1;
    drop(y - 1, x);
    drop(y + 1, x);
    drop(y, x - 1);
    drop(y, x + 1);
};

void Grid::fillEnclosedArea()
{

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++)
        {
            if (grid[i][j] == -1)
                grid[i][j] = 0;
            else if (grid[i][j] == 0 || grid[i][j] == 2)
            {
                grid[i][j] = 1;
                capturedTiles++;
            }

        }
    }
};


/*PLAYER*/
void Grid::draw(RenderWindow& window1)
{
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
        {
            if (grid[i][j] == 0) continue;
            tileSprite.setTextureRect(IntRect((grid[i][j] == 1) ? 0 : 54, 0, tileSize, tileSize));
            tileSprite.setPosition(j * tileSize, i * tileSize);
            window1.draw(tileSprite);
        }
};


void Player::handleInput()
{
    if (Keyboard::isKeyPressed(Keyboard::Left)) { dx = -1; dy = 0; }
    if (Keyboard::isKeyPressed(Keyboard::Right)) { dx = 1; dy = 0; }
    if (Keyboard::isKeyPressed(Keyboard::Up)) { dx = 0; dy = -1; }
    if (Keyboard::isKeyPressed(Keyboard::Down)) { dx = 0; dy = 1; }
};

//Player Movement Updates
bool Player::move(float elapsedTime)
{
    timer += elapsedTime;
    if (timer > delay)
    {
        x += dx;
        y += dy;

        x = std::max(0, std::min(N - 1, x));
        y = std::max(0, std::min(M - 1, y));

        if (g->getTile(x, y) == 2) return false;
        if (g->getTile(x, y) == 0) g->setTile(x, y, 2);


        timer = 0;
    }
    return true;
};




/*ENEMY*/

void Enemy::move()
{
    x += dx;
    if (g->getTile(x / g->get_ts(), y / g->get_ts()) == 1) { dx = -dx; x += dx; }

    y += dy;
    if (g->getTile(x / g->get_ts(), y / g->get_ts()) == 1) { dy = -dy; y += dy; }
};

void  Enemy::draw(RenderWindow& window1) {
    EntitySprite.setPosition(x, y);
    window1.draw(EntitySprite);
    EntitySprite.rotate(10);
};




/*GAME_VIEW*/

Game_view:: Game_view(int ts, int enem) : window1(VideoMode(N* ts, M* ts + 2 * ts), "Xonix Game!"), D(), F(), S()
{
    requiredCapture = N * M*0.75; // 75% of the board

    grid = new Grid(ts);
    player = new Player(grid);

    D.make_user_name(window1);
    player->set_name(D.display_name(window1));
    F.read_scores(scores);
    enemy_count= F.find_user(scores, player->get_name(), enemy_count); 
    window1.clear(sf::Color::Black);


    window1.setFramerateLimit(60);
    enemyTexture.loadFromFile("images/enemy.png");
    

    for (int i = 0; i < enemy_count; i++)
        enemies.push_back(Enemy(enemyTexture, grid));

    grid->initialize();

};



void Game_view:: draw(){

    window1.clear(sf::Color::Black);

    grid->draw(window1);
    D.draw_bar(window1, grid->get_ts());
    if (grid->get_capturedT() > D.get_progress()) {
        S.get_win(); 
            if(grid->get_capturedT()>=requiredCapture){
                isRunning = false;
        }
    }
    D.update_covArea(window1, grid->get_capturedT(), N*M);
    player->draw(window1);


    for (auto& enemy : enemies)
        enemy.draw(window1);
   

    if (isRunning != true) {
       
        F.open_file(player->get_name(), grid->get_capturedT(),N*M, enemy_count);
        F.read_scores(scores);

        if (grid->get_capturedT() < requiredCapture) {
            S.get_lose();
            D.print_text("YOU LOSE\n  ", window1, scores, player->get_name(), S.get_end_lose());
        }

        else {
           
            D.print_text("YOU WON\n", window1, scores, player->get_name(), S.get_end_win());
        }
    }
    window1.display();

};





/*GAME*/

void Game::run()
{
    while (ptr->get_window()->isOpen())
    {
        float elapsedTime = clock.restart().asSeconds();
        handleEvents();
        if (ptr->get_isRun())
            update(elapsedTime);
        ptr->draw();
    }
};

void Game:: handleEvents()
{
    Event e;
    while (ptr->get_window()->pollEvent(e))
    {
        if (e.type == Event::Closed) ptr->get_window()->close();
        if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) reset();
    }
}

void Game::update(float elapsedTime)
{
    ptr->get_player()->handleInput();
    ptr->set_isRun(ptr->get_player()->move(elapsedTime));

    for (auto& enemy : ptr->get_enem())
    {
        enemy.move();

    }

    if (ptr->get_grid()->getTile(ptr->get_player()->get_x(), ptr->get_player()->get_y()) == 1)
    {
        ptr->get_player()->set_dx(0);
        ptr->get_player()->set_dy(0);

        for (auto& enemy : ptr->get_enem()) {
            int ey = enemy.get_y() / ptr->get_grid()->get_ts();
            int ex = enemy.get_x() / ptr->get_grid()->get_ts();
            ptr->get_grid()->drop(ey, ex);
        }
        ptr->get_grid()->fillEnclosedArea(); 
    }

    for (auto& enemy : ptr->get_enem()) {
        if (ptr->get_grid()->getTile(enemy.get_x() / ptr->get_grid()->get_ts(), enemy.get_y() / ptr->get_grid()->get_ts()) == 2)
            ptr->set_isRun(false);
    }
};

void Game:: reset()
{
    ptr->get_grid()->initialize();
    ptr->get_player()->set_x(10);
    ptr->get_player()->set_y(0);
    ptr->set_isRun(true);
}

