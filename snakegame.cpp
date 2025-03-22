#include <iostream>
#include <vector>
#include <random>
#include <chrono>  // for functions like steady_clock and duration_cast (time oriented functions)
#include <cstdlib> // for rand() and srand()
#include <ctime>   // for time()

using namespace std;

// FruitType class allows the user to choose from a set of fruit symbols
class FruitType
{
    vector<char> fruitOptions; // List of available fruit symbols
    char selectedFruit;        // The fruit symbol selected by the user

public:
    // Constructor
    FruitType()
    {
        fruitOptions = {'F', '*', '@', '&', '$'}; // Available fruit symbols
        selectedFruit = fruitOptions[0];          // Set default fruit to the first one
    }

    // Function to display fruit choices and let the user select one
    void chooseFruit()
    {
        cout << "Choose your fruit symbol:\n";

        // Display each fruit option with a corresponding number
        for (size_t i = 0; i < fruitOptions.size(); i++)
        {
            cout << i + 1 << ". " << fruitOptions[i] << endl;
        }

        int choice;
        cout << "Enter choice number: ";
        cin >> choice;

        // If input is valid, update the selected fruit
        if (choice > 0 && choice <= fruitOptions.size())
        {
            selectedFruit = fruitOptions[choice - 1];
            cout << "You selected: " << selectedFruit << endl;
        }
        // Otherwise, use the default fruit
        else
        {
            cout << "Invalid choice. Default fruit selected: " << selectedFruit << endl;
        }
    }

    // Getter function to retrieve the selected fruit symbol
    char getSelectedFruit() const
    {
        return selectedFruit;
    }
};

// Fruit class handles fruit spawning and repositioning
class Fruit
{
    vector<int> position; // Stores fruit's current position (x, y)
public:
    char fruit; // Added to store the character/symbol used to display the fruit

    // Constructor allows setting a custom fruit character, with default as 'F'
    Fruit(char fruitChar = 'F', int x = 9, int y = 9) : position({x, y}), fruit(fruitChar) {}

    // Getter function to return fruit position
    vector<int> getPos() const { return position; }

    // Change fruit position ensuring it doesn't overlap with the snake
    void changeFruitPos(const vector<vector<int>> &snake)
    {
        // cheak whether the snake not there before placing the fruit there.
    }
};

class SpecialFruitType
{
    vector<char> specialFruits; // List of special fruit symbols
    char specialFruit;          // Currently selected special fruit

public:
    // Constructor initializes available fruits and sets default
    SpecialFruitType()
    {
        specialFruits = {'$', '&', 'P', 'S'}; // Define power-up fruits
        specialFruit = specialFruits[0];      // Default fruit

        srand(time(0)); // Initialize random seed only once per program run
    }

    // Randomly choose a special fruit from the list
    void chooseSpecialFruit()
    {
        int index = rand() % specialFruits.size(); // Pick a random index (0 to size-1)
        specialFruit = specialFruits[index];       // Set selected fruit
    }

    // Getter for the selected fruit
    char getSpecialFruit() const
    {
        return specialFruit;
    }
};

class SpecialFruit
{
    vector<int> position; // Position of the special fruit
    char fruitChar;       // Symbol representing the special fruit
    bool active;          // Whether the fruit is currently active (visible)

public:
    // Constructor: set default symbol and inactive status
    SpecialFruit(char c = '$') : fruitChar(c), active(false)
    {
        srand(time(0)); // Seeds the random number generator using the current time
    }

    // place fruit randomly, avoiding overlap with the snake
    void spawn(const vector<vector<int>> &snake)
    {
        int x, y;
        bool overlap; // flag to check if the position is already occupied by the snake.

        do
        {
            x = rand() % 20; // Random x from 0 to 19
            y = rand() % 20; // Random y from 0 to 19
            overlap = false; //  reset to false for each new random position.

            // Check if this position overlaps with any part of the snake
            for (const auto &segment : snake)
            {
                if (segment[0] == x && segment[1] == y)
                {
                    overlap = true; // shows that it is overlapping if the position is matched
                    break;
                }
            }
        } while (overlap); // Keep generating positions until a safe one is found

        position = {x, y}; // Set the new position for the fruit
        active = true;     // Mark the fruit as active and then display on the game
    }

    // remove the fruit from the board
    void deactivate()
    {
        active = false; // flag set back to false
    }

    // acts as a getter and tells whether the special fruit is currently active
    bool isActive() const
    {
        return active;
    }

    //  returns the current position of the special fruit so that it can be drawn on the board.
    vector<int> getPosition() const
    {
        return position;
    }

    // returns the character symbol used to display the fruit
    char getChar() const
    {
        return fruitChar;
    }

    // lets you change the fruit's symbol
    void setChar(char c)
    {
        fruitChar = c;
    }
};

// Score class to track and manage player's score
class Score
{
    int score;

public:
    Score() : score(0) {}

    void increaseScore() { score++; } // Increment score

    int getScore() const { return score; } // Return current score
};

// Block class represents obstacles in the game
class Block
{
    vector<int> position;
    char block;

public:
    Block(int x, int y) : position({x, y}), block('B') {}

    char getBlockType() { return block; }

    // Check if the snake collides with the block
    bool checkIfHit(vector<int> pos) const
    {
        return position == pos;
    }

    vector<int> getPosition() const
    {
        return position;
    }
};

// Snake class handles movement, collision detection, and growth
class Snake
{
public:
    vector<vector<int>> snake;
    char dir;

    Snake()
    {
        // Initial snake size with three segments
        for (int i = 0; i < 3; i++)
        {
            snake.push_back({9, 3 + i});
        }
        dir = 'w'; // Default direction (up)
    }

    char checkDir() { return dir; }              // Get current direction
    vector<int> checkHead() { return snake[0]; } // Get snake's head position

    // Increase the snake's size by duplicating its last segment
    void increaseSnakeSize()
    {
        snake.push_back(snake.back());
    }

    // Move the snake based on user input, check for collisions, and update position
    char move(char position, Fruit &f, Score &s, const vector<Block> &b)
    {
        int prevX = snake[0][0], prevY = snake[0][1];

        // Update snake's head position based on direction
        switch (position)
        {
        case 'w':
            snake[0][1]--;
            break;
        case 's':
            snake[0][1]++;
            break;
        case 'd':
            snake[0][0]++;
            break;
        case 'a':
            snake[0][0]--;
            break;
        }

        if (snake[0][1] >= 20) // If snake moves out from the bottom, come out from top
            snake[0][1] = 0;
        else if (snake[0][1] < 0) // If snake moves out from the top, come out from bottom
            snake[0][1] = 19;

        if (snake[0][0] >= 20) // If snake moves out from the right, come out from left
            snake[0][0] = 0;
        else if (snake[0][0] < 0) // If snake moves out from the left, come out from right
            snake[0][0] = 19;

        // Check collision with blocks
        for (const auto &block : b)
        {
            if (block.checkIfHit(snake[0]))
            {
                return 'b';
            }
        }

        // Check collision with itself
        for (size_t i = 1; i < snake.size(); i++)
        {
            if (snake[0] == snake[i])
            {
                return 's';
            }
        }

        dir = position; // Update direction

        // Check if the snake eats the fruit
        vector<int> fruitPos = f.getPos();
        if (fruitPos[0] == snake[0][0] && fruitPos[1] == snake[0][1])
        {
            increaseSnakeSize();     // Grow snake
            f.changeFruitPos(snake); // Respawn fruit
            s.increaseScore();       // Increase score
        }

        // Move the snake's body
        for (size_t i = 1; i < snake.size(); i++)
        {
            swap(prevX, snake[i][0]);
            swap(prevY, snake[i][1]);
        }

        return 'G'; // Game continues
    }
};

// Arena class handles the game board and obstacles
class Arena
{
    char arena[20][20];
    vector<Block> blocks;

public:
    Arena()
    {
        resetArena();
        for (int i = 0; i < 20; i++)
        {
            blocks.push_back(Block(0, i)); // Add boundary blocks
            arena[0][i] = 'B';
        }
    }

    void resetArena()
    {
        for (int i = 0; i < 20; i++)
            for (int j = 0; j < 20; j++)
                arena[i][j] = '.';
    }

    const vector<Block> &getBlocks() const { return blocks; } // Return block list

    // Print the arena with snake, fruit, and obstacles
    void printArena(const Snake &s, const Fruit &f, Score &sc, const SpecialFruit &spf)
    {
        cout << endl;
        resetArena();

        // Place the fruit
        vector<int> fruitPos = f.getPos();
        arena[fruitPos[1]][fruitPos[0]] = 'F'; // Correct row/col indexing

        // Place the special fruit if it is active
        if (spf.isActive())
        {
            vector<int> spfPos = spf.getPosition();
            arena[spfPos[1]][spfPos[0]] = spf.getChar(); // Use actual special fruit symbol
        }

        // Place the snake
        for (size_t i = 0; i < s.snake.size(); i++)
        {
            int x = s.snake[i][0], y = s.snake[i][1];
            arena[y][x] = (i == 0) ? '@' : '#'; // '@' for head, '#' for body
        }

        // Place blocks
        for (auto &block : blocks)
        {
            vector<int> pos = block.getPosition();
            int x = pos[0], y = pos[1];
            if (x >= 0 && x < 20 && y >= 0 && y < 20)
            {
                arena[y][x] = 'B';
            }
        }

        // Print the arena
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 20; j++)
                cout << arena[i][j] << " ";
            cout << endl;
        }

        cout << "Score: " << sc.getScore();
    }
};

// Game class to manage game logic and interaction
class Game
{
    Arena arena;
    Snake snake;
    Fruit fruit;
    Score score;

public:
    // Parameterized constructor to accept pre-initialized components
    Game(Fruit f, Score s, Snake sn, Arena a) : fruit(f), score(s), snake(sn), arena(a) {}

    // Process user input and move the snake
    char move(char dir)
    {
        if ((snake.checkDir() == 'w' && dir == 's') ||
            (snake.checkDir() == 's' && dir == 'w') ||
            (snake.checkDir() == 'a' && dir == 'd') ||
            (snake.checkDir() == 'd' && dir == 'a'))
        {
            return 'G';
        }
        return snake.move(dir, fruit, score, arena.getBlocks());
    }

    void printGame(const SpecialFruit &spf) {
        arena.printArena(snake, fruit, score, spf);
    }
};

class Voice
{
public:
    // Play sound when snake hits a wall, block, or itself
    void playCollisionSound()
    {
        // add specific sound
    }

    // Play sound when the snake eats a regular fruit
    void playFruitSound()
    {
        // add specific sound
    }

    // Play sound when the snake eats a special fruit
    void playSpecialFruitSound()
    {
        // add specific sound
    }

    // Play sound when the game ends (either by quitting or death)
    void playGameOverSound()
    {
        // add specific sound
    }
};

// Main function to run the game loop
int main()
{

    // Create a FruitType object to let the user choose their fruit symbol
    FruitType fruitType;     // New addition
    fruitType.chooseFruit(); // Prompt user for choice

    // Initialize game objects using the selected fruit symbol
    Fruit fruit(fruitType.getSelectedFruit()); // Pass selected symbol to Fruit
    Score score;
    Snake snake;
    Arena arena;

    // Special fruit setup
    SpecialFruitType specialFruitType;           // Handles random selection
    SpecialFruit specialFruit;                   // The fruit (position + symbol)
    auto lastSpecialSpawn = steady_clock::now(); // Timer for 15s spawn (NEED TO UNDERSTAND FURTHER GPT)

    Game game(fruit, score, snake, arena);
    Voice voice;

    game.printGame(specialFruit); // include special fruit
    cout << endl;

    char move;

    while (true)
    {
        cout << "Enter move (w/a/s/d) or 'q' to quit: ";
        cin >> move;

        if (move == 'q' || move == 'w' || move == 's' || move == 'a' || move == 'd')
        {
            if (move == 'q')
            {
                voice.playGameOverSound(); // Sound for quitting
                cout << "Game Over. You quit the game." << endl;
                break;
            }

            char result = game.move(move);

            if (result == 'b')
            {
                voice.playCollisionSound(); // Hit block
                cout << "Game Over. You hit a block." << endl;
                break;
            }
            else if (result == 's')
            {
                voice.playCollisionSound(); // Hit self
                cout << "Game Over. You hit yourself." << endl;
                break;
            }

            // See if the snake grew
            static int prevLength = snake.snake.size(); // keeps old length
            if (snake.snake.size() > prevLength)
            {
                // Check if it ate special fruit
                if (specialFruit.isActive() && snake.checkHead() == specialFruit.getPosition())
                {
                    voice.playSpecialFruitSound();
                    specialFruit.deactivate(); // remove it after eaten
                }
                else
                {
                    voice.playFruitSound();
                }
                prevLength = snake.snake.size(); // update the stored length
            }
        }

        // GPT SUGGESTION TO AVOID FRUITS RECURRING OVER THE SPAN OF 1 SECOND :
        // This block controls the appearance of a special fruit in your Snake game, ensuring:
        // - It appears only once every 15 seconds.
        // - It doesn’t appear repeatedly in every game loop once the 15 seconds have passed.
        // - It doesn’t appear again if there’s already a special fruit on screen.
        // Spawn special fruit every 15 seconds (only once per window)

        // Get the current time using a steady (non-adjustable) clock
        auto now = steady_clock::now();

        // Calculate how many seconds have passed since the last special fruit was spawned
        // duration_cast<seconds>(...) converts the time difference into whole seconds
        auto timeSinceLast = duration_cast<seconds>(now - lastSpecialSpawn).count();

        // Only spawn a new special fruit if:
        // 1. At least 15 seconds have passed since the last spawn
        // 2. There is currently no active special fruit on the board
        if (timeSinceLast >= 15 && !specialFruit.isActive())
        {
            // Randomly select a special fruit character (e.g., 'P', 'S', '&')
            specialFruitType.chooseSpecialFruit();

            // Assign the randomly selected character to the actual special fruit object
            specialFruit.setChar(specialFruitType.getSpecialFruit());

            // Place the special fruit somewhere on the board,
            // ensuring it doesn't overlap with the snake's body
            specialFruit.spawn(snake.snake);

            // Update the last spawn time to the current time
            // This ensures we wait another 15 seconds before spawning the next one
            lastSpecialSpawn = now;
        }

        // Step 9: Display the entire game board, including the snake, fruit, and special fruit
        game.printGame(specialFruit);
        cout << endl;
    }

    return 0;
}
