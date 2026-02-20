
#include "xonix.h"



int main()
{
   
    Game_view* sp = new Game_view(18, 4);
    Game game(sp->get_ptr());
    game.run();
    delete sp;


    return 0;
}
