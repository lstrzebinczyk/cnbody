#include <Gosu/Gosu.hpp>
#include "lib/galaxy.cpp"
// #include <iostream>

class GameWindow : public Gosu::Window
{
  Galaxy* galaxy;
  int counter;

  public:
  
  GameWindow() : Window(1000, 1000, false)
  {
    counter = 0;
    galaxy = new Galaxy(3500);
    setCaption(L"Gosu Tutorial Game"); 
  }

  void update()
  {
    galaxy->calculate_forces();
    galaxy->move();
    counter++;
    if(counter == 20){
      exit(1);
    }
  }

  void draw()
  {
    graphics().pushTransform(Gosu::scale(0.2));
    graphics().pushTransform(Gosu::translate(2000, 2000));
    galaxy->draw(graphics());
    graphics().popTransform();
  }
};

int main() 
{
  GameWindow window;
  window.show(); 
}