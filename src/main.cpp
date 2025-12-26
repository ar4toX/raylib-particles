/*
what's going on :(
circle thing? :3
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
    Vector2d _vel;;;;;
    int _size = (rand() % 11) +10;
    long int _mass = pow(_size,10)/*1000000*/;
    Color _color = PURPLE;

   Particle(float posX, float posY, int Vel, int size) {
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
     /*if (_pos(0) >= 800 - _size || _pos(0) <= _size) {
       _vel(0) *= -1;
       if (_pos(0)>=800-_size) _pos(0)=800-_size;
       if (_pos(0) <= _size) _pos(0) = _size;
     }

     if (_pos(1) >= 800 - _size || _pos(1) <= _size) {
       _vel(1) *= -1;
       if (_pos(1)>=800-_size) _pos(1)=800-_size;
       if (_pos(1) <= _size) _pos(1) = _size;
     }*/
   }

   void check_collision(Particle particle) {
     int x_dist = _pos(0) - particle._pos(0);
     int y_dist = _pos(1) - particle._pos(1);
     float total_dist = sqrt((x_dist*x_dist) + (y_dist*y_dist));

     if (total_dist <= (_size + particle._size)) {
       Vector2d relVel = particle._vel -  _vel;
       Vector2d direction = particle._pos - _pos;

       _pos -= direction*0.1;

       Vector2d unitTangent(-direction(0), -direction(1));

       float velOneN = direction.dot(_vel);
       float velOneT = unitTangent.dot(_vel);

       float velTwoN = direction.dot(particle._vel);
       float velTwoT = unitTangent.dot(particle._vel);

       Vector2d velOneTAfterCollision = velOneT*unitTangent;
       Vector2d velTwoTAfterCollision = velTwoT*unitTangent;

       Vector2d velOneNAfterCollision = ((velOneN*(_mass-particle._mass)+2*particle._mass*velTwoN)/(_mass+particle._mass))*direction;
       Vector2d velTwoNAfterCollision = ((velTwoN*(particle._mass-_mass)+2*_mass*velOneN)/(_mass+particle._mass))*direction;

       _vel = velOneNAfterCollision + velOneTAfterCollision;
       std::cout << _vel << std::endl;
       std::cout << std::endl;
       //_vel += finalVel;
       
       _color = RED;
     }else{
       _color=PURPLE;
     }
   }

   void seekMouse(Vector2 mouse, float mass, float gravConst) {
     int x_dist = _pos(0) - mouse.x;
     int y_dist = _pos(1) - mouse.y;

     Vector2d direction = Vector2d(mouse.x, mouse.y) - Vector2d(_pos(0), _pos(1));

     float dist = sqrt((x_dist*x_dist)+(y_dist*y_dist));

     float force = (gravConst*_mass*mass)/(dist*dist);

     if (dist==0) return;
     
     Vector2d normalizedDirection = direction / dist;

     Vector2d acceleration = force / _mass * normalizedDirection;

     _vel(0)+=acceleration(0);
     _vel(1)+=acceleration(1);
   }

   void doGravity(Particle two, float gravConst) {
     int x_dist = two._pos(0) - _pos(0);
     int y_dist = two._pos(1) - _pos(1);

     Vector2d direction = Vector2d(two._pos(0), two._pos(1)) - Vector2d(_pos(0), _pos(1));
     //Vector2d direction = Vector2d(_pos(0), _pos(1)) - Vector2d(two._pos(0), two._pos(1));

     float distance = sqrt( (x_dist*x_dist) + (y_dist*y_dist) );

     if (distance==0) return;

     float force = (gravConst*_mass*two._mass)/pow(distance, 2);

     Vector2d acceleration = (force / _mass) * direction.normalized();

     _vel(0)+=acceleration(0);
     _vel(1)+=acceleration(1);
   }

   void teleport(Vector2 target) {
     _pos(0) = target.x;
     _pos(1) = target.y;
     _vel(0)=0;
     _vel(1)=0;
   }

   void draw() {
     DrawCircle(_pos(0),_pos(1), _size, _color);
   }
};

int main ()
{
  int windowWidth = 800;
  int windowHeight = 800;

  int planetSpeed = 3;
  int planetSize = 3;

  float gravConst = 6.6743*(pow(10,-11));

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(windowWidth, windowHeight, "Particle Simulation");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

  //Create particles
  std::vector<Particle> particles;

  //grid of particles, fun 
  /*for (int i = 50; i<750; i+=50) {
    for (int j=50; j<750; j+=50) {
      Particle newParticle{i, j, 0, 5};
      particles.push_back(newParticle);
    }
  }*/

  Camera2D camera = { 0 };
  camera.target = (Vector2){0, 0};
  camera.zoom = 1.0f;

  SetTargetFPS(60);
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
    //Input and stuff
    if (IsKeyDown(KEY_Q)) break;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_B) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          Particle newParticle{GetMousePosition().x, GetMousePosition().y, 0, 50};
          particles.push_back(newParticle);
      }
      if (IsKeyPressed(KEY_B)) {
          Particle newParticle{GetMousePosition().x, GetMousePosition().y,planetSpeed, planetSize};
          particles.push_back(newParticle);
      }
      std::cout << particles.size() << "th particle created" << std::endl;
    }

    

    //delete last particle
    if (IsKeyPressed(KEY_D) && particles.size() > 0) particles.pop_back();

    if (IsKeyPressed(KEY_S)) planetSpeed++;
    if (IsKeyPressed(KEY_A)) planetSpeed--;

    if (IsKeyPressed(KEY_X)) planetSize++;
    if (IsKeyPressed(KEY_Z)) planetSize--;

    camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f));

    if (IsKeyPressed(KEY_J)) camera.zoom+=0.1;
    if (IsKeyPressed(KEY_K)) camera.zoom-=0.1;

    //Have time do things
    //defParticle.move();
    for (int i = 0; i < particles.size(); ++i) {
      //std::cout << i << "particles" << std::endl;
      particles[i].move();
    }

    //check particle collisions
    /*for (int i = 0; i < particles.size(); ++i) {
      for (int j = 0; j < particles.size(); ++j) {
        if (i!=j) particles[i].check_collision(particles[j]);
      }
    }*/

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
    DrawText(TextFormat("time (s): %f",GetTime()), 1, 1, 20, PURPLE);
    DrawText(TextFormat("particles: %0i", particles.size()),1, 20, 20, PURPLE);
    DrawText(TextFormat("NewPSpeed: %0i", planetSpeed), 1, 40, 20, PURPLE);
    DrawText(TextFormat("NewPSize: %0i", planetSize), 1, 60, 20, PURPLE);

    //Draw whatever
    //DrawCircle(thing.posX, thing.posY, 25.0f, PURPLE);
    //defParticle.draw(PURPLE);

    for (int j = 0; j < particles.size(); ++j) {
      particles[j].draw();
    }
    //particleB.draw(GREEN);
    //if (particleCExists) particleC.draw(BLUE);		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();

    //Debug space
    //std::cout << rand() << std::endl;
	}

	// cleanup
	// unload our texture so it can be cleaned up
	//UnloadTexture(textureName);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
