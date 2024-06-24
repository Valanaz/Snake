
#include "Astar.h"

AstarPoint::AstarPoint(const Point& p):x{p.first},y{p.second}{
    ;
}

std::shared_ptr<AstarPoint>& AstarPoint::parent(){
    return m_parent;
}

int& AstarPoint::F(){
    return m_F;
}

int& AstarPoint::G(){
    return m_G;
}

int& AstarPoint::H(){
    return m_H;
}

AstarPoint::operator Point(){
    return Point(x,y);
}

const bool AstarPoint::operator<(const AstarPoint& rhs){
    return this->F() < const_cast<AstarPoint&>(rhs).F();
}


Astar::Astar(Board& b,Snake& s):board(b),snake(s){

}

bool Astar::analysePath(){
    std::list<AstarPoint> toApple;
    bool ok = findPath(snake.head(),board.apple(),board.table(),toApple,board.boardRandomEngine());
    if(!toApple.empty()){
        exportPath(toApple.back());
    }
    else{
        return false;
    }
    return ok;
}

//路径存放在_path
void Astar::exportPath(const AstarPoint& target){
    _path.clear();
    std::shared_ptr<AstarPoint> curr = std::make_shared<AstarPoint>(target);
    while(curr->parent()){
        _path.push_front(*curr);
        curr = curr->parent();
    }
}

//探路蛇吃到苹果后，要求能连通到尾巴，但是不希望一步就能吃到尾巴(除非蛇长度等于2) 但这样会吃不到最后一个苹果
bool Astar::fakeSnakeSafe(){
    Table fakeTable = board.table();
    std::list<Point> fakeSnake = snake.body();
    Point newTail;
    for(const Point& ap : _path){
        // Point act = action(Point(ap.x,ap.y));
        newTail = fakeSnake.front();
        fakeSnake.push_back(Point(ap.first,ap.second));
        fakeSnake.pop_front();
        fakeTable[ap.first][ap.second] = Body;
        fakeTable[newTail.first][newTail.second] = Backguard;
    }
    fakeSnake.push_front(newTail);//吃到苹果不切尾巴
    fakeTable[newTail.first][newTail.second] = Body;

    std::list<AstarPoint> faketoTail;
    bool ok = findPath(fakeSnake.back(),fakeSnake.front(),fakeTable,faketoTail,board.boardRandomEngine());
    // 探路失败时的现场观察
    // if(!ok || (faketoTail.back().parent()->parent() == nullptr && fakeSnake.size() > 2) ){
    //     std::cout<< "---------fakeStat---------" <<std::endl;
    //     char ch[4] = {'#','@','o',' '};
    //     for(int i = 0;i<boardSize;++i){
    //         for(int j = 0;j<boardSize;++j){
    //             if(i==fakeSnake.back().first && j == fakeSnake.back().second){
    //                 std::cout<< 'H' ;
    //                 continue;
    //             }
    //             if(i==fakeSnake.front().first && j == fakeSnake.front().second){
    //                 std::cout<< 'T' ;
    //                 continue;
    //             }
    //             std::cout<< ch[fakeTable[i][j]] ;
    //         }
    //         std::cout<<std::endl;
    //     }
    //     std::cout<< fakeSnake.size() <<std::endl;
    //     std::cout<< "---------fakeStat over---------" <<std::endl;
    // }
    return ok && (faketoTail.back().parent()->parent() != nullptr || fakeSnake.size() <= 2);
}

//路径解析为动作
Point Astar::action(const Point& nextPoint){
    int dx = nextPoint.first - snake.head().first;
    int dy = nextPoint.second - snake.head().second;
    return Point(dx,dy);
}

//转一条追尾路径，但是不踩尾巴，远离苹果，但必须保证和尾巴连通
Point Astar::wander(){
    std::default_random_engine eng = board.boardRandomEngine();
    std::vector<Point> surrounds = surroundPoints(snake.head(),Zero,board.table(),eng);
    int _distance {0};
    Point select(0,0);
    for(auto& p : surrounds){
        std::list<AstarPoint> wandertoTail;
        //本意是，wander一步以后仍存在到尾巴的路径 这个检测不严格，差一步位移
        if(!findPath(p,snake.tail(),board.table(),wandertoTail,eng)){
            continue;
        }
        int td = calculateH(p,board.apple());
        if(td > _distance){
            _distance = td;
            select = p;
        }
    }
    // std::cout << "wander to " << select.first<< ","<<select.second <<std::endl;
    // // 漫游异常检查
    // if(select == Zero){
    //     printStat();
    //     exit(1);
    // }
    return action(select);
}

Point Astar::getStep(){
    if(_path.empty())
    {
        return Zero;
    }
    Point _step = _path.front();
    _path.pop_front();
    return action(_step);
}

//寻找一条head->target的最短路线 _open存放可选地块
bool Astar::findPath(Point head,Point target,const Table& table,std::list<AstarPoint>& _open,std::default_random_engine& eng){
    if(head == target){
        return true;
    }
    std::list<AstarPoint> _close;//已经在路线上的点
    _open.emplace_back(head);
    bool ok = false;
    while(!_open.empty() && !ok){
        _open.sort();
        AstarPoint minFPoint = _open.front();
        _open.pop_front();
        _close.push_back(minFPoint);//踩踏之前，open中包含target则寻路成功

        std::vector<Point> surrounds = surroundPoints(minFPoint,target,table,eng);
        for(auto& p : surrounds){
            AstarPoint closePoint = contain(_close,p);
            if(Zero != Point(closePoint)){
                continue;
            }
            AstarPoint exist = contain(_open,p);
            if(Point(exist) != Zero){
                //计算新的G 与 历史值比较
                int history_G = exist.G();
                if(1+minFPoint.G() < history_G){
                    exist.parent() = std::make_shared<AstarPoint>(minFPoint);
                    exist.G() = calculateG(exist);
                }
            }
            else{
                AstarPoint ap(p);
                ap.parent() = std::make_shared<AstarPoint>(minFPoint);
                ap.G() = calculateG(ap);
                ap.H() = calculateH(ap,target);
                _open.push_back(ap);
            }

            if(p == target){//够得到终点
                ok = true;//成功之后 open排序取出终点追溯parent即可得到路径
                break;
            }
        }
    }
    return ok;
}

//水平/竖直位移开销 1
int Astar::calculateG(AstarPoint& _p){
    // return _p.parent() ? ++_p.parent()->G() : 1;
    int old = _p.parent() ? _p.parent()->G() : 0;
    return ++old;
}

int Astar::calculateH(const AstarPoint& _p,const AstarPoint& _target){
    return abs(_p.x - _target.x) + abs(_p.y - _target.y);
}

int Astar::calculateF(AstarPoint& _p){
    return _p.G()+_p.H();
}

//判断_p是否属于结点集合_list
AstarPoint Astar::contain(const std::list<AstarPoint>& _list,const Point& _p){
    for(const AstarPoint& listP : _list){
        if(_p.first == listP.x && _p.second == listP.y){
            return listP;
        }
    }
    return Zero;
}

std::vector<Point> Astar::surroundPoints(Point center,Point target,const Table& table,std::default_random_engine& eng){
    std::vector<Point> surrounds,availables(4);
    availables[0] = center + Up;
    availables[1] = center + Down;
    availables[2] = center + Left;
    availables[3] = center + Right;
    for(auto ap:availables){
        if(table[ap.first][ap.second] == Apple || table[ap.first][ap.second] == Backguard || ap == target){
            surrounds.push_back(ap);
        }
    }
    std::shuffle(surrounds.begin(),surrounds.end(),eng);
    return surrounds;
}

//现场
void Astar::printStat(){
    Table test = board.table();
    std::list<Point> testSnake = snake.body();
    char ch[4] = {'#','@','o',' '};
    for(int i = 0;i<boardSize;++i){
        for(int j = 0;j<boardSize;++j){
            if(i==testSnake.back().first && j == testSnake.back().second){
                std::cout<< 'H';
                continue;
            }
            if(i==testSnake.front().first && j == testSnake.front().second){
                std::cout<< 'T';
                continue;
            }
            std::cout << ch[test[i][j]];
        }
        std::cout<<std::endl;
    }
    std::cout<< testSnake.size() <<std::endl;
}



    

