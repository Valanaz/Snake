
#include <thread>
#include "Snake.h"
#include "Astar.h"

void testch();

int play()
{
    Board b;
    Snake s{b};
    Point action = Right;
    char c;
    b.showAll();
    while(1)
    {
        c = getch();
        switch (c)//屏蔽逆向
        {
        case 'w':
            if(action == Down){break;}
            action = Up;
            break;
        case 's':
            if(action == Up){break;}
            action = Down;
            break;
        case 'd':
            if(action == Left){break;}
            action = Right;
            break;
        case 'a':
            if(action == Right){break;}
            action = Left;
            break;
        default:
            break;
        }
        s.move(action);
        b.showAll();
    }
    return 0;
}

int autoPlay(){
    duration_t dt(100);
    Board b;
    Snake s{b};
    Astar _astar(b,s);
    b.showAll();
    while(1){//可以做一个进度判定的死循环条件 例如snake占地比例等
        if(_astar.analysePath() && _astar.fakeSnakeSafe()){
            while(s.move(_astar.getStep())){
                b.showAll();
                break;
            }
        }
        else{
            // _astar.printStat();
            s.move(_astar.wander()); 
            b.showAll();
        }
        std::this_thread::sleep_for(dt);
    }
    return 0;
}

int main(){
    // play();
    autoPlay();
    return 0;
}

void testch()
{
    char c;
    while(1)
    {
        c = getch();
        std::cout << "key press: " << c << " ascii:" << int(c) <<std::endl;
    }

}
