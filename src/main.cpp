#include "../include/raylib.h"
#include "../include/raymath.h"
#include <deque>
#include <iostream>

using namespace std;

int cellSize = 20;
int cellCount = 25;
int WINDOW_SIZE = cellSize * cellCount;

Color navy = {13, 16, 31, 255};
Color purple = {95, 117, 227, 255};

double lastUpdateTime = 0;

bool UpdateEvent(double delay) {
  double currentTime = GetTime();
  if (currentTime - lastUpdateTime >= delay) {
    lastUpdateTime = currentTime;
    return true;
  }
  return false;
}

bool ElementInDeque(Vector2 item, deque<Vector2> body) {
  for (int i = 0; i < body.size(); i++) {
    if (Vector2Equals(item, body[i])) {
      return true;
    }
  }
  return false;
}

class Snake {
public:
  deque<Vector2> body = {Vector2{5, 5}, Vector2{5, 6}, Vector2{5, 7}};
  Vector2 direction = Vector2{1, 0};
  bool addSegment = false;
  void Draw() {
    for (unsigned int i = 0; i < body.size(); i++) {
      Rectangle portion =
          Rectangle{body[i].x * (float)cellSize, body[i].y * (float)cellSize,
                    (float)cellSize, (float)cellSize};
      DrawRectangleRounded(portion, 0.2, 4, purple);
    }
  }
  void Update() {
    Vector2 updatedHead = Vector2Add(body[0], direction);
    if (!addSegment) {
      body.pop_back();
    } else {
      addSegment = false;
    }
    body.push_front(updatedHead);
  }
};

class Food {
public:
  Vector2 position;
  Texture2D texture;
  deque<Vector2> snakeBody;
  Food(deque<Vector2> snakeBody) {
    Image image = LoadImage("../Graphics/food.png");
    texture = LoadTextureFromImage(image);
    UnloadImage(image);

    position = getPosition();
  }

  ~Food() { UnloadTexture(texture); }

  void Draw() {
    DrawTexture(texture, cellSize * position.x, cellSize * position.y, purple);
  }

  Vector2 getPosition() {
    Vector2 potentialPos;
    bool isClear = false;
    while (!isClear) {
      float x = GetRandomValue(0, cellCount - 1);
      float y = GetRandomValue(0, cellCount - 1);
      potentialPos = Vector2{x, y};
      isClear = !ElementInDeque(potentialPos, snakeBody);
    }

    return potentialPos;
  }
  void Update() { position = getPosition(); }
};

class Game {
public:
  Snake snake = Snake();
  Food food = Food(snake.body);
  bool isRunning = true;

  void Draw() {
    snake.Draw();
    food.Draw();
  }

  void Update() {
    snake.Update();
    CollisionWithFood();
    Collisions();
  }

  void CollisionWithFood() {
    if (Vector2Equals(snake.body[0], food.position)) {

      food.Update();
      snake.addSegment = true;
    }
  }

  void Collisions() {
    deque<Vector2> headlessBody = snake.body;
    headlessBody.pop_front();

    if (ElementInDeque(snake.body[0], headlessBody)) {
      GameOver();
    }
    if (snake.body[0].x == cellCount || snake.body[0].x == -1) {
      GameOver();
    }
    if (snake.body[0].y == cellCount || snake.body[0].y == -1) {
      GameOver();
    }
  }
  void GameOver() {
    snake.body = {Vector2{5, 5}, Vector2{5, 6}, Vector2{5, 7}};
    food.position = food.getPosition();
    isRunning = false;
  }
};

int main() {
  InitWindow(WINDOW_SIZE, WINDOW_SIZE, "Snake Game");
  SetTargetFPS(60);

  Game game = Game();

  while (WindowShouldClose() == false) {
    BeginDrawing();
    ClearBackground(navy);

    if (game.isRunning) {
      if (UpdateEvent(0.2))
        game.Update();
    }

    if (IsKeyPressed(KEY_W) && game.snake.direction.y != 1) {
      game.snake.direction = Vector2{0, -1};
      game.isRunning = true;
    }
    if (IsKeyPressed(KEY_S) && game.snake.direction.y != -1) {
      game.snake.direction = Vector2{0, 1};
      game.isRunning = true;
    }
    if (IsKeyPressed(KEY_A) && game.snake.direction.x != 1) {
      game.snake.direction = Vector2{-1, 0};
      game.isRunning = true;
    }
    if (IsKeyPressed(KEY_D) && game.snake.direction.x != -1) {
      game.snake.direction = Vector2{1, 0};
      game.isRunning = true;
    }

    game.Draw();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
