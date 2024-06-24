#pragma once

#include <array>
#include <utility>
#include <random>
#include <ctime>
#include <iostream>

#include"termios.h"
#include"unistd.h"
// #include <sys/select.h>
// #include <sys/time.h>
#include <sys/ioctl.h>

constexpr int boardSize = 20;
constexpr int cooldown = 10;
constexpr int Border = 0,Body = 1,Apple = 2,Backguard = 3;

using Point = std::pair<int,int>;
using Table = std::array<std::array<int,boardSize>,boardSize>;

Point operator+(const Point&,const Point&);
bool operator==(const Point&,const Point&);
static struct termios old;
void clear_screen_();

class Board{
    public:
        Board();
        virtual ~Board();
        static bool isBorder(const Point& pos);
        Point& setApple();
        Point& apple();
        const Table& table() const;
        int& get(const Point& pos);
        void gameOver(bool win)const;
        void showAll();

        std::default_random_engine& boardRandomEngine();
    private:
        static inline char ch[4] = {'#','+','O',' '};
        Table m_table;
        Point m_apple;
       
        std::default_random_engine e;
        // std::mt19937 engine {static_cast<std::default_random_engine::result_type>(seed)};
        std::uniform_int_distribution<int> u;
};

//STDIN_FILENO STDOUT_FILENO是终端 io的文件描述符
static void __attribute__((constructor)) 
_termios_init(void){
    struct winsize win;

    //检查是否终端窗口
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == -1) {
        // printf("Not a terminal window.\n");
        exit(1);
    }

    if(tcgetattr(STDIN_FILENO,&old) < 0)
    {
        exit(1);
    }
    struct termios curr = old;
    curr.c_lflag &= ~ICANON;
    curr.c_lflag &= ~ECHO;
    curr.c_cc[VMIN] = 1;
    curr.c_cc[VTIME] = 0;
    if(tcsetattr(STDIN_FILENO,TCSANOW,&curr) < 0)
    {
        exit(1);
    }
}

static void __attribute__((destructor))
_termios_restore(void){
    if(tcsetattr(STDIN_FILENO,TCSANOW,&old) < 0)
    {
        exit(1);
    }
}

char getch();//读按键字符



