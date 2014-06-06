#include <cstdio>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

#define SHOW(expr) cout << #expr << ": " << (expr) << "\n"
#define ArraySize(a) sizeof(a) / sizeof(a[0])

void printi(int i) 
{
	cout << i << " ";
}

int main(int argc, char *argv[])
{
	vector<int> ivec(10);
	SHOW(ivec.size());
	SHOW(ivec.capacity());
	SHOW(ivec.end() - ivec.begin());
	
	vector<int> res;
	res.reserve(10);
	
	SHOW(ivec.size());
	SHOW(ivec.capacity());
	SHOW(res.end() - res.begin());
	
	int x[] = {3, 5, 7, 1, 4, 2, 9, 8};
	int y[] = {2, 9, 4, 3, 0, 1, 8};
	int z[] = {8, 0, 9};
	
	set<int> xSet(x, x+ArraySize(x));
	set<int> ySet(y, y+ArraySize(y));
	set<int> zSet(z, z+ArraySize(z));
	
	SHOW(xSet.size());
	SHOW(ySet.size());
	SHOW(zSet.size());
	
	cout << "\nxSet: ";
	for_each(xSet.begin(), xSet.end(), printi);
	
	cout << "\nySet: ";
	for_each(ySet.begin(), ySet.end(), printi);
	
	cout << "\nzSet: ";
	for_each(zSet.begin(), zSet.end(), printi);
	cout << endl;
	
	std::vector<int> resXY;
	resXY.reserve(std::min(xSet.size(), ySet.size()));
	
	std::vector<int>::iterator it =
	std::set_intersection(xSet.begin(), xSet.end(), ySet.begin(), ySet.end(), resXY.begin());
	SHOW(resXY.size());
	SHOW(int(resXY.end() - resXY.begin()));
	SHOW(int(it - resXY.begin()));
	int nXy = it - resXY.begin();
	
	std::vector<int> resXYZ;
	resXYZ.reserve(std::min(nXy, (int)zSet.size()));

	it = 
	std::set_intersection(resXY.begin(), it, zSet.begin(), zSet.end(), resXYZ.begin());
	SHOW(resXYZ.size());
	
	cout << "\nX & Y & Z: ";
	for_each(resXYZ.begin(), it, printi);
	
	return 0;
}