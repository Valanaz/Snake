#pragma once
#include <cassert>
#include <list>
#include <chrono>
#include "Board.h"

using timestamp = std::chrono::steady_clock::time_point;
using duration_t = std::chrono::milliseconds;

constexpr Point Up{-1,0},Down{1,0},Left{0,-1},Right{0,1},Zero{0,0};

class Snake{
    public:
        Snake(Board& board);
        virtual ~Snake();
        bool move(const Point& step );
        const Point& head() const;
        const Point& tail() const;
        const std::list<Point>& body() const;
    private:
        std::list<Point> m_body;
        Board& m_board;
};

bool cd(void);

