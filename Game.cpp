#include <SDL2/SDL.h>
#include <iostream>

class Game
{

    struct Vector2
    {
        float x;
        float y;
    };
    const int PaddleH = 100;
    const int thickness = 15;
    float PaddleVel = 300.0f;

    public:
        Game();
        bool Initialize();
        void RunLoop();
        void Shutdown();

    private:
        void ProcessInput();
        void UpdateGame();
        void GenerateOutput();

        SDL_Window* mWindow;
        bool mIsRunning;
        SDL_Renderer* mRenderer;
        Vector2 mBallPos;
        Vector2 mPaddlePos;
        Vector2 mPaddlePos2;
        Uint32 mTicksCount;
        int mPaddleDir;
        int mPaddleDir2;
        Vector2 mBallVel;
        int mWinner;
};

int main(int argc, char** argv) {

    Game MyGame;
    bool Success = MyGame.Initialize();

    if (Success)
    {
        MyGame.RunLoop();
    }

    MyGame.Shutdown();

    return 0;
}

Game::Game()
{
    mWindow = nullptr;
    mIsRunning = true;
    mRenderer = nullptr;

    mBallPos   = {512.0f, 384.0f};  
    mPaddlePos = {25.0f, 384.0f};   
    mPaddlePos2 = {1024.0f - 25.0f, 384.0f};
    mBallVel = {-200.0f, 235.0f};

    mTicksCount = 0;
    mPaddleDir = 0;
    mPaddleDir2 = 0;
    mWinner = 0;
}

bool Game::Initialize()
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to Initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow(
        "Pong: Version 2.0",
        100,
        100,
        1024,
        768,
        0
    );

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    return true;
}

void Game::Shutdown()
{
    if (mWinner == 1)
    {
        std::cout << std::endl << "-------------PLAYER 2 WINS-------------" << std::endl;
        std::cout << "           Time Survived: " << mTicksCount/1000.0f << std::endl;
        std::cout << "-------------PLAYER 2 WINS-------------" << std::endl << std::endl;
    }
    else if (mWinner == 2)
    {
        std::cout << std::endl << "-------------PLAYER 1 WINS-------------" << std::endl;
        std::cout << "           Time Survived: " << mTicksCount/1000.0f << std::endl;
        std::cout << "-------------PLAYER 1 WINS-------------" << std::endl << std::endl;
    }
    else
    {
        std::cout << std::endl << "-------------GAME OVER-------------" << std::endl;
        std::cout << "           Time Survived: " << mTicksCount/1000.0f << std::endl;
        std::cout << "-------------GAME OVER-------------" << std::endl << std::endl;
    }

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                mIsRunning = false;
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        mIsRunning = false;
    }

    mPaddleDir = 0;
    if (state[SDL_SCANCODE_W])
    {
        mPaddleDir -= 1;
    }

    if (state[SDL_SCANCODE_S])
    {
        mPaddleDir += 1;
    }

    mPaddleDir2 = 0;
    if (state[SDL_SCANCODE_I])
    {
        mPaddleDir2 -= 1;
    }

    if (state[SDL_SCANCODE_K])
    {
        mPaddleDir2 += 1;
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
        ;

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    if (mPaddleDir != 0)
    {
        mPaddlePos.y += mPaddleDir * PaddleVel * deltaTime;

        if (mPaddlePos.y < (PaddleH/2.0f + thickness))
            mPaddlePos.y = PaddleH/2.0f + thickness;
        else if (mPaddlePos.y > (768.0f - PaddleH/2.0f - thickness))
            mPaddlePos.y = (768.0f - PaddleH/2.0f - thickness);
    }

    if (mPaddleDir2 != 0)
    {
        mPaddlePos2.y += mPaddleDir2 * PaddleVel * deltaTime;

        if (mPaddlePos2.y < (PaddleH/2.0f + thickness))
            mPaddlePos2.y = PaddleH/2.0f + thickness;
        else if (mPaddlePos2.y > (768.0f - PaddleH/2.0f - thickness))
            mPaddlePos2.y = (768.0f - PaddleH/2.0f - thickness);
    }

    mBallPos.x += mBallVel.x * deltaTime;
    mBallPos.y += mBallVel.y * deltaTime;

    if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
        mBallVel.y *= -1;

    if (mBallPos.y >= 768.0f - thickness && mBallVel.y > 0.0f)
        mBallVel.y *= -1;

    float ballLeft = mBallPos.x - thickness/2.0f;
    float ballRight = mBallPos.x + thickness/2.0f;

    float paddleLeft = mPaddlePos.x - thickness/2.0f;
    float paddleRight = mPaddlePos.x + thickness/2.0f;

    float diff = fabs(mBallPos.y - mPaddlePos.y);

    if (ballLeft <= paddleRight &&
        ballRight >= paddleLeft &&
        diff <= PaddleH/2.0f &&
        mBallVel.x < 0.0f)
    {
        mBallVel.x = -1.0f*mBallVel.x + 100.0f;
        PaddleVel = 1.0f * PaddleVel + 150.0f;
        mBallPos.x = paddleRight + thickness/2.0f;
    }

    float paddle2Left = mPaddlePos2.x - thickness/2.0f;
    float paddle2Right = mPaddlePos2.x + thickness/2.0f;

    float diff2 = fabs(mBallPos.y - mPaddlePos2.y);

    if (ballRight >= paddle2Left &&
        ballLeft <= paddle2Right &&
        diff2 <= PaddleH/2.0f &&
        mBallVel.x > 0.0f)
    {
        mBallVel.x = -1.0f*mBallVel.x - 50.0f;
        PaddleVel = 1.0f * PaddleVel + 25.0f;
        mBallPos.x = paddle2Left - thickness/2.0f;
    }

    mTicksCount = SDL_GetTicks();

    if (mBallPos.x <= thickness)
    {
        mWinner = 1;
        mIsRunning = false;
    }

    if (mBallPos.x >= 1024.0f - thickness)
    {
        mWinner = 2;
        mIsRunning = false;
    }
}

void Game::GenerateOutput()
{
    SDL_SetRenderDrawColor(
        mRenderer,
        0,
        0,
        0,
        255
    );

    SDL_RenderClear(mRenderer);

    SDL_SetRenderDrawColor(
        mRenderer,
        255,
        255,
        255,
        255
    );

    SDL_Rect topWall{
        0,
        0,
        1024,
        thickness
    };

    SDL_Rect bottomWall{
        0,
        768 - thickness,
        1024,
        thickness
    };

    SDL_Rect ball{
        static_cast<int>(mBallPos.x - thickness/2),
        static_cast<int>(mBallPos.y - thickness/2),
        thickness,
        thickness
    };

    SDL_Rect Paddle{
        static_cast<int>(mPaddlePos.x - thickness/2),
        static_cast<int>(mPaddlePos.y - PaddleH/2.0f),
        thickness,
        PaddleH
    };

    SDL_Rect Paddle2{
        static_cast<int>(mPaddlePos2.x - thickness/2),
        static_cast<int>(mPaddlePos2.y - PaddleH/2.0f),
        thickness,
        PaddleH
    };

    SDL_RenderFillRect(mRenderer, &topWall);
    SDL_RenderFillRect(mRenderer, &bottomWall);
    SDL_RenderFillRect(mRenderer, &ball);
    SDL_RenderFillRect(mRenderer, &Paddle);
    SDL_RenderFillRect(mRenderer, &Paddle2);

    SDL_RenderPresent(mRenderer);
}