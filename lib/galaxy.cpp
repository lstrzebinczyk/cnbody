#include <Gosu/Gosu.hpp>
#include <cmath>
#include "star.cpp"
// #include "vector.cpp"
#include <iostream>
// #include <omp.h>
#include <time.h>

class Galaxy{
  public:

  int size;
  int stars_count;
  float gravity_constant;
  Star **stars;
  float **a_x_matrix;
  float **a_y_matrix;

  Galaxy(int _stars_count){
    stars_count = _stars_count;
    size = 1000;
    gravity_constant = 0.0001;

    srand(time(NULL));

    stars = new Star*[stars_count];

    for(int i = 0; i < stars_count; i++){
      float random_x = 1.0 * size * rand() / RAND_MAX;
      float random_y = 1.0 * size * rand() / RAND_MAX;
      float random_mass = 10000 * sqrt(-2 * log(1.0 * rand() / RAND_MAX)) * sin(3.1415926 * rand() / RAND_MAX);

      if(random_mass < 0){
        random_mass *= -1;
      }

      std::cout << random_mass << "\n";

      stars[i] = new Star(random_x, random_y, random_mass);
    }

    std::cout << "Initialised galaxy with " << stars_count << " stars\n";
  }

  Galaxy(){
  }

  ~Galaxy(){
  }

  void draw(Gosu::Graphics &graphics){
    for(int i = 0; i < stars_count; i++){
      stars[i]->draw(graphics);
    }
  }

  void init_matrices(){
    a_x_matrix = new float*[stars_count];
    a_y_matrix = new float*[stars_count];

    #pragma omp parallel for num_threads(200)
    for(int i = 0; i < stars_count; ++i){
      a_x_matrix[i] = new float[stars_count];
      a_y_matrix[i] = new float[stars_count];
    }

    #pragma omp parallel for num_threads(200)
    for(int i = 0; i < stars_count; ++i){
      #pragma omp parallel for num_threads(200)
      for(int j = 0; j < stars_count; ++j){
        a_x_matrix[i][j] = 0;
        a_y_matrix[i][j] = 0;
      }
    }
  }

  void update_matrices(){
    #pragma omp parallel for num_threads(200) //shared(a_x_matrix, a_y_matrix)
    for(int i = 0; i < stars_count; ++i){
      #pragma omp parallel for num_threads(200)
      for(int j = i+1; j < stars_count; ++j){
        update_forces_with_matrices(i, j);
      }
    }
  }

  void update_forces(){
    #pragma omp parallel for num_threads(200)
    for(int i = 0; i < stars_count; ++i){
      float x_force = 0;
      float y_force = 0;

      for(int j = 0; j < stars_count; ++j){
        x_force += a_x_matrix[i][j];
        y_force += a_y_matrix[i][j];
      }

      stars[i]->update_acceleration(Vector(x_force, y_force));
    }
  }

  void remove_matrices(){
    #pragma omp parallel for num_threads(200)
    for(int i = 0; i < stars_count; ++i){
      delete a_x_matrix[i];
      delete a_y_matrix[i];
    }

    delete a_x_matrix;
    delete a_y_matrix;
  }

  void calculate_forces(){
    init_matrices();
    update_matrices();
    update_forces();
    remove_matrices();
  }

  void add_to_matrix(int i, int j, float x, float y){
    a_x_matrix[i][j] -= x;
    a_x_matrix[j][i] += x;
    a_y_matrix[i][j] -= y;
    a_y_matrix[j][i] += y;
  }

  void update_forces_with_matrices(int i, int j){
    Star *star_1 = stars[i];
    Star *star_2 = stars[j];

    float distance = star_1->distance_to(star_2);

    if(distance > star_1->size + star_2->size){
      float force = force_between(star_1, star_2, distance);
      float force_x = force * (star_1->position.a - star_2->position.a);
      float force_y = force * (star_1->position.b - star_2->position.b);
      add_to_matrix(i, j, force_x, force_y);
    }
  }

  float force_between(Star *star_1, Star *star_2, float distance){
    return gravity_constant * star_1->mass * star_2->mass / (distance * distance);
  }

  void move(){
    #pragma omp parallel for
    for(int i = 0; i < stars_count; i++){
      stars[i]->move();
    }
  }
};

