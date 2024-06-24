#pragma once

// #include "Board.h"
#include "Snake.h"
#include <algorithm>
#include <memory>

class AstarPoint{
public:
    AstarPoint(const Point& p);
    virtual ~AstarPoint() = default;
    int& F();
    int& G();
    int& H();
    std::shared_ptr<AstarPoint>& parent();

    operator Point();
    const bool operator<(const AstarPoint& rhs);
public:
    int x,y;
private:
    int m_F {0},m_G{0},m_H{0};
    std::shared_ptr<AstarPoint> m_parent = nullptr;
};

class Astar{
public:
    Astar(Board& b,Snake& s);
    virtual ~Astar() = default;
    
    bool analysePath();
    void exportPath(const AstarPoint& target);
    Point action(const Point& nextPoint);
    bool fakeSnakeSafe();
    Point wander();
    Point getStep();
    void printStat();

private:
    static bool findPath(Point head,Point target,const Table& table,std::list<AstarPoint>& _open,std::default_random_engine& eng);
    static std::vector<Point> surroundPoints(Point center,Point target,const Table& table,std::default_random_engine& eng);
    static AstarPoint contain(const std::list<AstarPoint>& _list,const Point& _p);
    static int calculateG(AstarPoint& _p);
    static int calculateH(const AstarPoint& _p,const AstarPoint& _target);
    static int calculateF(AstarPoint& _p);
private:
    Board& board;
    Snake& snake;
    std::list<Point> _path;//将会包含最短路径之外的点，但是根据终点逐个取parent到起点可以获得路径

};