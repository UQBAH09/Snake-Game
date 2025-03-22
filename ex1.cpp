#include <iostream>
#include <vector>
#include <random>
#include<fstream>
#include<algorithm>

using namespace std;

// Fruit class handles fruit spawning and repositioning
class Fruit {
    vector<int> position;
public:
    char fruit;
    Fruit(int x = 9, int y = 9) : position({x, y}), fruit('F') {}

    // Getter function to return fruit position
    vector<int> getPos() const { return position; }

    // Change fruit position ensuring it doesn't overlap with the snake
    void changeFruitPos(const vector<vector<int>>& snake) {
        // cheak whether the snake not there before placing the fruit there.
    }
};

// Score class to track and manage player's score
class Score {
    int score;
public:
    Score() : score(0) {}
    
	Score operator++(int){
    	score++;
    	return *this;
	}
	Score operator+=(int value){
    	score+= value;
    	return *this;
	}

    int getScore() const { return score; } // Return current score
    
	void putscoreinfile(){
    	ofstream outFile("score.txt");
    	if(outFile.is_open()){
    	 outFile<<score<<endl;
		}
		
	}
	void gettopscores(){
		ifstream inFile("score.txt");
		vector<int>scores;
		if(!inFile.is_open()){
			cout<<"Error in opening the file!!"<<endl;
			return;
		}
		while(inFile>>score){
			scores.push_back(score);
		}
		inFile.close();
		if(scores.size()<5){
			cout<<"There are less than 5 scores in the file"<<endl;
			return;
		}
		sort(scores.begin(),scores.end(),greater<int>());
		
		cout<<"Top 5 scores are:"<<endl;
		for(int i=0;i<5;i++){
			cout<<scores[i]<<endl;
		}
	}
	void appendscoretofile(){
		ofstream outFile("score.txt",ios::app);
		if(outFile.is_open()){
			outFile<<score<<endl;
			outFile.close();
		}
		else{
			cerr<<"Error: Unable to open file for appending"<<endl;
		}
	}
};

// Block class represents obstacles in the game
class Block {
    vector<int> position;
    char block;
public:
    Block(int x, int y) : position({x, y}), block('B') {}

    char getBlockType() { return block; }

    // Check if the snake collides with the block
    bool checkIfHit(vector<int> pos) const {
        return position == pos;
    }

    vector<int> getPosition() const {  
        return position;
    }
};

// Snake class handles movement, collision detection, and growth
class Snake {
public:
    vector<vector<int>> snake;
    char dir;

    Snake() {
        // Initial snake size with three segments
        for (int i = 0; i < 3; i++) {
            snake.push_back({9, 3 + i});
        }
        dir = 'w'; // Default direction (up)
    }

    char checkDir() { return dir; } // Get current direction
    vector<int> checkHead() { return snake[0]; } // Get snake's head position

    // Increase the snake's size by duplicating its last segment
    void increaseSnakeSize() {
        snake.push_back(snake.back());
    }

    // Move the snake based on user input, check for collisions, and update position
    char move(char position, Fruit& f, Score& s, const vector<Block>& b) {
        int prevX = snake[0][0], prevY = snake[0][1];
    
        // Update snake's head position based on direction
        switch (position) {
            case 'w': snake[0][1]--; break;
            case 's': snake[0][1]++; break;
            case 'd': snake[0][0]++; break;
            case 'a': snake[0][0]--; break;
        }
    
        if (snake[0][1] >= 20)  // If snake moves out from the bottom, come out from top
            snake[0][1] = 0;
        else if (snake[0][1] < 0)  // If snake moves out from the top, come out from bottom
            snake[0][1] = 19;
        
        if (snake[0][0] >= 20)  // If snake moves out from the right, come out from left
            snake[0][0] = 0;
        else if (snake[0][0] < 0)  // If snake moves out from the left, come out from right
            snake[0][0] = 19;
    
        // Check collision with blocks
        for (const auto& block : b) {
            if (block.checkIfHit(snake[0])) {
                return 'b';
            }
        }
    
        // Check collision with itself
        for (size_t i = 1; i < snake.size(); i++) {
            if (snake[0] == snake[i]) {
                return 's';
            }
        }
    
        dir = position; // Update direction
    
        // Check if the snake eats the fruit
        vector<int> fruitPos = f.getPos();
        if (fruitPos[0] == snake[0][0] && fruitPos[1] == snake[0][1]) {
            increaseSnakeSize();  // Grow snake
            f.changeFruitPos(snake); // Respawn fruit
            s++; // Increase score
        }
    
        // Move the snake's body
        for (size_t i = 1; i < snake.size(); i++) {
            swap(prevX, snake[i][0]);
            swap(prevY, snake[i][1]);
        }
    
        return 'G'; // Game continues
    }
    
};

// Arena class handles the game board and obstacles
class Arena {
	protected:
    char arena[20][20];
    vector<Block> blocks;
public:
    void resetArena() {
        for (int i = 0; i < 20; i++)
            for (int j = 0; j < 20; j++)
                arena[i][j] = '.';
    }

    const vector<Block>& getBlocks() const { return blocks; } // Return block list

    // Print the arena with snake, fruit, and obstacles
    void printArena(const Snake& s, const Fruit& f, Score& sc) {
        cout << endl;
        resetArena();

        // Place the fruit
        vector<int> fruitPos = f.getPos();
        arena[fruitPos[1]][fruitPos[0]] = 'F'; // Correct row/col indexing

        // Place the snake
        for (size_t i = 0; i < s.snake.size(); i++) {
            int x = s.snake[i][0], y = s.snake[i][1];
            arena[y][x] = (i == 0) ? '@' : '#'; // '@' for head, '#' for body
        }

        // Place blocks
        for (auto& block : blocks) {
            vector<int> pos = block.getPosition();
            int x = pos[0], y = pos[1];
            if (x >= 0 && x < 20 && y >= 0 && y < 20) {
                arena[y][x] = 'B';
            }
        }

        // Print the arena
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++)
                cout << arena[i][j] << " ";
            cout << endl;
        }

        cout << "Score: " << sc.getScore();
    }
};
class Classic:public Arena{
	public:
		Classic(){
		resetArena();
	}
};
class Boundary:public Arena{
	public:
	Boundary(){
		resetArena();
		for(int i=0;i< 20;i++){
			blocks.push_back(Block({0,i}));
			blocks.push_back(Block({19,i}));	
		}
		for(int i=1;i< 19;i++){
			blocks.push_back(Block({i,0}));
			blocks.push_back(Block({i,19}));	
		}
	}	
};
class Complex:public Boundary{
	public:
		Complex(){
			resetArena();
			for(int i=4;i<8;i++){
				for(int j=4;j<8;j++){
				blocks.push_back(Block({i,j}));
				}
			}
			for(int i=4;i<8;i++){
				for(int j=12;j<16;j++){
				blocks.push_back(Block({i,j}));	
				}
			}
			for(int i=12;i<16;i++){
				for(int j=4;j<8;j++){
				blocks.push_back(Block({i,j}));
				}
			}
			for(int i=12;i<16;i++){
				for(int j=12;j<16;j++){
					blocks.push_back(Block({i,j}));
				}
			}
		}
		
};
// Game class to manage game logic and interaction
class Game {
    Arena arena;
    Snake snake;
    Fruit fruit;
    Score score;
    Classic classic;
    Boundary boundary;
    Complex complex;

public:
    // Process user input and move the snake
    char move(char dir) {
        // Prevent movement in the opposite direction
        if ((snake.checkDir() == 'w' && dir == 's') ||
            (snake.checkDir() == 's' && dir == 'w') ||
            (snake.checkDir() == 'a' && dir == 'd') ||
            (snake.checkDir() == 'd' && dir == 'a')) {
            return 'G';
        }
        char g = snake.move(dir, fruit, score, arena.getBlocks());
        
        if (g == 'b' || g == 's'){
        	score.appendscoretofile();
        	score.gettopscores();
		}
        return g;
    }

    void printGame() { arena.printArena(snake, fruit, score); }
};

// Main function to run the game loop
//int main() {
//    Game game;
//    
//    game.printGame();
//    cout << endl;
//    
//    char move;
//    
//    while (true) {
//        cout << "Enter move (w/a/s/d) or 'q' to quit: ";
//        cin >> move;
//        
//        if (move =='q' || move == 'w' || move == 's' || move == 'a' || move == 'd'){
//            if (move == 'q') {
//                cout << "Game Over. You quit the game." << endl;
//                break;
//            }
//
//            char result = game.move(move);
//
//            if (result == 'b') {
//                cout << "Game Over. You hit a block." << endl;
//                break;
//            }
//            else if (result == 's') {
//                cout << "Game Over. You hit yourself." << endl;
//                break;
//            }
//        }
//        game.printGame();
//        cout << endl;
//        
//    }
//
//    return 0;
//}


int main(){
	Snake s;
	Fruit f;
	Score sc;
	
	Classic c;
	c.printArena(s,f,sc);
	
	Boundary b;
	b.printArena(s,f,sc);
	
	Complex cp;
	cp.printArena(s,f,sc);
}