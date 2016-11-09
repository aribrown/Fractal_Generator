// Fractal Drawer
//
// By: Ari Brown
//
#include<iostream>
#include<stdlib.h>
#include<SDL2/SDL.h>
#include<SDL2_image/SDL_image.h>
#include<cmath>
using namespace std;

const int SCREEN_WIDTH = 1300;
const int SCREEN_HEIGHT = 850;


bool init();
bool loadMedia();
void close();
SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// SDL FUNCTIONS ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// initializes SDL and the window
bool init()
{
        bool success = true;
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
                cout << "couldn't initialize sdl";
                success = false;
        }
        else {
                if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") ){
                        cout << "linear texture failed";
                }
                gWindow = SDL_CreateWindow( "Ari's Program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
                if ( gWindow == NULL ) {
                        cout << "no window";
                        success = false;
                }
                else {
                        // create renderer for window
                        gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED );
                        if (gRenderer == NULL) {
                                cout << "renderer couldn't be created";
                                success = false;
                        }
                        else {
                                // init renderer color
                                SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255);
                                SDL_RenderClear( gRenderer );
                                SDL_RenderCopy( gRenderer, gTexture, NULL, NULL);
                                if (!(IMG_Init(IMG_INIT_PNG))){
                                        cout << "couldn't initialize image";
                                        success = false;
                                }                         
                        }
                }
        }
        return success;
}
// loads background image
bool loadMedia()
{
        bool success = true;

        gTexture = loadTexture( "black.png");
        if (gTexture == NULL){
                success = false;
        }
        return success;
}
// closes SDL
void close()
{
        SDL_DestroyTexture(gTexture);
        gTexture = NULL;

        SDL_DestroyRenderer( gRenderer );
        SDL_DestroyWindow( gWindow );
        gWindow = NULL;
        gRenderer = NULL;
        

        IMG_Quit();
        SDL_Quit();

}
// loads the background image to the surface
SDL_Texture* loadTexture(std::string path)
{
        SDL_Texture* newTexture = NULL;
        SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
        if (loadedSurface ==NULL) {
                cout << "can't load image";
        }
        else
        {
                newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface);
                if (newTexture == NULL){
                        cout << "couldn't create texture";
                }
                SDL_FreeSurface(loadedSurface);
        }
        return newTexture;
}

static int rand_seed = -1;
void seed_random(int s)
{
        if (s>0){
                srand(s);
                rand_seed = s;
        }
}

// function used to generate a random integer between a range
int random_int(int lo, int hi)
{
        int range = hi - lo + 1;
        char *seed_str;
        if (rand_seed == -1 ) {
                seed_str = getenv("SNAKE_SEED");
                if (seed_str != NULL)
                        seed_random( (unsigned) atoi(seed_str) );
                else
                        seed_random( (unsigned) time(NULL) );
        }
        if ( range <= 0)
                return 17;
        return lo + ( rand() % range );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// CLASSES //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// contains color components
struct Color {
        unsigned short r;
        unsigned short g;
        unsigned short b;
        unsigned short a;
};

////////////////////////////////////////////////// Line /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


// the Line class takes in a (float) angle from the +x axis,
//                         a (float) length of the line,
//                         an (integer) for x coordinate,
//                         an (integer) for y coordinate,
//                         and a Color struct for line color
//
class Line {
        public:
                Line(); // default constructor
                Line(float a, float l, int start_x, int start_y, Color color);
                
                void draw_line(); // draws line
                
                // setters of private variables
                void set_color(Color color);
                void set_angle(float a);
                void set_length(float l);
                void set_position(int xx, int yy);
                void set_alpha(unsigned short a);
                
                // getters of private variables
                float get_length();
                float get_angle();
                int get_x();
                int get_y();
                unsigned short get_alpha();

        private:
                
                // calculates distance
                float pythag(int x1, int y1, int x2, int y2);

                // private variables
                Color c; 
                float angle;
                float length;
                int x;
                int y;
};
// default constructor
Line::Line(){}

// constructor takes in angle, length, x, y, and color
Line::Line(float a, float l, int start_x, int start_y, Color color)
{
        c.r = color.r;
        c.g = color.g;
        c.b = color.b;
        c.a = color.a;
        angle = a;
        length = l;
        x = start_x;
        y = start_y;
}

// draws line based on unit vector and length
void Line::draw_line()
{
        SDL_SetRenderDrawColor(gRenderer, c.r, c.g, c.b, c.a); // sets SDL to a draw color
        
        // gets directional components of line
        float dx = cos (angle * M_PI/180);
        float dy = -1*sin (angle * M_PI/180);

        // determins magnitude of the directional vector
        float mag = sqrt( pow(dx, 2) + pow(dy, 2) );
        
        // calculates unit vector in the direction
        float unit_x = dx/mag;
        float unit_y = dy/mag;
        
        // starts at the x and y point
        float current_x = (float)x;
        float current_y = (float)y;
        // increments x and y by unit vector and draws pixels until length is achieved
        while ((int)pythag(x, y, (int)current_x, (int)current_y) <= length)
        {
                SDL_RenderDrawPoint (gRenderer, current_x, current_y);
                current_x += unit_x;
                current_y += unit_y;
        }
}

// returns the length of the line
float Line::pythag(int x1, int y1, int x2, int y2)
{
      return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));  
}

//////////////////////// setters /////////////////////////
void Line::set_color(Color color)
{
        c.r = color.r;
        c.g = color.g;
        c.b = color.b;
        c.a = color.a;
}
void Line::set_angle(float a)
{
        angle = a;
}
void Line::set_length(float l)
{
        length = l;
}
void Line::set_position(int xx, int yy)
{
        x = xx;
        y = yy;
}
void Line::set_alpha(unsigned short a)
{
        c.a = a;
}

//////////////////////// getters /////////////////////////
float Line::get_angle()
{
        return angle;
}
float Line::get_length()
{
        return length;
}
int Line::get_x()
{
        return x;
}
int Line::get_y()
{
        return y;
}
unsigned short Line::get_alpha()
{
        return c.a;
}
////////////////////////////////////////////////// Koch /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


// Koch curve fractal, each iteration is as follows:
//
// take a line: 
//
// -----------------
//
//
// replace with:
//
//         /\
//        /  \
// -------    ------
// 
// (each angle is 60 degrees)
//
class Koch {
        public:
                // constructor and destructor
                Koch(); // default
                ~Koch();

                // The Koch curve can be initialized with:
                //                              float length,
                //                              float angle,
                //                              integer x,
                //                              integer y,
                //                              Color color,
                //                              integer iterations,
                //                              float angle_constant (angle is 60 for equilateral iterations)
                //
                Koch(float l, float a, int xpos, int ypos, Color col, int iterations, float a_const);
                
                // recursively repeats line replacement n number of times
                int recursion(int n);

                // prints the fractal
                void print();

                // setters
                void set_angle(float a);
                void set_length(float l);
                void set_angle_const(float a);
                void set_position(int xx, int yy);
                void set_iterations(int n);
                void set_pi(float p);
                void set_color(Color col);
                
                // getters
                float get_angle();
                float get_pi();
                float get_length();
                float get_angle_const();
                int get_x();
                int get_y();
                int get_iterations();
                Color get_color();
                
                // reinitializes private variables,
                // deletes entire array of lines, in order to 
                // produce a new array
                // (used if variables change in main loop, animation)
                void reinitialize();

        private:
                
                // private variables
                float angle;
                float length;
                int x, y;
                Color color;
                int angle_const;
                int iterations;
                float pi; // initialized to M_PI but can be changed (it's fun)

                // function splits each line into four new lines,
                // updates array of lines by deleting old lines and
                // replacing with four new smaller lines
                // (as shown in Koch curve figure)
                void make_four();

                // dynamic array of lines
                Line *lines;
                int cap;
                int num;

};

// dfault constructor
Koch::Koch(){}

// constructor with length, angle, x, y, color, iterations, and geometric fractal angle
Koch::Koch(float l, float a, int xpos, int ypos, Color col, int its, float a_const)
{
        pi = M_PI;
        angle = a;
        length = l;
        x = xpos - l/2;
        y = ypos;
        color = col;
        angle_const = a_const;
        Line initial(angle, length, x, y, color);
        // make first node with line
        
        cap = 10;
        num = 0;
        lines = new Line[cap];
        lines[num] = initial;
        num++;
        iterations = its;
        recursion(iterations);
}

// destructor
Koch::~Koch()
{
//        delete []lines;
}

// recursively makes four lines out of one line, n iterations
int Koch::recursion(int n)
{
        
        if (n == 0) {return n;}
        
        make_four();

        recursion(n - 1);

}

void Koch::make_four()
{
        // temporary new array of lines, capacity four times as big (4x as many lines will be generated)
        Line *lines2 = new Line[cap*4];
        
        int num2 = 0;  // number of Lines used in new array

        for (int i = 0; i < num; i++) // loop through old array
        {
                Line cline = lines[i]; // for each line
                
                //////////////////////////////////////////////////////////////////////////////////////////////////
                ///// generate four lines based on the line given, these lines are each geometrically defined   
                //              by the nature of the fractal:                                                   
                                                                                                                
                Line temp1(cline.get_angle(), cline.get_length()/4, cline.get_x(), cline.get_y(), color);       
                                                                                                                
                int x2 = cline.get_x() + ((cline.get_length()/4) * cos(cline.get_angle() * pi/180));            
                int y2 = cline.get_y() + ((cline.get_length()/4) * sin(cline.get_angle() * pi/180));            
                Line temp2(cline.get_angle() + angle_const, cline.get_length()/4, x2, y2, color);               
                if (i >= num/2)                                                                                 
                {                                                                                               
                        temp2.set_angle(cline.get_angle() - angle_const);                                       
                }                                                                                               
                                                                                                                
                int x3 = cline.get_x() + (2*(cline.get_length()/4) * cos(cline.get_angle() * pi/180));          
                int y3 = cline.get_y() + (2*(cline.get_length()/4) * sin(cline.get_angle() * pi/180));          
                Line temp3(cline.get_angle() + (2*angle_const), cline.get_length()/4,  x3, y3, color);          
                if (i >= num/2)                                                                                 
                {                                                                                               
                        temp3.set_angle(cline.get_angle() - (2*angle_const));                                   
                }                                                                                               
                                                                                                                
                Line temp4(cline.get_angle(), cline.get_length()/4, x3, y3, color);                             
                                                                                                                
                //////////////////////////////////////////////////////////////////////////////////////////////////

                // populate new array with four lines generated
                lines2[num2] = temp1;
                lines2[num2+1] = temp2;
                lines2[num2+2] = temp3;
                lines2[num2+3] = temp4;
                num2 += 4; // update count
        }
        delete []lines; // delete old lines
        num = num2; // update number of lines
        lines = lines2; // reassign array pointer
        cap = cap*4; // update capacity
}

// prints line by drawing each line
void Koch::print()
{
        for (int i = 0; i < num; i++)
        {
                lines[i].draw_line();
        } 
}

// deletes array of lines in order to create a new
// array in animation
void Koch::reinitialize()
{
        delete []lines;
        Line initial(angle, length, x, y, color);
        
        cap = 10;
        num = 0;
        lines = new Line[cap];
        lines[num] = initial;
        num++;
        recursion(iterations);
}

//////////////////////// setters /////////////////////////
void Koch::set_angle(float a)
{
        angle = a;
}
void Koch::set_length(float l)
{
        length = l;
}
void Koch::set_angle_const(float a)
{
        angle_const = a;
}
void Koch::set_position(int xx, int yy)
{
        x = xx;
        y = yy;
}
void Koch::set_iterations(int n)
{
        iterations = n;
}
void Koch::set_pi(float p)
{
        pi = p;
} 
void Koch::set_color(Color col)
{
        color = col;
}


//////////////////////// getters /////////////////////////
float Koch::get_angle()
{
        return angle;
}
float Koch::get_length()
{
        return length;
}
float Koch::get_angle_const()
{
        return angle_const;
}
int Koch::get_x()
{
        return x;
}
int Koch::get_y()
{
        return y;
}
int Koch::get_iterations()
{
        return iterations;
}
float Koch::get_pi()
{
        return pi;
}
Color Koch:: get_color()
{
        return color;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// MAIN IMPLEMENTATION //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char* args[])
{

        if (!init()){
                cout << "couldn't init";
        }
        else{
                if( !loadMedia() ) {
                        cout << "failed to load media";
                }               
                else {

                        bool quit = false;
                        SDL_Event e;

                        ///////////// INITIAL VARIABLES //////////////
                        int GROWTH = 10;
                        bool right = true;
                        int ITERATIONS = 0;
                        int LENGTH = 80;
                        int SMALL = 60;
                        int LARGE = 1100;
                        bool grow = true;
                        int RATE = 4;
                        bool color_increment = false;
                        ///////////// CONTENTS ///////////////////////
                        
                        Color c_koch = {50, 130, 20, 255};
                        Koch koch0((float)LENGTH, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, c_koch, ITERATIONS, 60); 
                        Koch koch1((float)LENGTH, 90, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, c_koch, ITERATIONS, 60);
                        Koch koch2((float)LENGTH, 180, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, c_koch, ITERATIONS, 60);
                        Koch koch3((float)LENGTH, 270, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, c_koch, ITERATIONS, 60);
                        Koch koch4((float)LENGTH, 45, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, c_koch, ITERATIONS, 60); 
                        Koch koch5((float)LENGTH, 135, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, c_koch, ITERATIONS, 60);
                        Koch koch6((float)LENGTH, 225, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, c_koch, ITERATIONS, 60);
                        Koch koch7((float)LENGTH, 315, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, c_koch, ITERATIONS, 60);
                        
                        // populate array of koch curves
                        Koch koch[8];
                        koch[0] = koch0; koch[1] = koch1; koch[2] = koch2;
                        koch[3] = koch3; koch[4] = koch4; koch[5] = koch5;
                        koch[6] = koch6; koch[7] = koch7;

                        ///////////// TIMER //////////////////////////
                        int timer = 0;
                        

                        ///////////// RUN LOOP ///////////////////////
                        while (!quit)
                        {
                                SDL_RenderCopy( gRenderer, gTexture, NULL, NULL);
                                
                                // manages interface events through SDL
                                while (SDL_PollEvent(&e) != 0)
                                {
                                        if (e.type == SDL_QUIT){
                                                quit = true;
                                        }
                                        else if ( e.type == SDL_KEYDOWN ) 
                                        {
                                                switch(e.key.keysym.sym)
                                                {
                                                        case SDLK_RIGHT:
                                                                break;

                                                        case SDLK_LEFT:
                                                                break;

                                                        case SDLK_UP:
                                                                break;

                                                        case SDLK_DOWN:
                                                                break;

                                                }
                                        }
                                }


                                ///////////// OBJECT RENDER ///////////////////
                                
                                if (timer % 10 == 0) // rate of animation 
                                {
                                        // boolean motion right or left
                                        if (koch[0].get_x() > SCREEN_WIDTH - LENGTH) {right = !right;}
                                        if (koch[0].get_x() < LENGTH) {right = !right;}
                                        
                                        // boolean grow or shrink
                                        if ((int)koch[0].get_length() > LARGE) {grow = !grow;}
                                        if ((int)koch[0].get_length() < SMALL) {grow = !grow;}
                                        
                                        // 
                                        for (int i = 0; i < 8; i++) 
                                        {
                                                if (timer % 1500 == 0 && timer > 100)
                                                {
                                                        if (koch[i].get_iterations() > 4) {
                                                                koch[i].set_iterations(0);
                                                        }
                                                        koch[i].set_iterations(koch[i].get_iterations() + 1);
                                                }

                                                // implements the boolean values
                                                if (grow) {koch[i].set_length(koch[i].get_length() + GROWTH);}
                                                if (!grow) {koch[i].set_length(koch[i].get_length() - GROWTH);}
                                                if (right){koch[i].set_position(koch[i].get_x()+1, koch[i].get_y());}
                                                if (!right){koch[i].set_position(koch[i].get_x()-1, koch[i].get_y());}
                                               
                                                // rotates
                                                koch[i].set_angle(koch[i].get_angle() + 1);
                                                koch[i].set_angle_const(koch[i].get_angle_const() + 1);
                                                koch[i].reinitialize();
                                                koch[i].set_pi(RATE); // if pi is increased, the rate of spinning increases (idk why)
                                        }
                                } 

                                // print all the lines
                                for (int i = 0; i < 8; i++)
                                {
                                        koch[i].print();
                                }

                                // update the SDL screen
                                SDL_RenderPresent( gRenderer );

                                // increment the timer
                                timer++;
                                if (timer > 30000) {timer = 0;} // resets timer
                        }
                }
        }
        close();
        return 0;
}

