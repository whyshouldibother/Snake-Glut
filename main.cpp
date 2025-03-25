#include <GL/freeglut.h>
#include <iostream>
#include <random>
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
            x = 0;
            return false;
        }
        else if (x < 0)
        {
            x = screenWidth;
            return false;
        }
        if (y >= screenHeight)
        {
            y = 0;
            return false;
        }
        else if (y < 0)
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
void rectangle(int x, int y, int w, int h)
{

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();
}
vector<entity> snake;
entity food(randGen(300, 10), randGen(300, 10));
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < snake.size(); i++)
        rectangle(snake[i].x, snake[i].y, 10, 10);
    rectangle(food.x, food.y, 10, 10);
    glFlush();
    glutSwapBuffers();
}
void keyboard(int key, int, int)
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
}
void gameLoop(int blocksize)
{
    rectangle(0, 0, 10, 10);
    for (int i = snake.size() - 1; i > 0; i--)
        snake[i] = snake[i - 1];
    snake[0].update(blocksize);
    glutPostRedisplay();
    glutTimerFunc(100, gameLoop, blocksize);
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
    snake.push_back(entity(0, 10));
    snake[0].directionX = 1;

    glutTimerFunc(100, gameLoop, 10);
    glutMainLoop();
    return 0;
}