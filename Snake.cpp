#include "Snake.h"

Snake::Snake(Board& board):m_board{board}
{
    m_body.push_back(Point(boardSize/2,boardSize/2));
    m_board.get(m_body.front()) = Body;
}

Snake::~Snake(){}

bool Snake::move(const Point& step ){
    if(!(step == Left || step == Right || step == Up || step == Down)){
        return false;
    }

    // if(cd())
    {
        //下一个踩踏点
        Point next_pos(m_body.back().first + step.first,m_body.back().second + step.second);
        bool grow = false;
        if(m_board.get(next_pos) == Apple)
        {
            m_board.setApple();
            grow = true;
        }
        //踩踏点是边界或者body，则结束
        if(m_board.get(next_pos) == Body || m_board.get(next_pos) == Border){
            m_board.gameOver(false);
        }

        //没吃，则切掉尾巴(尾巴是链表头)
        Point last_tail(0,0);
        if(!grow)
        {
            last_tail = m_body.front();
            m_board.get(last_tail) = Backguard;
            m_body.pop_front();
        }
        //容纳当前踩踏点(免去body所有点坐标更新，只要更新首尾即可)
        m_body.push_back(next_pos);
        m_board.get(next_pos) = Body;

    }
    return true;
}

const Point& Snake::head() const
{
    return m_body.back();
}

const Point& Snake::tail() const
{
    return m_body.front();
}

const std::list<Point>& Snake::body() const
{
    return m_body;
}

bool cd(void)
{
    //cooldown:两次move之间时差限定最小值,自动蛇不用cd
    duration_t dt(cooldown);
    static timestamp op_time = std::chrono::steady_clock::now() - dt;
    timestamp curr_time = std::chrono::steady_clock::now();

    auto delta = curr_time - op_time;
    duration_t op_dt = std::chrono::duration_cast<std::chrono::milliseconds>(delta);
    if(delta.count() < cooldown)
    {
        return false;
    }
    op_time = std::chrono::steady_clock::now();
    return true;
}
