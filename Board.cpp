#include"Board.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//光标回到起点
void clear_screen_() {
    printf("\033[H");
}

Board::Board():e{static_cast<std::default_random_engine::result_type>(time(nullptr))},
u(1,boardSize-2){
    for(auto & subArray : m_table)
    {
        subArray.fill(Backguard);
        subArray.front() = Border;
        subArray.back() = Border;
    }
    m_table.front().fill(Border);
    m_table.back().fill(Border);
    setApple();
}

Board::~Board(){}

bool Board::isBorder(const Point& pos){
    if(pos.first != 0 
        && pos.second != 0 
        && pos.first != boardSize - 1
        && pos.second != boardSize - 1)
    {
        return false;
    }
    return true;
}

Point& Board::setApple(){
    bool full = true;
    for(size_t i = 1;(i != m_table.size() - 1)&&full;++i)
    {
        for(size_t j = 1;j !=  m_table[i].size() -1 ;++j)
        {
            if(m_table[i][j] == Backguard)
            {
                full = false;
                break;
            }
        }
    }
    
    if(full)
    {
        gameOver(true);
    }

    do{
        m_apple = Point(u(e),u(e));
    }while(get(m_apple) != Backguard);

    get(m_apple) = Apple;

    return m_apple;
}

Point& Board::apple(){
    return m_apple;
}

void Board::showAll(){
    static int last_size = -1;
    clear_screen_();
    char buffer[boardSize * boardSize * boardSize],*begin = buffer;
    if(last_size == -1){
        strcpy(begin,"\033[2J");
        begin+= strlen("\033[2J");  
        last_size = 1;  
    }
    for(size_t i {0};i < m_table.size();++i){//row
        if(i > 0){
            strcpy(begin,"\r\n");
            begin += strlen("\r\n");
        }
        for(size_t j = 0;j < m_table[i].size();++j){//col
            strcpy(begin,&ch[m_table[i][j]]);
            begin += 1;
        }
    }
    fwrite(buffer, begin - buffer, 1, stdout);
    fflush(stdout);
}
std::default_random_engine& Board::boardRandomEngine(){
    return e;
}

const Table& Board::table() const{
    return m_table;
}

int& Board::get(const Point& pos)
{
    return m_table[pos.first][pos.second];
}

void Board::gameOver(bool win)const
{
    if(win)
    {
        std::cout << "win";
    }
    else
    {
        std::cout << "lose";
    }

    while(getch() != 32)
    {}
    exit(0);
}

Point operator+(const Point& rp,const Point& lp)
{
    return Point(rp.first + lp.first , rp.second + lp.second);
}

bool operator==(const Point& rp,const Point& lp){
    return rp.first == lp.first && rp.second == lp.second;
}

char getch()
{
    char key = 0;
    if(read(STDIN_FILENO,&key,1) < 0)//终端输入文件描述符 读一个字符到key
    {
        return 0;
    }
    return key;
}
