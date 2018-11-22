//----------------------------------------------------------------
//OMLBEGIN: 2D Segments
//Uses: vector2d.h
#include<algorithm>
using namespace std;

//!escape \section{Seg-Seg intersection, halfplane intersection area}
//!begin_codebook
//!start
struct Seg {
	Vec a, b;
	Vec d() { return b-a; }
};

Vec intersection(Seg l, Seg r) {
	Vec dl = l.d(), dr = r.d();
	if(cross(dl, dr) == 0)
		return {nanl(""), nanl("")};
	
	double h = cross(dr, l.a-r.a) / len(dr);
	double dh = cross(dr, dl) / len(dr);
	
	return l.a + dl * (h / -dh);
}

//Returns the area bounded by halfplanes
double calc_area(vector<Seg> lines) {
	double lb = -HUGE_VAL, ub = HUGE_VAL;
	
	vector<Seg> linesBySide[2];
	for(auto line : lines) {
		if(line.b.y == line.a.y) {
			if(line.a.x < line.b.x) {
				lb = max(lb, line.a.y);
      } else {
				ub = min(ub, line.a.y);
      }
		} else if(line.a.y < line.b.y) {
			linesBySide[1].push_back(line);
    } else {
			linesBySide[0].push_back({line.b, line.a});
    }
	}
	
	sort(linesBySide[0].begin(), linesBySide[0].end(), [](Seg l, Seg r) {
		if(cross(l.d(), r.d()) == 0) return normal(l.d())*l.a > normal(r.d())*r.a;
		return cross(l.d(), r.d()) < 0;
	});
	sort(linesBySide[1].begin(), linesBySide[1].end(), [](Seg l, Seg r) {
		if(cross(l.d(), r.d()) == 0) return normal(l.d())*l.a < normal(r.d())*r.a;
		return cross(l.d(), r.d()) > 0;
	});
	
	//Now find the application area of the lines and clean up redundant ones
	vector<double> applyStart[2];
	for(int side = 0; side < 2; side++) {
		vector<double> &apply = applyStart[side];
		vector<Seg> curLines;
		
		for(auto line : linesBySide[side]) {
			while(curLines.size() > 0) {
				Seg other = curLines.back();
				
				if(cross(line.d(), other.d()) != 0) {
					double start = intersection(line, other).y;
					if(start > apply.back()) break;
				}
				
				curLines.pop_back();
				apply.pop_back();
			}
			
			if(curLines.size() == 0) {
				apply.push_back(-HUGE_VAL);
      } else {
				apply.push_back(intersection(line, curLines.back()).y);
      }
			curLines.push_back(line);
		}
		
		linesBySide[side] = curLines;
	}
	applyStart[0].push_back(HUGE_VALL);
	applyStart[1].push_back(HUGE_VALL);
	
	double result = 0;
	{
		double lb = -HUGE_VALL, ub;
		for(int i=0, j=0; i < (int)linesBySide[0].size() && j < (int)linesBySide[1].size();lb = ub) {
			ub = min(applyStart[0][i+1], applyStart[1][j+1]);
			
			double alb = lb, aub = ub;
			Seg l0 = linesBySide[0][i], l1 = linesBySide[1][j];
			
			if(cross(l1.d(), l0.d()) > 0) {
				alb = max(alb, intersection(l0, l1).y);
      } else if(cross(l1.d(), l0.d()) < 0) {
				aub = min(aub, intersection(l0, l1).y);
      }
			alb = max(alb, lb);
			aub = min(aub, ub);
			aub = max(aub, alb);
			
			{
				double x1 = l0.a.x + (alb - l0.a.y) / l0.d().y * l0.d().x;
				double x2 = l0.a.x + (aub - l0.a.y) / l0.d().y * l0.d().x;
				result -= (aub - alb) * (x1 + x2) / 2;
			}
			{
				double x1 = l1.a.x + (alb - l1.a.y) / l1.d().y * l1.d().x;
				double x2 = l1.a.x + (aub - l1.a.y) / l1.d().y * l1.d().x;
				result += (aub - alb) * (x1 + x2) / 2;
			}
			
			if(applyStart[0][i+1] < applyStart[1][j+1]) {
				i++;
			} else {
				j++;
      }
		}
	}
	return result;
}
//!finish
//!end_codebook
//OMLEND: 2D Segments
//----------------------------------------------------------------
