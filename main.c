#include <ncurses.h>
#include <stdlib.h>

#define KEY_ESC 27

// 座標を表す構造体
struct Point {
  int x;
  int y;
};

// スネークを表す構造体
struct Snake {
  struct Point *segments; // スネークの各セグメントの座標を格納する動的配列
  int length;             // スネークの長さ
  char direction;         // 'U' = 上, 'D' = 下, 'L' = 左, 'R' = 右
};

// フードを表す構造体
struct Food {
  struct Point position;  // フードの座標
};

// ゲームボードを表す構造体
struct Board {
  struct Snake snake;     // スネーク
  struct Food food;       // フード
  int width;              // ボードの幅
  int height;             // ボードの高さ
  int score;              // スコア
};

void initialize_board(struct Board *board, int width, int height);
void draw_board(const struct Board *board);
int update_snake_position(struct Board *board);

int main() {
  // ncursesの初期化
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  timeout(100); // 0.1秒ごとにgetch()を非ブロッキングで呼び出す

  // ゲームボードの初期化
  struct Board board;
  initialize_board(&board, 20, 20);

  draw_board(&board);

  int key;
  while((key = getch()) != KEY_ESC){
    // ゲームボードの描画
    nodelay(stdscr, FALSE);
    // 一時停止（キー入力を待つ）

    switch(key){
    case KEY_UP:
      board.snake.direction = 'U';
      break;
    case KEY_DOWN:
      board.snake.direction = 'D';
      break;
    case KEY_LEFT:
      board.snake.direction = 'L';
      break;
    case KEY_RIGHT:
      board.snake.direction = 'R';
      break;
    }

    int result = update_snake_position(&board);
    draw_board(&board);
    if(result < 0){
      endwin();
      free(board.snake.segments);
      return 0;
    }
  }

  // ncursesの終了処理
  endwin();

  // メモリの解放
  free(board.snake.segments);

  return 0;
}

int update_snake_position(struct Board *board){
  int move_x, move_y;
  switch(board->snake.direction){
  case 'R':
    move_x = 1;
    move_y = 0;
    break;
  case 'L':
    move_x = -1;
    move_y = 0;
    break;
  case 'U':
    move_x = 0;
    move_y = -1;
    break;
  case 'D':
    move_x = 0;
    move_y = 1;
    break;
  }
  board->snake.segments[0].x += move_x;
  board->snake.segments[0].y += move_y;
  // ゲームオーバー判定
  if((board->snake.segments[0].x >= (board->width - 1) || board->snake.segments[0].y >= (board->height - 1)) ||
     (board->snake.segments[0].x < 1 || board->snake.segments[0].y < 1) ||
     (board->snake.segments[0].x == board->snake.segments[board->snake.length].x && board->snake.segments[0].y == board->snake.segments[board->snake.length].y)){
    return -1;
  }
  int last_x = board->snake.segments[0].x;
  int last_y = board->snake.segments[0].y;

  for(int i=1;i<board->snake.length;i++){
    int temp_last_x = board->snake.segments[i].x;
    int temp_last_y = board->snake.segments[i].y;

    board->snake.segments[i].x = last_x;
    board->snake.segments[i].y = last_y;

    last_x = temp_last_x;
    last_y = temp_last_y;
  }
  return 0;
}

// ゲームボードの初期化関数
void initialize_board(struct Board *board, int width, int height) {
  // ボードのサイズを設定
  board->width = width;
  board->height = height;

  // スネークの初期化
  board->snake.length = 1;
  board->snake.segments = malloc(board->snake.length * sizeof(struct Point));
  board->snake.segments[0].x = width / 2;
  board->snake.segments[0].y = height / 2;
  board->snake.direction = 'R';

  // フードの初期化（とりあえず固定座標）
  board->food.position.x = width / 4;
  board->food.position.y = height / 4;

  // スコアの初期化
  board->score = 0;
}

// ゲームボードの描画関数
void draw_board(const struct Board *board) {
  clear();
  // ボードの境界を描画
  for (int y = 0; y < board->height; ++y) {
    for (int x = 0; x < board->width; ++x) {
      if (y == 0 || y == board->height - 1 || x == 0 || x == board->width - 1) {
	mvprintw(y, x, "#");
      }
    }
  }
  mvaddch(board->snake.segments[0].y, board->snake.segments[0].x, board->snake.direction);
  if(board->snake.length > 1){
    // スネークを描画
    for (int i = 1; i < board->snake.length; ++i) {
      mvprintw(board->snake.segments[i].y, board->snake.segments[i].x, "O");
    }
  }

  // フードを描画
  mvprintw(board->food.position.y, board->food.position.x, "*");
  // スコアを描画
  mvprintw(board->height, 0, "Score: %d", board->score);
  // 画面を更新
  refresh();
}
