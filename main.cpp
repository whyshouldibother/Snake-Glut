#include <GL/freeglut.h>
#include <iostream>
#include <random>
#include <cstdlib>
#include <ctime>
clock_t lastInputTime = 0;
using namespace std;
// Randomizer
random_device rd;
mt19937 mt(rd());
// Return coordinates

int randGen(int max, int blockSize)
{
    uniform_int_distribution<int> dist(0, max / blockSize - 1);
    return (dist(rd) * blockSize);
}
class entity
{
public:
    //(x,y) = coordinates
    int x;
    int y;

    //(directionX, directionY) = direction vector
    int directionX = 0;
    int directionY = 0;

    entity() {};
    entity(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    // Check for walls
    bool check(int screenWidth, int screenHeight)
    {
        if (x >= screenWidth)
        {
            x = -screenWidth;
            return false;
        }
        else if (x < -screenWidth)
        {
            x = screenWidth;
            return false;
        }
        if (y >= screenHeight)
        {
            y = -screenHeight;
            return false;
        }
        else if (y < -screenHeight)
        {
            y = screenHeight;
            return false;
        }
        return true;
    }

    // Update entity in refrence to direction
    void update(int blocksize)
    {
        x += blocksize * directionX;
        y += blocksize * directionY;
    }

    // Grow entity if food is eaten
    bool grow(entity *food, int maxX, int maxY, int blockSize, vector<entity> snake, int score)
    {
        if (x == food->x && y == food->y)
        {
            bool maxScore = score >= (maxX * maxY) / (blockSize * blockSize) - 1;
        foodGen: // Regenerate food
            food->x = randGen(maxX, blockSize);
            food->y = randGen(maxY, blockSize);
            if (!maxScore)
                for (int i = 0; i < snake.size(); i++)
                    if (snake[i].x == food->x && snake[i].y == food->y)
                        goto foodGen;
            return true;
        }
        else
            return false;
    }

    // Compare two parts of snake for collision
    bool operator==(entity compare)
    {
        return (x == compare.x && y == compare.y);
    }
};
struct rectParms
{
    int x, y, length, width;
};
struct Color
{
    float r, g, b, a;
};
void DrawRectangle(int x, int y, int w, int h, Color color)
{

    glBegin(GL_QUADS);
    glColor3f(color.r, color.g, color.b);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();
}
void drawText(float x, float y, const char *text, void *font = GLUT_BITMAP_HELVETICA_18)
{
    glColor3f(1.0, 1.0, 1.0); // White text
    glRasterPos2f(x, y);      // Set text position
    for (const char *c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}

vector<entity> snake;
entity food(randGen(300, 10), randGen(300, 10));
bool gameOver = false;
Color snakeHead = {1, 1, 1, 1}, snakeBody = {0.9, 0.9, 0.9, 1}, foodColor = {0.15, 0.15, 0.15, 1};
void DrawBody(vector<entity> snake, int start, Color color)
{
    int offset[4];
    for (int i = start; i < snake.size() - 1; i++)
    {

        for (int j = 0; j < 4; j++)
            offset[j] = 0;
        if (snake[i - 1].y == snake[i + 1].y)
        {
            offset[2] = 1;
            offset[3] = 2;
        }
        else if (snake[i - 1].x == snake[i + 1].x)
        {
            offset[0] = 1;
            offset[1] = 2;
        }
        else if ((snake[i + 1].directionX == 1 && snake[i].directionY == 1) || (snake[i + 1].directionY == -1 && snake[i].directionX == -1))
        {
            offset[1] = 1;
            offset[2] = 1;
        }
        else if ((snake[i + 1].directionX == 1 && snake[i].directionY == -1) || (snake[i + 1].directionY == 1 && snake[i].directionX == -1))
        {
            offset[1] = 1;
            offset[3] = 1;
        }
        else if ((snake[i + 1].directionY == 1 && snake[i].directionX == 1) || (snake[i + 1].directionX == -1 && snake[i].directionY == -1))
        {
            offset[0] = 1;
            offset[3] = 1;
        }
        else if ((snake[i + 1].directionY == -1 && snake[i].directionX == 1) || (snake[i + 1].directionX == -1 && snake[i].directionY == 1))
        {
            offset[0] = 1;
            offset[2] = 1;
        }
        DrawRectangle(snake[i].x + offset[0], snake[i].y + offset[2], 10 - offset[1], 10 - offset[3], snakeBody);
    }
    if (snake.size() > 1)
    {
        for (int j = 0; j < 4; j++)
            offset[j] = 1;
        if (snake[snake.size() - 1].directionX == 1)
        {
            offset[1] = 0;
        }
        else if (snake[snake.size() - 1].directionX == -1)
        {
            offset[0] = 0;
        }
        else if (snake[snake.size() - 1].directionY == 1)
        {
            offset[3] = 0;
        }
        else if (snake[snake.size() - 1].directionY == -1)
        {
            offset[2] = 0;
        }
        DrawRectangle(snake[snake.size() - 1].x + offset[0], snake[snake.size() - 1].y + offset[2], 10 - 2 * offset[1], 10 - 2 * offset[3], snakeBody);
    }
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw snake
    DrawBody(snake, 1, snakeBody);
    DrawRectangle(snake[0].x, snake[0].y, 10, 10, snakeHead);
    DrawRectangle(food.x, food.y, 10, 10, foodColor);
    if (gameOver)
        drawText(0, 0, "GameOver");
    glFlush();
    glutSwapBuffers();
}
void keyboard(int key, int, int)
{
    clock_t currentTime = clock();
    if ((currentTime - lastInputTime) < 100)
        return;
    else
    {
        switch (key)
        {
        case GLUT_KEY_UP:
            if (snake[0].directionY == 0)
            {
                snake[0].directionY = 1;
                snake[0].directionX = 0;
            }
            break;
        case GLUT_KEY_DOWN:
            if (snake[0].directionY == 0)
            {
                snake[0].directionX = 0;
                snake[0].directionY = -1;
            }
            break;
        case GLUT_KEY_LEFT:
            if (snake[0].directionX == 0)
            {
                snake[0].directionY = 0;
                snake[0].directionX = -1;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (snake[0].directionX == 0)
            {
                snake[0].directionY = 0;
                snake[0].directionX = 1;
            }
            break;
        }
        lastInputTime = currentTime;
    }
}
void gameLoop(int blocksize)
{
    if (!gameOver)
    {
        for (int i = snake.size() - 1; i > 0; i--)
            snake[i] = snake[i - 1];
        snake[0].update(blocksize);
        for (int i = 1; i < snake.size(); i++)
            if (snake[0] == snake[i])
            {
                gameOver = true;
                break;
            }
        if (snake[0].grow(&food, 300, 300, blocksize, snake, snake.size() - 1))
        {
            snake.push_back(entity(snake[snake.size() - 1].x, snake[snake.size() - 1].y));
        }
        glutPostRedisplay();
    }
    if (snake[0].check(300, 300))
        glutTimerFunc(100, gameLoop, blocksize);
    else
        gameOver = true;
}
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Snake");

    glClearColor(0, 0, 0, 1);
    glColor3f(0, 0, 0);
    glPointSize(1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-300, 300, -300, 300);

    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);
    snake.reserve(1000 * sizeof(entity));
    snake.push_back(entity(0, 0));

    glutTimerFunc(100, gameLoop, 10);
    glutMainLoop();
    return 0;
}