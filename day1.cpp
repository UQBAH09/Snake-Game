// === Includes ===
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
using namespace std;

// === Game Classes ===

class Fruit {
    vector<int> position;
public:
    int fruitType;
    Fruit(int x = 9, int y = 9, int type = 1) : position({x, y}), fruitType(type) {}
    vector<int> getPos() const { return position; }
    void changeFruitPos(const vector<vector<int>>& snake) {
        random_device rd; mt19937 gen(rd());
        uniform_int_distribution<int> dist(0, 19);
        vector<int> oldPos = position;
        while (true) {
            position[0] = dist(gen);
            position[1] = dist(gen);
            bool valid = true;
            for (const auto& seg : snake) {
                if (seg == position || position == oldPos) {
                    valid = false;
                    break;
                }
            }
            if (valid) break;
        }
    }
    void setFruitType(int type) { fruitType = type; }
    int getFruitType() const { return fruitType; }
};

class Score {
    int score;
public:
    Score() : score(0) {}
    Score operator++(int) { score++; return *this; }
    Score operator+=(int value) { score += value; return *this; }
    int getScore() const { return score; }
};

class Block {
    vector<int> position;
public:
    Block(int x, int y) : position({x, y}) {}
    bool checkIfHit(vector<int> pos) const { return position == pos; }
    vector<int> getPosition() const { return position; }
};

class Snake {
public:
    vector<vector<int>> snake;
    char dir;
    Snake() {
        for (int i = 0; i < 3; i++)
            snake.push_back({9, 3 + i});
        dir = 'w';
    }
    char checkDir() const { return dir; }
    vector<int> checkHead() { return snake[0]; }
    void increaseSnakeSize() { snake.push_back(snake.back()); }
    char move(char position, Fruit& f, Score& s, const vector<Block>& b) {
        int prevX = snake[0][0], prevY = snake[0][1];
        switch (position) {
            case 'w': snake[0][1]--; break;
            case 's': snake[0][1]++; break;
            case 'd': snake[0][0]++; break;
            case 'a': snake[0][0]--; break;
        }

        if (snake[0][1] >= 20) snake[0][1] = 0;
        else if (snake[0][1] < 0) snake[0][1] = 19;
        if (snake[0][0] >= 20) snake[0][0] = 0;
        else if (snake[0][0] < 0) snake[0][0] = 19;

        for (const auto& block : b)
            if (block.checkIfHit(snake[0])) return 'b';

        for (size_t i = 1; i < snake.size(); i++)
            if (snake[0] == snake[i]) return 's';

        dir = position;

        if (f.getPos() == snake[0]) {
            increaseSnakeSize();
            f.changeFruitPos(snake);
            s++;
        }

        for (size_t i = 1; i < snake.size(); i++) {
            swap(prevX, snake[i][0]);
            swap(prevY, snake[i][1]);
        }

        return 'G';
    }
};

class Arena {
protected:
    vector<Block> blocks;
public:
    virtual ~Arena() {}
    virtual void resetArena() { blocks.clear(); }
    vector<Block>& getBlocks() { return blocks; }
};

class Classic : public Arena {
public:
    Classic() { resetArena(); }
};

class Complex : public Arena {
public:
    Complex() {
        resetArena();
        for (int i = 0; i < 20; i++) {
            blocks.push_back(Block(0, i));
            blocks.push_back(Block(19, i));
        }
        for (int i = 1; i < 19; i++) {
            blocks.push_back(Block(i, 0));
            blocks.push_back(Block(i, 19));
        }
    }
};

class Boundary : public Arena {
public:
    Boundary() {
        resetArena();
        for (int i = 4; i < 8; i++)
            for (int j = 4; j < 8; j++)
                blocks.push_back(Block(i, j));
        for (int i = 4; i < 8; i++)
            for (int j = 12; j < 16; j++)
                blocks.push_back(Block(i, j));
        for (int i = 12; i < 16; i++)
            for (int j = 4; j < 8; j++)
                blocks.push_back(Block(i, j));
        for (int i = 12; i < 16; i++)
            for (int j = 12; j < 16; j++)
                blocks.push_back(Block(i, j));
    }
};

class GameSFML {
public:
    Arena* arena;
    Snake snake;
    Fruit fruit;
    Score score;
    float cellSize;
    bool gameOver = false;
    GameSFML(Arena* a, float cell) : arena(a), cellSize(cell) {}
    char update(char dir) {
        return snake.move(dir, fruit, score, arena->getBlocks());
    }
    void render(sf::RenderWindow& window, sf::Text& scoreText, int snakeStyle) {
        window.clear(sf::Color::Black);

        sf::RectangleShape fruitRect(sf::Vector2f(cellSize, cellSize));
        switch (fruit.getFruitType()) {
            case 1: fruitRect.setFillColor(sf::Color::Red); break;
            case 2: fruitRect.setFillColor(sf::Color::Green); break;
            case 3: fruitRect.setFillColor(sf::Color::Yellow); break;
            case 4: fruitRect.setFillColor(sf::Color::Magenta); break;
            case 5: fruitRect.setFillColor(sf::Color::Cyan); break;
        }
        auto fpos = fruit.getPos();
        fruitRect.setPosition(fpos[0] * cellSize, fpos[1] * cellSize);
        window.draw(fruitRect);

        for (size_t i = 0; i < snake.snake.size(); ++i) {
            sf::RectangleShape seg(sf::Vector2f(cellSize, cellSize));
            switch (snakeStyle) {
                case 1: seg.setFillColor(i == 0 ? sf::Color::Green : sf::Color(0, 180, 0)); break;
                case 2: seg.setFillColor(i == 0 ? sf::Color::Cyan : sf::Color(0, 128, 255)); break;
                case 3: seg.setFillColor(i == 0 ? sf::Color::Yellow : sf::Color(255, 215, 0)); break;
                case 4: seg.setFillColor(i == 0 ? sf::Color::Magenta : sf::Color(200, 0, 200)); break;
                case 5: seg.setFillColor(i == 0 ? sf::Color::White : sf::Color(160, 160, 160)); break;
            }
            seg.setPosition(snake.snake[i][0] * cellSize, snake.snake[i][1] * cellSize);
            window.draw(seg);
        }

        for (auto& block : arena->getBlocks()) {
            auto pos = block.getPosition();
            sf::RectangleShape b(sf::Vector2f(cellSize, cellSize));
            b.setFillColor(sf::Color::Blue);
            b.setPosition(pos[0] * cellSize, pos[1] * cellSize);
            window.draw(b);
        }

        scoreText.setString("Score: " + to_string(score.getScore()));
        window.draw(scoreText);
    }
    char getDir() const { return snake.checkDir(); }
};

// === Includes ===
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
using namespace std;

// === Game Classes ===

// [SNIPPED: same as before for Fruit, Score, Block, Snake, Arena, Classic, Complex, Boundary, GameSFML classes]
// Continue from the last class GameSFML

// === Main Function ===

int main() {
    const int gridSize = 20;
    const float cellSize = 32.f;
    sf::RenderWindow window(sf::VideoMode(gridSize * cellSize, gridSize * cellSize), "Snake Game", sf::Style::Close);
    int speedLevel = 2;
    int snakeStyle = 1;
    int fruitStyle = 1;
    window.setFramerateLimit(5 + speedLevel * 3);

    enum GameState { MENU, PLAYING, GAME_OVER, HIGH_SCORES, SPEED_SELECT, MODE_SELECT, SNAKE_STYLE_SELECT, FRUIT_STYLE_SELECT };
    GameState state = MENU;

    sf::Font font;
    if (!font.loadFromFile("DejaVuSans.ttf")) return -1;

    sf::Text scoreText("Score: 0", font, 20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    sf::Text gameOverText("Game Over! Press ESC to return", font, 24);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);
    gameOverText.setPosition(100, gridSize * cellSize / 2 - 30);

    sf::RectangleShape buttons[6];
    sf::Text buttonTexts[6];
    string labels[6] = { "Play Game", "High Scores", "Select Speed", "Arena Mode", "Snake Style", "Fruit Style" };
    for (int i = 0; i < 6; ++i) {
        buttons[i].setSize(sf::Vector2f(180, 50));
        buttons[i].setFillColor(sf::Color(100, 149, 237));
        buttons[i].setPosition((gridSize * cellSize - 180) / 2, 150 + i * 70);
        buttonTexts[i].setFont(font);
        buttonTexts[i].setCharacterSize(30);
        buttonTexts[i].setString(labels[i]);
        buttonTexts[i].setFillColor(sf::Color::White);
        buttonTexts[i].setPosition(buttons[i].getPosition().x + 10, buttons[i].getPosition().y + 10);
    }

    sf::Text speedPrompt("Choose Speed (1-5):", font, 24);
    speedPrompt.setFillColor(sf::Color::White);
    speedPrompt.setPosition(80, gridSize * cellSize / 2 - 20);

    sf::Text modePrompt("Select Arena: 1=Classic, 2=Boundary, 3=Complex", font, 20);
    modePrompt.setFillColor(sf::Color::White);
    modePrompt.setPosition(50, gridSize * cellSize / 2);

    sf::Text stylePrompt("Select Snake Style (1-5)", font, 24);
    stylePrompt.setFillColor(sf::Color::White);
    stylePrompt.setPosition(100, gridSize * cellSize / 2);

    sf::Text fruitPrompt("Select Fruit Style (1-5)", font, 24);
    fruitPrompt.setFillColor(sf::Color::White);
    fruitPrompt.setPosition(100, gridSize * cellSize / 2);

    int selectedMode = 1;
    Arena* arena = new Classic();
    GameSFML* game = new GameSFML(arena, cellSize);
    game->fruit.setFruitType(fruitStyle);
    char direction = 'w';

    auto loadTopScores = []() {
        ifstream file("score.txt");
        vector<int> scores;
        int s;
        while (file >> s) scores.push_back(s);
        sort(scores.begin(), scores.end(), greater<int>());
        if (scores.size() > 5) scores.resize(5);
        return scores;
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                if (state != MENU) {
                    state = MENU;
                    delete game;
                    delete arena;
                    if (selectedMode == 1) arena = new Classic();
                    else if (selectedMode == 2) arena = new Boundary();
                    else arena = new Complex();
                    game = new GameSFML(arena, cellSize);
                    game->fruit.setFruitType(fruitStyle);
                    direction = 'w';
                    continue;
                } else {
                    window.close();
                }
            }

            if (state == MENU && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                for (int i = 0; i < 6; ++i) {
                    if (buttons[i].getGlobalBounds().contains(mousePos)) {
                        if (i == 0) {
                            delete game;
                            delete arena;
                            if (selectedMode == 1) arena = new Classic();
                            else if (selectedMode == 2) arena = new Complex();
                            else arena = new Boundary();
                            game = new GameSFML(arena, cellSize);
                            game->fruit.setFruitType(fruitStyle);
                            direction = 'w';
                            state = PLAYING;
                        } else if (i == 1) state = HIGH_SCORES;
                        else if (i == 2) state = SPEED_SELECT;
                        else if (i == 3) state = MODE_SELECT;
                        else if (i == 4) state = SNAKE_STYLE_SELECT;
                        else if (i == 5) state = FRUIT_STYLE_SELECT;
                    }
                }
            }

            if (state == SPEED_SELECT && event.type == sf::Event::KeyPressed) {
                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num5) {
                    speedLevel = event.key.code - sf::Keyboard::Num0;
                    window.setFramerateLimit(5 + speedLevel * 3);
                    state = MENU;
                }
            }

            if (state == MODE_SELECT && event.type == sf::Event::KeyPressed) {
                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num3) {
                    selectedMode = event.key.code - sf::Keyboard::Num0;
                    state = MENU;
                }
            }

            if (state == SNAKE_STYLE_SELECT && event.type == sf::Event::KeyPressed) {
                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num5) {
                    snakeStyle = event.key.code - sf::Keyboard::Num0;
                    state = MENU;
                }
            }

            if (state == FRUIT_STYLE_SELECT && event.type == sf::Event::KeyPressed) {
                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num5) {
                    fruitStyle = event.key.code - sf::Keyboard::Num0;
                    game->fruit.setFruitType(fruitStyle);
                    state = MENU;
                }
            }

            if (state == PLAYING && event.type == sf::Event::KeyPressed) {
                char current = game->getDir();
                if (event.key.code == sf::Keyboard::W && current != 's') direction = 'w';
                else if (event.key.code == sf::Keyboard::S && current != 'w') direction = 's';
                else if (event.key.code == sf::Keyboard::A && current != 'd') direction = 'a';
                else if (event.key.code == sf::Keyboard::D && current != 'a') direction = 'd';
            }
        }

        if (state == MENU) {
            window.clear(sf::Color(30, 30, 30));
            for (int i = 0; i < 6; ++i) {
                window.draw(buttons[i]);
                window.draw(buttonTexts[i]);
            }
        } else if (state == SPEED_SELECT) {
            window.clear(sf::Color::Black);
            window.draw(speedPrompt);
        } else if (state == MODE_SELECT) {
            window.clear(sf::Color::Black);
            window.draw(modePrompt);
        } else if (state == SNAKE_STYLE_SELECT) {
            window.clear(sf::Color::Black);
            window.draw(stylePrompt);
        } else if (state == FRUIT_STYLE_SELECT) {
            window.clear(sf::Color::Black);
            window.draw(fruitPrompt);
        } else if (state == PLAYING) {
            if (!game->gameOver) {
                char result = game->update(direction);
                if (result == 'b' || result == 's') {
                    game->gameOver = true;
                    ofstream out("score.txt", ios::app);
                    if (out.is_open()) {
                        out << game->score.getScore() << endl;
                        out.close();
                    }
                }
            }
            game->render(window, scoreText, snakeStyle);
            if (game->gameOver) {
                window.draw(gameOverText);
                state = GAME_OVER;
            }
        } else if (state == GAME_OVER) {
            game->render(window, scoreText, snakeStyle);
            window.draw(gameOverText);
        } else if (state == HIGH_SCORES) {
            window.clear(sf::Color::Black);
            vector<int> topScores = loadTopScores();
            for (size_t i = 0; i < topScores.size(); ++i) {
                sf::Text t("Score " + to_string(i + 1) + ": " + to_string(topScores[i]), font, 24);
                t.setPosition(180, 100 + i * 40);
                t.setFillColor(sf::Color::White);
                window.draw(t);
            }
            sf::Text back("Press ESC to return", font, 20);
            back.setPosition(180, 350);
            back.setFillColor(sf::Color::Yellow);
            window.draw(back);
        }

        window.display();
    }

    delete game;
    delete arena;
    return 0;
}
