/*
what's going on :(
particle thing? :3
*/

#include "raylib.h"
#include <iostream>
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <Eigen/Dense>

using Eigen::Vector2d;

class Particle {
  public:
    Vector2d _pos;
    Vector2d _vel;
    int _size = (rand() % 11) +10;
    long int _mass = pow(_size,0);
    Color _color = PURPLE;

   Particle(int posX, int posY, int Vel, int size) {
     srand(time(0));
     //std::cout << "mouse particle constructed" << std::endl;
     _pos(0)=posX;
     _pos(1)=posY;
     _size=size;
     _mass=(pow(_size,5))*100000;
     _vel = Vector2d(Vel, 0);
   }

   Particle() {
     srand(time(0));
     _pos(0)=(rand() % 6010)/10;
     _pos(1)=(rand() % 4010)/10;
     _vel = Vector2d(/*(rand() % 6) -3*/0, /*(rand() % 6) -3*/0);
     //_size = (rand() % 21) + 0;
     //_mass = _size*10;
     std::cout << "randParticle of mass" << _mass << " constructed" << std::endl;
   }

   void move() {
     _pos(0) += _vel(0);
     _pos(1) += _vel(1);

     //uncomment for particles to bounce off walls
     if (_pos(0) >= 800 - _size || _pos(0) <= _size) {
       _vel(0) *= -1;
       if (_pos(0)>=800-_size) _pos(0)=800-_size;
       if (_pos(0) <= _size) _pos(0) = _size;
     }

     if (_pos(1) >= 800 - _size || _pos(1) <= _size) {
       _vel(1) *= -1;
       if (_pos(1)>=800-_size) _pos(1)=800-_size;
       if (_pos(1) <= _size) _pos(1) = _size;
     }
   }

   void check_collision(Particle particle) {
     Vector2d normal = particle._pos - _pos;
     float distance = sqrt(pow(normal(0), 2)+pow(normal(1),2));

     if (distance <= (_size + particle._size)*1.01) {
       Vector2d v1 = _vel;
       Vector2d v2 = particle._vel;
       _color=RED;
       _pos+=-normal/10;
        //_vel-=normal/(10*particle._mass+_mass/particle._mass- _mass);

     }else{
       _color=PURPLE;
     }
   }

   void doGravity(Particle two, float gravConst) {
     int x_dist = two._pos(0) - _pos(0);
     int y_dist = two._pos(1) - _pos(1);

     Vector2d direction = Vector2d(two._pos(0), two._pos(1)) - Vector2d(_pos(0), _pos(1));
     //Vector2d direction = Vector2d(_pos(0), _pos(1)) - Vector2d(two._pos(0), two._pos(1));

     float distance = sqrt( (x_dist*x_dist) + (y_dist*y_dist) );

     if (distance==0) return;
     if (distance<=(_size+two._size)) return;

     float force = (gravConst*_mass*two._mass)/pow(distance, 2);

     Vector2d acceleration = (force / _mass) * direction.normalized();

     _vel(0)+=acceleration(0);
     _vel(1)+=acceleration(1);
   }

   void draw() {
     DrawCircle(_pos(0),_pos(1), _size, _color);
   }
};

int main ()
{
  int windowWidth = 800;
  int windowHeight = 800;

  int particleSpeed = 0;
  int particleSize = 1;

  float gravConst = 6.6743*(pow(10,-11));

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(windowWidth, windowHeight, "Particle Simulation");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

  //Create particle list
  std::vector<Particle> particles;

  //grid of particles, fun
  /*int count=0;
  for (int i = 50; i<750; i+=50) {
    for (int j=50; j<750; j+=50) {
      Particle newParticle{i, j, 0, 10};
      particles.push_back(newParticle);
      count++;
    }
  } std::cout << count << "PARTICLES";*/

  Camera2D camera = { 0 };
  camera.target = (Vector2){0, 0};
  camera.zoom = 1.0f;

  SetTargetFPS(60);
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
    //Input and stuff
    if (IsKeyDown(KEY_Q)) break;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          Particle newParticle{GetMousePosition().x, GetMousePosition().y,particleSpeed, particleSize};
          particles.push_back(newParticle);
    }

    //delete last particle
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
      if (IsKeyDown(KEY_E) && particles.size() >0) particles.pop_back();
      if (IsKeyDown(KEY_W)) {Particle newParticle{GetMousePosition().x, GetMousePosition().y, particleSpeed, particleSize};
        particles.push_back(newParticle);}
      
      if (IsKeyDown(KEY_D)) particleSpeed++;
      if (IsKeyDown(KEY_S)) particleSpeed--;

      if (IsKeyDown(KEY_C)) particleSize++;
      if (IsKeyDown(KEY_X)) particleSize--;
    }else{
    if (IsKeyPressed(KEY_E) && particles.size() > 0) particles.pop_back();
    if (IsKeyPressed(KEY_W)) {Particle newParticle{GetMousePosition().x, GetMousePosition().y, particleSpeed, particleSize};
        particles.push_back(newParticle);}

    if (IsKeyPressed(KEY_D)) particleSpeed++;
    if (IsKeyPressed(KEY_S)) particleSpeed--;

    if (IsKeyPressed(KEY_C)) particleSize++;
    if (IsKeyPressed(KEY_X)) particleSize--;
    }

    camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f));

    if (IsKeyPressed(KEY_J)) camera.zoom+=0.1;
    if (IsKeyPressed(KEY_K)) camera.zoom-=0.1;

    //Have time do things
    //defParticle.move();
    for (int i = 0; i < particles.size(); ++i) {
      particles[i].move();
    }

    //check particle collisions
    for (int i = 0; i < particles.size(); ++i) {
      for (int j = 0; j < particles.size(); ++j) {
        if (i!=j) particles[i].check_collision(particles[j]);
      } 
    }

    //gravity somehow
    for (int i=0; i < particles.size(); ++i) {
      for (int j = 0; j < particles.size(); ++j) {
        if (i!=j) particles[i].doGravity(particles[j], gravConst);
      }
    }

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

    BeginMode2D(camera);

		// draw some text using the default font
    DrawText(TextFormat("time (s): %f",GetTime()), 1, 1, 20, WHITE);
    DrawText(TextFormat("particles (e to erase): %0i", particles.size()),1, 20, 20, WHITE);
    DrawText(TextFormat("NewPSpeed (s-d): %0i", particleSpeed), 1, 40, 20, WHITE);
    DrawText(TextFormat("NewPSize (x-c): %0i", particleSize), 1, 60, 20, WHITE);

    //Draw whatever

    for (int j = 0; j < particles.size(); ++j) {
      particles[j].draw();
    }
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();

    //Debug space
    //std::cout << rand() << std::endl;
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
