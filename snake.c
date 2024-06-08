#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include "windows.h"

#define MAX_X 15
#define MAX_Y 15
#define START_DELAY 700
#define BLUE FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define YELLOW FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define RED FOREGROUND_RED | FOREGROUND_INTENSITY
#define GREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY

unsigned short COLOR1, COLOR2;
int GAMEMODE;
int DELAY = START_DELAY;

enum DIRECTION {LEFT, RIGHT, UP, DOWN};
enum KEYS {KEY_LEFT = 'a', KEY_RIGHT = 'd',
		   KEY_UP = 'w', KEY_DOWN = 's', 
		   KEY_STOP = 'f', KEY_PAUSE = 'p', 
           KEY2_LEFT = 'j', KEY2_RIGHT = 'l',
		   KEY2_UP = 'i', KEY2_DOWN = 'k',};

typedef struct tail_t{
	int x;
	int y;
	} tail_t;	
typedef struct snake_t{
	int x;
	int y;
	int direction;
	struct tail_t * tail;
	size_t tsize;
	int score;
	}  snake_t;
typedef struct food_t{
	int x;
	int y;
    int isEaten;
} food_t;

struct snake_t initSnake(int x, int y, size_t tsize){
	struct snake_t snake;
	snake.x = x;
	snake.y = y;
	snake.direction = LEFT;
	snake.tsize = tsize;
	snake.score = 0;
	snake.tail = (tail_t *) malloc (sizeof(tail_t) * 100);
	for (int i =0; i < tsize; ++i){
		snake.tail[i].x = x + i +1;
		snake.tail[i].y = y;
		}
	return snake;
}
struct food_t initFood(){
	struct food_t fd;
    fd.isEaten = 0;
	fd.x = rand() % (MAX_X-1);
	fd.y = rand() % (MAX_Y-1);
	return fd;
}
void refreshFood(food_t *food){
	food->isEaten = 0;
    food->x = rand() % (MAX_X-1);
	food->y = rand() % (MAX_Y-1);
}
void repairFood(food_t *food, snake_t snake){
	for(int i = 0; i < snake.tsize; i++)
		while (food->isEaten == 0 && snake.tail[i].x == food->x && snake.tail[i].y == food->y)
			refreshFood(food);
}
void printSnake(snake_t snake, snake_t snake2, food_t food){
    char matrix[MAX_X][MAX_Y];
	for (int i = 0; i < MAX_X; ++i){
        for (int j = 0; j < MAX_Y; ++j){
			matrix[i][j] = ' ';
        }
    }

    matrix[snake.x][snake.y] = '@';
    for (int i = 0; i < snake.tsize; ++i){
        matrix[snake.tail[i].x][snake.tail[i].y] = '*';
    }
	if(GAMEMODE){
		matrix[snake2.x][snake2.y] = '%';
		for (int i = 0; i < snake2.tsize; ++i){
			matrix[snake2.tail[i].x][snake2.tail[i].y] = '#';
		}
	}
	matrix[food.x][food.y] = '$';

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int j = 0; j < MAX_Y; ++j){
        for (int i = 0; i < MAX_X; ++i){
			SetConsoleTextAttribute(hStdOut,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            if(matrix[i][j] == '@' || matrix[i][j] == '*')
				SetConsoleTextAttribute(hStdOut,COLOR1);
            if(matrix[i][j] == '%' || matrix[i][j] == '#')
				SetConsoleTextAttribute(hStdOut,COLOR2);
			printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
}
void increaseScore(snake_t *snake, snake_t *snake2, food_t food){
	if(GAMEMODE)
		if (snake->x == food.x && snake->y == food.y)
			snake->score++;
		else
			snake2->score++;
	else{
		snake->score++;
	}

}
void printScore(snake_t snake, snake_t snake2){
	GAMEMODE ? printf("SCORE 1: %d\t SCORE 2: %d\n", snake.score, snake2.score):printf("\tSCORE: %d\n", snake.score);
}
void printPause(){
	printf("\tPAUSE\n");
}
void eatTail(snake_t *snake){
	for (int i = 0; i < snake->tsize; i++){
		if(snake->x == snake->tail[i].x &&
			snake->y == snake->tail[i].y){
				snake->score -= (snake->tsize - i);
				snake->tsize = i;
		}
	}
}
void move(snake_t *snake,food_t *food){
	for (int i = snake->tsize; i > 0; i--){
		snake->tail[i] = snake->tail[i-1];
	}
	snake->tail[0].x = snake->x;
	snake->tail[0].y = snake->y;
    
    switch (snake->direction){
		case UP:
			snake->y = snake->y - 1;	
            if (snake->y < 0){
                snake->y = MAX_Y - 1;
            }
			break;
		case DOWN:
			snake->y = snake->y + 1;	
            if (snake->y == MAX_Y - 1){
                snake->y = 0;
            }
			break;
		case LEFT:
			snake->x = snake->x - 1;	
            if (snake->x < 0){
                snake->x = MAX_X - 1;
            }
			break;
		case RIGHT:
			snake->x = snake->x + 1;	
            if (snake->x == MAX_X - 1){
                snake->x = 0;
            }
			break;
	}

    if (snake->x == food->x &&snake->y == food->y){
        food->isEaten = 1;
        snake->tsize++;
    }
}
void changeDirection(snake_t *snake, char key){
	if(key == KEY_UP && snake->direction != DOWN)
		snake->direction = UP;
	if(key == KEY_DOWN && snake->direction != UP)
		snake->direction = DOWN;
	if(key == KEY_LEFT && snake->direction != RIGHT)
		snake->direction = LEFT;
	if(key == KEY_RIGHT && snake->direction != LEFT)
		snake->direction = RIGHT;
}
void botSnakeDirection(snake_t *snake2,food_t food){
    if ((snake2->direction == LEFT || snake2->direction == RIGHT) 
                                        && snake2->y == food.y)
        return;
    if ((snake2->direction == UP || snake2->direction == DOWN) 
                                        && snake2->x == food.x)
        return;

    if ((snake2->direction == LEFT || snake2->direction == RIGHT) 
                                        && snake2->y != food.y){
        if (food.y < snake2->y){
            snake2->direction = UP;
			return;
        }
        else {
            snake2->direction = DOWN;
			return;
        }
    }
    if ((snake2->direction == UP || snake2->direction == DOWN) 
                                        && snake2->x != food.x){
        if (food.x < snake2->x){
            snake2->direction = LEFT;
			return;
        }
        else {
            snake2->direction = RIGHT;
			return;
        }                                        
    }
	if((snake2->direction == UP || snake2->direction == DOWN)&& snake2->x > food.x)
		snake2->direction = LEFT;
	else if((snake2->direction == UP || snake2->direction == DOWN)&& snake2->x < food.x)
		snake2->direction = RIGHT;
	else if((snake2->direction == LEFT || snake2->direction == RIGHT) && snake2->y > food.y)
		snake2->direction = UP;
	else if((snake2->direction == LEFT || snake2->direction == RIGHT)&& snake2->y < food.y)
		snake2->direction = DOWN;
}
void startMenu(){
	printf("Hello! This is SNAKE THE GAME !");
	printf("Rules:\n1)'w','a','s','d' - for move;\n2)'p' - for pause;\n3)'f' - for exit.\n");
	printf("Select game option:\n1)Solo game\n2)Competitive game\n");
	printf("Gamemode: ");
	char c;
	while(c != '1' && c != '2')
		scanf("%c", &c);
	switch (c)
	{
	case '1':
		GAMEMODE = 0;
		break;
	case '2':
		GAMEMODE = 1;
		break;
	}
	c = 0;
	printf("Choose snake color:\n1)Blue\n2)Yellow\n3)Red\n4)Green\nColor:");
	while(c != '1' && c != '2'&& c != '3'&& c != '4')
		scanf("%c", &c);
	switch (c)
	{
	case '1':
		COLOR1 = BLUE;
		COLOR2 = RED;
		break;
	case '2':
		COLOR1 = YELLOW;
		COLOR2 = GREEN;
		break;
	case '3':
		COLOR1 = RED;
		COLOR2 = BLUE;
		break;
	case '4':
		COLOR1 = GREEN;
		COLOR2 = YELLOW;
		break;
	}
	system("cls");
}

int main(){
	startMenu();
	struct snake_t snake = initSnake( 10, 5, 3);
    struct snake_t snake2 = initSnake( 2, 2, 3);
	struct food_t food = initFood();
	char key;
	while(key != KEY_STOP){
        system("cls");
		if(kbhit()){
			key = tolower(getch());
			changeDirection(&snake, key);
		}
		if(key != KEY_PAUSE){
			move(&snake, &food);
            if(GAMEMODE){
				botSnakeDirection(&snake2,food);
            	move(&snake2,&food);
			}
			if(food.isEaten){
				increaseScore(&snake, &snake2, food);
				DELAY = START_DELAY - 25*snake.score;
				if(DELAY <= 50)
					DELAY = 50;
				refreshFood(&food);
				repairFood(&food, snake);
				repairFood(&food, snake2);
			}
			eatTail(&snake);
			eatTail(&snake2);
			printScore(snake, snake2);
		}else {
			printPause();
		}	
			printSnake(snake,snake2, food);
			Sleep(DELAY); 
	}
	system("cls");
	printScore(snake, snake2);
	printf("\tGAME OVER\n");
	return 0;
}