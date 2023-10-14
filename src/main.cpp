#include <list>
#include <raylib.h>
#include <memory>
#include <random>

struct BoardPos
{
	int row;
	int col;
};

struct TokenData
{
	TokenData() = default;
	TokenData(const Rectangle& frame, bool empty ) :frame(frame), empty(empty){}
	~TokenData() = default;
	TokenData& operator=(const TokenData& other)= default;

	Rectangle frame{};
	bool empty;
};

class Token
{
public:
	Token() = default;
	~Token() = default;

	Token(Token& other)
	{
		data = std::move(other.data);
		row = other.row;
		col = other.col;
		pos = other.pos;
	}

	Token& operator =(Token& other)
	{
		data = std::move(other.data);
		row = other.row;
		col = other.col;
		pos = other.pos;
		return *this;
	}

	std::unique_ptr<TokenData> data{};
	int row;
	int col;
	Vector2 pos;
};

constexpr int TOKEN_WIDTH = 3;
constexpr int TOKEN_HEIGHT = 3;
constexpr float TOKEN_SIZE = 120;

Token board[TOKEN_WIDTH][TOKEN_HEIGHT];

BoardPos emptyToken{};

int randomInt(int start, int end)
{
	std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distr(start, end);
	return distr(generator);
}

void shuffleBoard()
{
	std::unique_ptr<TokenData> tokens[(TOKEN_WIDTH * TOKEN_HEIGHT) - 1];

	for(int row = 0; row < TOKEN_WIDTH; row++)
	{
		for(int col = 0; col < TOKEN_HEIGHT; col++)
		{
			if(board[row][col].data->empty)
				continue;

			tokens[TOKEN_WIDTH * row + col] = std::move(board[row][col].data);
		}
	}

	for(int i = TOKEN_WIDTH * TOKEN_HEIGHT - 2; i >=1; i--)
	{
		int r = randomInt(0,i);
		tokens[i].swap(tokens[r]);
	}


	for(int row = 0; row < TOKEN_WIDTH; row++)
	{
		for(int col = 0; col < TOKEN_HEIGHT; col++)
		{
			if( row == 2 && col ==2)
				continue;
			board[row][col].data = std::move(tokens[TOKEN_WIDTH * row + col]);
		}
	}
}

void drawBoard(const Texture& boardTexture)
{
	int row = 0;
	int col = 0;

	for(row = 0; row < TOKEN_WIDTH; row++)
	{
		
		for(col = 0; col < TOKEN_HEIGHT; col++)
		{
			//DrawLine(row, 0, TOKEN_WIDTH,0, BLACK);
			if(board[row][col].data->empty)
			{
				DrawRectangle(board[row][col].pos.x, board[row][col].pos.y, TOKEN_SIZE, TOKEN_SIZE, RED);
			}
			else
			{
				DrawTextureRec(boardTexture, board[row][col].data->frame,board[row][col].pos, WHITE );
			}

			DrawRectangleLines(board[row][col].pos.x, board[row][col].pos.y, TOKEN_SIZE, TOKEN_SIZE, BLACK);
		}
	}
}

void moveToken(KeyboardKey key)
{
	int offsetCol = 0;
	int offsetRow = 0;
	switch(key)
	{
		case KEY_DOWN:
			{
				offsetCol = -1;
			}
		break;
		case KEY_UP:
			{
				offsetCol = 1;
			}
		break;
		case KEY_RIGHT:
			{
				offsetRow = -1;
			}
		break;
		case KEY_LEFT:
			{
				offsetRow = 1;
			}
		break;
		default:
			break;
	}

	const BoardPos tokenPos = { emptyToken.row + offsetRow , emptyToken.col + offsetCol};
	if( tokenPos.row < 0 || tokenPos.row >= TOKEN_WIDTH ||
		tokenPos.col < 0 || tokenPos.col >= TOKEN_HEIGHT)
	{
		return;
	}

	auto& empty = board[emptyToken.row][emptyToken.col];
	auto& token = board[tokenPos.row][tokenPos.col];

	token.data.swap(empty.data);

	emptyToken.row = token.row;
	emptyToken.col = token.col;
}

int main ()
{
	InitWindow(360, 360, "Sliding Puzzle");
	SetTargetFPS(30);

	Camera2D camera{
		.offset = 0,
		.target = 0,
		.rotation = 0,
		.zoom = 1.0f,
	};

	auto texture = LoadTexture(ASSETS_PATH"homer-simpson.png");
	for(int row = 0; row < TOKEN_WIDTH; row++)
	{
		for(int col = 0; col < TOKEN_HEIGHT; col++)
		{
			//token data initialization
			std::unique_ptr<TokenData> token = std::make_unique<TokenData>(
				Rectangle{row * TOKEN_SIZE,col * TOKEN_SIZE, TOKEN_SIZE, TOKEN_SIZE},
				false
			);

			//Token initialization
			board[row][col].data = std::move(token);
			board[row][col].row = row;
			board[row][col].col = col;
			board[row][col].pos = Vector2{ row * TOKEN_SIZE, col * TOKEN_SIZE};
		}
	}

	board[TOKEN_WIDTH -1][TOKEN_HEIGHT-1].data->empty = true;

	emptyToken.col = board[TOKEN_WIDTH -1][TOKEN_HEIGHT-1].col;
	emptyToken.row = board[TOKEN_WIDTH -1][TOKEN_HEIGHT-1].row;

	shuffleBoard();

	while(!WindowShouldClose())
	{
		camera.zoom += (GetMouseWheelMove() * 0.05f);
		if (camera.zoom > 3.0f) camera.zoom = 3.0f;
		else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

		if(IsKeyPressed(KEY_RIGHT)) moveToken(KEY_RIGHT);
		if(IsKeyPressed(KEY_LEFT)) moveToken(KEY_LEFT);
		if(IsKeyPressed(KEY_DOWN)) moveToken(KEY_DOWN);
		if(IsKeyPressed(KEY_UP)) moveToken(KEY_UP);
		
		BeginDrawing();
		ClearBackground(WHITE);
		BeginMode2D(camera);

		drawBoard(texture);

		EndMode2D();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}