//----------------------------------------------------------------
//OMLBEGIN: 2D Segments
//Uses: vector2d.h
#include<algorithm>
using namespace std;

struct Segment {
	Vec a, b;
	Vec d() {
		return b-a;
	}
};
ostream& operator<<(ostream& l, Segment r) {
	return l << r.a << '-' << r.b;
}

Vec intersection(Segment l, Segment r) {
	Vec dl = l.d(), dr = r.d();
	if(cross(dl, dr) == 0)
		return {nanl(""), nanl("")};
	
	long double h = cross(dr, l.a-r.a) / len(dr);
	long double dh = cross(dr, dl) / len(dr);
	
	return l.a + dl * (h / -dh);
}

//Returns the area bounded by halfplanes
long double getArea(vector<Segment> lines) {
	long double lowerbound = -HUGE_VALL, upperbound = HUGE_VALL;
	
	vector<Segment> linesBySide[2];
	for(auto line : lines) {
		if(line.b.y == line.a.y) {
			if(line.a.x < line.b.x)
				lowerbound = max(lowerbound, line.a.y);
			else
				upperbound = min(upperbound, line.a.y);
		}
		else if(line.a.y < line.b.y)
			linesBySide[1].push_back(line);
		else
			linesBySide[0].push_back({line.b, line.a});
	}
	
	sort(linesBySide[0].begin(), linesBySide[0].end(), [](Segment l, Segment r) {
		if(cross(l.d(), r.d()) == 0) return normal(l.d())*l.a > normal(r.d())*r.a;
		return cross(l.d(), r.d()) < 0;
	});
	sort(linesBySide[1].begin(), linesBySide[1].end(), [](Segment l, Segment r) {
		if(cross(l.d(), r.d()) == 0) return normal(l.d())*l.a < normal(r.d())*r.a;
		return cross(l.d(), r.d()) > 0;
	});
	
	//Now find the application area of the lines and clean up redundant ones
	vector<long double> applyStart[2];
	for(int side = 0; side < 2; side++) {
		vector<long double> &apply = applyStart[side];
		vector<Segment> curLines;
		
		for(auto line : linesBySide[side]) {
			while(curLines.size() > 0) {
				Segment other = curLines.back();
				
				if(cross(line.d(), other.d()) != 0) {
					long double start = intersection(line, other).y;
					if(start > apply.back())
						break;
				}
				
				curLines.pop_back();
				apply.pop_back();
			}
			
			if(curLines.size() == 0)
				apply.push_back(-HUGE_VALL);
			else
				apply.push_back(intersection(line, curLines.back()).y);
			curLines.push_back(line);
		}
		
		linesBySide[side] = curLines;
	}
	applyStart[0].push_back(HUGE_VALL);
	applyStart[1].push_back(HUGE_VALL);
	
	long double result = 0;
	{
		long double lb = -HUGE_VALL, ub;
		for(int i=0, j=0; i < (int)linesBySide[0].size() && j < (int)linesBySide[1].size();lb = ub) {
			ub = min(applyStart[0][i+1], applyStart[1][j+1]);
			
			long double alb = lb, aub = ub;
			Segment l0 = linesBySide[0][i], l1 = linesBySide[1][j];
			
			if(cross(l1.d(), l0.d()) > 0)
				alb = max(alb, intersection(l0, l1).y);
			else if(cross(l1.d(), l0.d()) < 0)
				aub = min(aub, intersection(l0, l1).y);
			alb = max(alb, lowerbound);
			aub = min(aub, upperbound);
			aub = max(aub, alb);
			
			{
				long double x1 = l0.a.x + (alb - l0.a.y) / l0.d().y * l0.d().x;
				long double x2 = l0.a.x + (aub - l0.a.y) / l0.d().y * l0.d().x;
				result -= (aub - alb) * (x1 + x2) / 2;
			}
			{
				long double x1 = l1.a.x + (alb - l1.a.y) / l1.d().y * l1.d().x;
				long double x2 = l1.a.x + (aub - l1.a.y) / l1.d().y * l1.d().x;
				result += (aub - alb) * (x1 + x2) / 2;
			}
			
			if(applyStart[0][i+1] < applyStart[1][j+1])
				i++;
			else
				j++;
		}
	}
	return result;
}
//OMLEND: 2D Segments
//----------------------------------------------------------------