#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include "windows.h"

#define MAX_X 15
#define MAX_Y 15
#define START_DELAY 500

int DELAY = START_DELAY;

enum DIRECTION {LEFT, RIGHT, UP, DOWN};
enum KEYS {KEY_LEFT = 'a', KEY_RIGHT = 'd',
		   KEY_UP = 'w', KEY_DOWN = 's', 
		   KEY_STOP = 'f', KEY_PAUSE = 'p'};

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

typedef struct food{
	int x;
	int y;
} food;

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

void addTail(struct snake_t *snake){
	snake->tsize++;
	if(snake->direction == LEFT){
		snake->tail[snake->tsize - 1].x = snake->tail[snake->tsize - 2].x + 1;
		snake->tail[snake->tsize - 1].y = snake->tail[snake->tsize - 2].y;
	} else if (snake->direction == RIGHT){
		snake->tail[snake->tsize - 1].x = snake->tail[snake->tsize - 2].x - 1;
		snake->tail[snake->tsize - 1].y = snake->tail[snake->tsize - 2].y;
	} else if (snake->direction == UP){
		snake->tail[snake->tsize - 1].x = snake->tail[snake->tsize - 2].x;
		snake->tail[snake->tsize - 1].y = snake->tail[snake->tsize - 2].y + 1;
	} else if (snake->direction == DOWN){
		snake->tail[snake->tsize - 1].x = snake->tail[snake->tsize - 2].x;
		snake->tail[snake->tsize - 1].y = snake->tail[snake->tsize - 2].y - 1;
	}
}

struct food initFood(){
	struct food food;
	food.x = rand() % (MAX_X-1);
	food.y = rand() % (MAX_Y-1);
}

void refreshFood(struct food *food){
	food->x = rand() % (MAX_X-1);
	food->y = rand() % (MAX_Y-1);
}

void printSnake(struct snake_t snake, struct food food){
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
	
	matrix[food.x][food.y] = '$';

    for (int j = 0; j < MAX_Y; ++j){
        for (int i = 0; i < MAX_X; ++i){
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
}

 _Bool haveEaten(struct snake_t snake, struct food food){
	return snake.x == food.x && snake.y == food.y;
 }

void printScore(struct snake_t snake){
	printf("\tSCORE: %d\n", snake.score);
}

void printPause(){
	printf("\tPAUSE\n");
}

void eatTail(struct snake_t *snake){
	for (int i = 0; i < snake->tsize; i++){
		if(snake->x == snake->tail[i].x &&
			snake->y == snake->tail[i].y){
				snake->score -= (snake->tsize - i);
				snake->tsize = i;
		}
	}
}

snake_t moveLeft(snake_t snake){
	for (int i = snake.tsize - 1; i > 0; i--){
		snake.tail[i] = snake.tail[i-1];
		}
	snake.tail[0].x = snake.x;
	snake.tail[0].y = snake.y;
	
	snake.x = snake.x - 1;	
	if (snake.x < 0){
		snake.x = MAX_X - 1;
		}
	return snake;
}

snake_t moveRight(snake_t snake){
	for (int i = snake.tsize - 1; i > 0; i--){
		snake.tail[i] = snake.tail[i-1];
		}
	snake.tail[0].x = snake.x;
	snake.tail[0].y = snake.y;
	
	snake.x = snake.x + 1;	
	if (snake.x == MAX_X - 1){
		snake.x = 0;
	}
	return snake;
}	

snake_t moveDown(snake_t snake){
	for (int i = snake.tsize - 1; i > 0; i--){
		snake.tail[i] = snake.tail[i-1];
		}
	snake.tail[0].x = snake.x;
	snake.tail[0].y = snake.y;
	
	snake.y = snake.y + 1;	
	if (snake.y == MAX_Y - 1){
		snake.y = 0;
	}
	return snake;
}	

snake_t moveUp(snake_t snake){
	for (int i = snake.tsize - 1; i > 0; i--){
		snake.tail[i] = snake.tail[i-1];
		}
	snake.tail[0].x = snake.x;
	snake.tail[0].y = snake.y;
	
	snake.y = snake.y - 1;	
	if (snake.y < 0){
		snake.y = MAX_Y - 1;
	}
	return snake;
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

void move(snake_t *snake){
	switch (snake->direction){
		case UP:
			*snake = moveUp(*snake);
			break;
		case DOWN:
			*snake = moveDown(*snake);
			break;
		case LEFT:
			*snake = moveLeft(*snake);
			break;
		case RIGHT:
			*snake = moveRight(*snake);
			break;
	}
}
	
int main(){
	struct snake_t snake = initSnake( 10, 5, 3);
	struct food food = initFood();
	char key;
	while(key != KEY_STOP){
        system("cls");
		if(kbhit()){
			key = tolower(getch());
			changeDirection(&snake, key);
		}
		if(key != KEY_PAUSE){
			move(&snake);	
			if(haveEaten(snake, food)){
				addTail(&snake);
				snake.score++;
				if(DELAY > 50)
					DELAY = START_DELAY - 25*snake.score;
				refreshFood(&food);
			}
			eatTail(&snake);
			printScore(snake);
		}else {
			printPause();
		}	
			printSnake(snake, food);
			Sleep(DELAY); 
	}
	system("cls");
	printScore(snake);
	printf("\tGAME OVER\n");
	return 0;
}