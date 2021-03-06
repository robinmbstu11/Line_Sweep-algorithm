#include <iostream>
#include <vector>
#include <map>
#include <cmath>

struct Point
{
    char letter;
    double x, y;
    Point & operator = (Point const & b) { letter = b.letter;  x = b.x; y = b.y; return *this; }
    Point(const Point & b) : letter(b.letter), x(b.x), y(b.y) {}
    Point(char _letter, double _x, double _y) : letter(_letter), x(_x), y(_y) {}
    bool operator <(const Point & b) const
    {
        if (y < b.y - 1.0e-9) return true;
        else if (y > b.y + 1.0e-9) return false;
        else if (x < b.x - 1.0e-9) return true;
        else return false;
    }
    bool operator ==(const Point & b) const
    {
        return fabs(y - b.y) < 1.0e-9 && fabs(x - b.x) < 1.0e-9;
    }
    bool operator !=(const Point & b) const
    {
        return fabs(y - b.y) > 1.0e-9 || fabs(x - b.x) > 1.0e-9;
    }
};

struct Segment
{
    Point beg, end;
    Segment & operator = (Segment const & b) { beg = b.beg; end = b.end; return *this; }
    Segment(const Segment & b) : beg(b.beg), end(b.end) {}
    Segment(const Point & _beg, const Point & _end) : beg(_beg), end(_end) {}
};

#define SEG_START 0
#define SEG_END 1

class event_less
{
public:
    bool operator()(const std::pair<double, int> & a, const std::pair<double, int> & b) const
    {
        if (a.first < b.first - 1.0e-9)
            return true;
        else if (a.first > b.first + 1.0e-9)
            return false;
        else if (a.second < b.second)
            return true;
        return false;
    }
};




std::pair<bool, Point> intersect(const Segment & a, const Segment & b, bool print)
{
    Point ret('a', 0, 0);
    double div = (a.beg.x - a.end.x)*(b.beg.y - b.end.y) - (a.beg.y - a.end.y)*(b.beg.x - b.end.x), t;
    double val = 1.0e-13;
    std::cout<<" div "<<div<<" "<<fabs(div)<<" "<<val<<std::endl;
    if (fabs(div) < 1.0e-13)
    {
        std::cout<<" false"<<std::endl;
        if (print) std::cout << "divisor is zero" << std::endl;
        return std::make_pair(false, ret);
    }
    ret.x = ((a.beg.x*a.end.y - a.beg.y*a.end.x)*(b.beg.x - b.end.x) - (a.beg.x - a.end.x)*(b.beg.x*b.end.y - b.beg.y*b.end.x)) / div;
    ret.y = ((a.beg.x*a.end.y - a.beg.y*a.end.x)*(b.beg.y - b.end.y) - (a.beg.y - a.end.y)*(b.beg.x*b.end.y - b.beg.y*b.end.x)) / div;
    if (print) std::cout << "found (" << ret.x << "," << ret.y << ")" << std::endl;
    //probably some of these tests are redundant
    if (fabs(a.end.x - a.beg.x) > 1.0e-9)
    {
        t = (ret.x - a.beg.x) / (a.end.x - a.beg.x);
        if (t < 1.0e-9 || t > 1.0 - 1.0e-9)  { if (print) std::cout << "out of bound: " << t << std::endl; return std::make_pair(false, ret); }
    }
    if (fabs(a.end.y - a.beg.y) > 1.0e-9)
    {
        t = (ret.y - a.beg.y) / (a.end.y - a.beg.y);
        if (t < 1.0e-9 || t > 1.0 - 1.0e-9)  { if (print) std::cout << "out of bound: " << t << std::endl; return std::make_pair(false, ret); }
    }
    if (fabs(b.end.x - b.beg.x) > 1.0e-9)
    {
        t = (ret.x - b.beg.x) / (b.end.x - b.beg.x);
        if (t < 1.0e-9 || t > 1.0 - 1.0e-9)  { if (print) std::cout << "out of bound: " << t << std::endl; return std::make_pair(false, ret); }
    }
    if (fabs(b.end.y - b.beg.y) > 1.0e-9)
    {
        t = (ret.y - b.beg.y) / (b.end.y - b.beg.y);
        if (t < 1.0e-9 || t > 1.0 - 1.0e-9)  { if (print) std::cout << "out of bound: " << t << std::endl; return std::make_pair(false, ret); }
    }
    if (print) std::cout << "intersection accepted" << std::endl;
    return std::make_pair(true, ret);
}

void intersect(int a, int b, const Point & I, std::vector<Segment> & segments, std::multimap<Point, int> & sweep, std::multimap<std::pair<double,int>, int,event_less> & events, bool print)
{
    //remove event of ending of old segment
    {
        int rem_end_events[2];
        rem_end_events[0] = a;
        rem_end_events[1] = b;
        for (int k = 0; k < 2; ++k)
        {
            std::pair< std::multimap<std::pair<double,int>, int,event_less>::iterator, std::multimap<std::pair<double,int>,int,event_less>::iterator > del = events.equal_range(std::make_pair(segments[rem_end_events[k]].end.x,SEG_END)); //get all events at position of the end
            bool flag = false;
            for (std::multimap<std::pair<double,int>, int,event_less>::iterator it = del.first; it != del.second; ++it) //search over all events
            {
                if (it->first.second == SEG_END && it->second == rem_end_events[k]) //event is end of segment and segment matches current
                {
                    events.erase(it); //remove that segment
                    flag = true;
                    break; //do not expect any more
                }
            }
            if (!flag) std::cout << "Cannot find proper ending event for segment" << std::endl;
        }
    }
    //add new segment with intersection point up to end
    segments.push_back(Segment(I, segments[a].end));
    //add event of starting of new segment
//    events[std::make_pair(I.x,SEG_START)] = (int)segments.size() - 1);
    events.insert(std::make_pair(std::make_pair(I.x,SEG_START), (int)segments.size() - 1));
    //add event of ending of new segment
    events.insert(std::make_pair(std::make_pair(segments.back().end.x,SEG_END),(int)segments.size() - 1));
    //change ending point for current segment
    segments[a].end = I;
    //add event of ending of old segment
    events.insert(std::make_pair(std::make_pair(I.x,SEG_END), a));
    //add new segment with intersection point up to end
    segments.push_back(Segment(I, segments[b].end));
    //add event of starting of new segment
    events.insert(std::make_pair(std::make_pair(I.x,SEG_START), (int)segments.size() - 1));
    //add event of ending of new segment
    events.insert(std::make_pair(std::make_pair(segments.back().end.x,SEG_END), (int)segments.size() - 1));
    //change ending point for current segment
    segments[b].end = I;
    //add event of ending of old segment
    events.insert(std::make_pair(std::make_pair(I.x,SEG_END), b));
    if (print)
    {
        std::cout << "Number of events: " << events.size() << std::endl;
        for (std::multimap<std::pair<double, int>, int,event_less>::iterator it = events.begin(); it != events.end(); ++it)
            std::cout << "x: " << it->first.first << " type " << (it->first.second == SEG_START ? "start" : "end") << " segment " << it->second << std::endl;
    }
}

//find all intersection points
void intersect(std::vector<Segment> & segments, std::vector<Point> & intersections, bool print)
{
    std::multimap<std::pair<double,int>,int,event_less> events;
    std::multimap<Point,int> sweep;

    if( print )
    {
        std::cout << "Input segments[" << segments.size() << "]: " << std::endl;
        for (std::vector<Segment>::iterator it = segments.begin(); it != segments.end(); ++it)
            std::cout << "[ " << it->beg.letter << "(" << it->beg.x << "," << it->beg.y << "), " << it->end.letter << "(" << it->end.x << "," << it->end.y << ") ] " << std::endl;
        std::cout << "Create events based on segments." << std::endl;
    }

    for (int k = 0; k < (int)segments.size(); ++k)
    {
        if (segments[k].beg.x > segments[k].end.x)
            std::swap(segments[k].beg, segments[k].end);
        events.insert(std::make_pair(std::make_pair(segments[k].beg.x,SEG_START),k));
        events.insert(std::make_pair(std::make_pair(segments[k].end.x,SEG_END), k));
    }


    if (print)
    {
        std::cout << "Number of events: " << events.size() << std::endl;
        for (std::multimap<std::pair<double, int>, int,event_less>::iterator it = events.begin(); it != events.end(); ++it)
            std::cout << "x: " << it->first.first << " type " << (it->first.second == SEG_START ? "start" : "end") << " segment " << it->second << std::endl;

        std::cout << " Start parsing events" << std::endl;
    }

    while (!events.empty())
    {
        std::multimap<std::pair<double,int>,int,event_less>::iterator first = events.begin();
        int t = first->first.second;
        int s = first->second;
        events.erase(first);
        if (t == SEG_START)
        {
            if( print ) std::cout << "Segment " << s << " start" << std::endl;
            //check if there is a line with same position
            std::multimap<Point, int>::iterator ins = sweep.insert(std::make_pair(segments[s].beg, s));
            if (print)
            {
                std::cout << "Inserted into sweep" << std::endl;
                for (std::multimap<Point, int>::iterator it = sweep.begin(); it != sweep.end(); ++it)
                    std::cout << it->first.letter << "(" << it->first.x << "," << it->first.y << ")" << " segment " << it->second << std::endl;
            }
            //check line (or lines above current)
            for (int dir = 0; dir <= 1; ++dir) // look up or down
            {
                if( print ) std::cout << "Looking " << (dir ? "up" : "down") << std::endl;
                std::multimap<Point, int>::iterator iter = ins;
                while ((dir ? ++iter : iter--) != (dir ? sweep.end() : sweep.begin())) //y is greater for next
                {
                    if (print) std::cout << "test " << s << " with " << iter->second << std::endl;
                    if (segments[s].beg != segments[iter->second].beg) //ignore same starting position
                    {
                        if (print) std::cout << "checking intersection" << std::endl;
                        std::pair<bool, Point> I = intersect(segments[s], segments[iter->second],print);
                        if (I.first)
                        {
                            I.second.letter += (char)intersections.size();
                            if( print ) std::cout << "Intersection of " << s << " and " << iter->second << " at " << I.second.letter << "(" << I.second.x << "," << I.second.y << ")" << std::endl;
                            intersections.push_back(I.second);
                            intersect(s, iter->second, I.second, segments, sweep, events,print);
                        }
                    }
                    else if (print) std::cout << "skipping segments with same starting point" << std::endl;
                    if ((2*dir-1)*(iter->first.y - ins->first.y) > 0) //visited line is above (below) current
                        break; //stop search
                }
            }
        }
        else if (t == SEG_END)
        {
            if( print ) std::cout << "Segment " << s << " end" << std::endl;
            //remove segment from sweep
            std::pair< std::multimap<Point, int>::iterator, std::multimap<Point, int>::iterator > range = sweep.equal_range(segments[s].beg);
            if( print ) std::cout << "Range distance " << std::distance(range.first,range.second) << " sweep size " << sweep.size() << std::endl;
            std::multimap<Point, int>::iterator above = range.second, below = range.first;
            bool flag = false, test = true;
            if( below-- == sweep.begin() ) test = false;
            if( above == sweep.end() ) test = false;
            if( test && print ) std::cout << "Test will be performed" << std::endl;
            for (std::multimap<Point, int>::iterator it = range.first; it != range.second; ++it) //search over all events
            {
                if( it->second == s) //found necessery segment
                {
                    if (print)
                    {
                        std::cout << "Erase segment " << s << " from sweep: " << std::endl;
                        for (std::multimap<Point, int>::iterator it = sweep.begin(); it != sweep.end(); ++it)
                            std::cout << it->first.letter << "(" << it->first.x << "," << it->first.y << ")" << " segment " << it->second << std::endl;
                    }
                    sweep.erase(it);
                    flag = true;
                    break; //do not expect any more
                }
            }
            if (!flag) std::cout << __FILE__ << ":" << __LINE__ <<  " Error: cannot find segment " << s << " in sweep" << std::endl;
            if (test)
            {
                if (print) std::cout << "test " << below->second << " with " << above->second << std::endl;
                if (segments[above->second].beg != segments[below->second].beg)
                {
                    if (print) std::cout << "checking intersection" << std::endl;
                    std::pair<bool, Point> I = intersect(segments[below->second], segments[above->second],print);
                    if (I.first)
                    {
                        I.second.letter += (char)intersections.size();
                        if( print ) std::cout << "Intersection of " << below->second << " and " << above->second << " at " << I.second.letter << "(" << I.second.x << "," << I.second.y << ")" << std::endl;
                        intersections.push_back(I.second);
                        intersect(below->second, above->second, I.second, segments, sweep, events,print);
                    }
                }
                else if (print) std::cout << "skipping segments with same starting point" << std::endl;
            }
        }
    }

}


int main()
{
    std::vector<Point> intersections;
    std::vector<Segment> segments;

//    segments.push_back(Segment(Point('A',-7.41, -0.58), Point('C',-1.3,-0.79)));
//    segments.push_back(Segment(Point('B',-4.0, 1.27),   Point('D',-4.21, -2.99)));
//    segments.push_back(Segment(Point('F',-4.92, 0.71),  Point('G',-4.26, -1.40)));
//    segments.push_back(Segment(Point('I',-4.55, -1.24), Point('J',-2.54, -0.42)));
//    segments.push_back(Segment(Point('K',-3.70, 0.48),  Point('L',-3.70, -2.41))); //vertical
      segments.push_back(Segment(Point('A',1, 2), Point('B',4, 2)));
      segments.push_back(Segment(Point('C',2, 1), Point('D',2, 3)));
    segments.push_back(Segment(Point('E',3, 1), Point('F',3, 3)));

    std::cout<<" intersection occur "<<intersect(Segment(Point('A',1, 2), Point('B',4, 2)),Segment(Point('C',3.54, 1.46), Point('D',6.7, 1.38)),true).first<<std::endl;
    std::cout<<"end"<<std::endl;

//    intersect(segments, intersections,true);
    std::cout << "Intersection points[" << intersections.size() << "]: " << std::endl;
    for (std::vector<Point>::iterator it = intersections.begin(); it != intersections.end(); ++it)
        std::cout << it->letter << "(" << it->x << "," << it->y << ") " << std::endl;
    std::cout << "Segments[" << segments.size() << "]: " << std::endl;
    for (std::vector<Segment>::iterator it = segments.begin(); it != segments.end(); ++it)
        std::cout << "[ " << it->beg.letter << "(" << it->beg.x << "," << it->beg.y << "), " << it->end.letter << "(" << it->end.x << "," << it->end.y << ") ] " << std::endl;
    return 0;
}







//#include <string>
//#include <queue>
//#include <iostream>
//#include<stdlib.h>
//#include<stdio.h>
//#include <set>
//using namespace std;
//struct Point {
//    double x;
//    double y;
//    int segment;
//    int segid1;
//    int segid2;
//};
//typedef struct Point point;
//
//struct Segment{
//    double x1;
//    double x2;
//    double y1;
//    double y2;
//    int name;
//    double x;
//    double y;
//};
//typedef struct Segment segment;
//struct classcomp {
//    bool operator() (const segment& lhs,const segment& rhs)
//    {//return lhs.x>rhs.x;
//        if(rhs.x==lhs.x && rhs.y==lhs.y)
//            return ((rhs.x1 - lhs.x2) * (rhs.y2 - lhs.y2) - (rhs.y1 - lhs.y2) *     (rhs.x2 - lhs.x2))<0;
//        else
//            return ((rhs.x1 - lhs.x1) * (rhs.y2 - lhs.y1) - (rhs.y1 - lhs.y1) * (rhs.x2 - lhs.x1))<0;
//    }
//};
//
//class CompareEventPoint {
//public:
//    bool operator()(point& t1, point& t2) // Returns true if t1 is earlier than t2
//    {
//        if(t1.y<t2.y)return true;
//        return false;
//    }
//};
//
//
//point checkIntersection(segment a,segment b)
//{
//    point p;
//    p.segment=-1;
//    double num=(a.y1-b.y1)*(b.x2-b.x1)-(a.x1-b.x1)*(b.y2-b.y1);
//    double den=(a.y1-a.y2)*(b.x2-b.x1)-(a.x1-a.x2)*(b.y2-b.y1);
//    double s=num/den;
//    if(s>0 && s<1)
//    {
//        double t=((1-s)*a.x1+s*a.x2-b.x1)/(b.x2-b.x1);
//        if(t>0 && t<1)
//        {
//            p.segment=0;
//            if(a.x1>b.x1)
//            {
//                p.segid1=a.name;
//                p.segid2=b.name;
//            }
//            else
//            {
//                p.segid1=b.name;
//                p.segid2=a.name;
//            }
//            p.x=(1-s)*a.x1+s*a.x2;
//            p.y=(1-s)*a.y1+s*a.y2;
//            return p;
//        }
//    }
//    return p;
//
//}
//
//int main()
//{
//    int n,i;
//    FILE *fp,*f;
//    priority_queue<point, vector<point>, CompareEventPoint> pq; // Creates a priority queue pq to store strings, and initializes the queue to be empty.
//    printf("Enter the number of line Segments\n");
//    scanf("%d",&n);
//    segment segArray[n];
//
//
//
//
//    point p;
//    printf("Enter the co-ordinates of End points with top point first and then low  point\n");
//    for(i=0;i<n;i++)
//    {
//        scanf("%lf%lf",&p.x,&p.y);
//        p.segment=i+1;
//        pq.push(p);
//        segArray[i].name=i+1;
//        segArray[i].x1=p.x;
//        segArray[i].y1=p.y;
//        scanf("%lf%lf",&p.x,&p.y);
//        p.segment=-(i+1);
//        pq.push(p);
//        segArray[i].x2=p.x;
//        segArray[i].y2=p.y;
//
//    }
//
//
//    set<segment,classcomp> tree;
//    set<segment,classcomp>::iterator itlow,itup;
//    int segid;
//    point ptemp;
//    double lineY;
//    int iteration=0;
//    while (!pq.empty()) {
//
//        ptemp=pq.top();
//        pq.pop();
//        iteration++;
//        segid=ptemp.segment;
//        lineY=ptemp.y;
//        // Case of starting Point
//
//        for (itlow=tree.begin(); itlow!=tree.end();++itlow)
//            printf("%d\t",itlow->name);
//        printf("\n");
//
//
//
//        if(segid>0)
//        {
//
//            tree.insert(segArray[segid-1]);
//            itlow=tree.lower_bound (segArray[segid-1]);
//            if(itlow!=tree.begin())                                                  //       ^
//            {    itlow--;
//                p=checkIntersection(segArray[segid-1],segArray[itlow->name-1]);
//                if(p.segment==0 && ptemp.y>p.y)
//                {
//                    pq.push(p);
//
//                }
//            }
//
//            itup=tree.upper_bound (segArray[segid-1]);
//            if(itup!=tree.end())
//            {
//
//                p=checkIntersection(segArray[segid-1],segArray[itup->name-1]);
//                if(p.segment==0 && ptemp.y>p.y)
//                {
//                    pq.push(p);
//                }
//            }
//        }
//
//
//        // Case of intersection Point
//        if(segid==0)
//        {
//            int t1=ptemp.segid1;
//            int t2=ptemp.segid2;
//
//
//            segment s1=segArray[t1-1];
//            segment s2=segArray[t2-1];
//
//
//            tree.erase(s1);
//            tree.erase(s2);
//
//
//            segArray[t1-1].x=ptemp.x;
//            segArray[t1-1].x1=ptemp.x;
//            segArray[t2-1].x=ptemp.x;
//            segArray[t2-1].x1=ptemp.x;
//            segArray[t1-1].y=ptemp.y;
//            segArray[t1-1].y1=ptemp.y;
//            segArray[t2-1].y=ptemp.y;
//            segArray[t2-1].y1=ptemp.y;
//
//            s1=segArray[t1-1];
//            s2=segArray[t2-1];
//
//            tree.insert(s1);
//            tree.insert(s2);
//
//
//            itlow=tree.lower_bound (s1);
//            if(itlow!=tree.begin())
//            {       itlow--;
//                p=checkIntersection(s1,segArray[itlow->name-1]);
//
//                if(p.segment==0 && ptemp.y>p.y)
//                {
//                    pq.push(p);
//                }
//            }
//            itup=tree.upper_bound (s2);
//            if(itup!=tree.end())
//            {
//
//                p=checkIntersection(s2,segArray[itup->name-1]);
//                if(p.segment==0 && ptemp.y>p.y)
//                {
//                    pq.push(p);
//                }
//            }
//
//
//            printf("Intersection point=%lf %lf\n",ptemp.x,ptemp.y);
//
//        }
//
//        // Case of End point
//        if(segid<0)
//        {
//
//
//
//            segid=-segid;
//            if(itlow!=tree.begin() && itup!=tree.end())
//            {
//                itlow=tree.lower_bound (segArray[segid-1]);
//                itlow--;
//                itup=tree.upper_bound (segArray[segid-1]);
//            }
//            tree.erase(segArray[segid-1]);
//            if(itlow!=tree.begin() && itup!=tree.end())
//            {
//                p=checkIntersection(segArray[itlow->name-1],segArray[itup->name-1]);
//                if(p.segment==0 && ptemp.y>p.y)
//                {
//                    pq.push(p);
//                }
//
//            }
//
//        }
//    }
//
//    return 0;
//}