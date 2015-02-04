//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

//paddle width and height and y location
#define PADDLEW 50
#define PADDLEH 10
#define PADDLEY (HEIGHT - 60)


// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // number of points initially
    int points = 0;
    
    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;
  
    // this must be outside of the following "forever" loop
    //velocity for x and y
    double dx = drand48() + 3.0;
    double dy = drand48() + 3.0;
    
    // wait for click before exiting
    waitForClick();
    
    // keep playing until game over
    while (lives > 0 && points < bricks)
    {
        // TODO
        // listen to mouse
        GEvent event = getNextEvent(MOUSE_EVENT);
        
        //IF WE hear an event
        if (event != NULL)
        {
            //was it movement?
            if(getEventType(event) == MOUSE_MOVED)
            {
                //the nex x is going to be this x position
                double x = getX(event);
                
                //don't let it pass the WIDTH
                //if it does, set it to this
                if (x + PADDLEW > WIDTH)
                {
                    x = WIDTH - PADDLEW;
                }
                
                //new location is at new x but same y
                setLocation(paddle, x, PADDLEY);
            }
        }
        
        //ball move
        move(ball, dx, dy);
        
        //BOUNCE off right
        if (getX(ball) + getWidth(ball) >= WIDTH)
        {
            dx = -dx;
        }

        // bounce off left edge of window
        else if (getX(ball) <= 0)
        {
            dx = -dx;
        }
        //if it hits the bottom, subtract a life, reset ball
        else if (getY(ball) + getHeight(ball) >= HEIGHT)
        {
            lives--;
            removeGWindow(window, ball);
            ball = initBall(window);
            if (lives > 0)
            {
            waitForClick();
            }
        }
        //bounce off top
        else if (getY(ball) <= 0)
        {
            dy = -dy;
        }
        
        GObject obj = detectCollision(window, ball);

        //if hit paddle
        if (obj != NULL)
        {
            if (obj == paddle)
            {
                //change y direction
                dy = -dy;
            }
            //hit a brick? bounce, add point, and remove brick
            else if (strcmp(getType(obj), "GRect") == 0)
            {
                dy = -dy;
                removeGWindow(window, obj);
                points++;
                updateScoreboard(window, label, points);
            }
         }    
        
        pause(10);
        
    }

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    //standard brick width and height
    int brickW = 30;
    int brickH = 10;
    //standard space between bricks; can be used for x and y
    int spaces = 9;
    
    for(int i = 0; i < ROWS; i++)// for each row
    {
        //similar to how I implemented x. See that for more detail below
        //this starts at y=60 rather than spaces. We want some more head room
        int y = 60 + i *(brickH + spaces);
        
        for(int j = 0; j < COLS; j++)//for each column
        {
            //each new brick must be (for x) "spaces" away from the front plus however many bricks
            //there are plus however many "spaces" px we need between each.
            int x = spaces + j*(brickW + spaces);
            
            GRect brick = newGRect(x, y, brickW, brickH);
            
            //color will be decided by i using switch
            switch(i)
            {
                case 0:
                setColor(brick, "RED");
                break;
                
                case 1:
                setColor(brick, "ORANGE");
                break;
                
                case 2:
                setColor(brick, "YELLOW");
                break;
                
                case 3:
                setColor(brick, "GREEN");
                break;
                
                //anything 4 and up will be blue so ROWS can be > 5
                default:
                setColor(brick, "BLUE");
                break;
                
            }
            
            setFilled(brick, true); //filled
            add(window, brick);
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(WIDTH/2 - RADIUS, HEIGHT/2 - RADIUS, RADIUS * 2, RADIUS * 2);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // make a rectangle with x in the middle, Y at PADDLEY and use paddle w and h
    GRect paddle = newGRect(WIDTH/2 - PADDLEW/2, PADDLEY, PADDLEW, PADDLEH);

    //set its color to black and fill it
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    
    
    //add it to our board/window
    add(window, paddle);
    
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    int points = 0;
    GLabel scoreLabel = newGLabel("");
    setFont(scoreLabel, "SansSerif-36");
    add(window, scoreLabel);
    updateScoreboard(window, scoreLabel, points);
    return scoreLabel;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
