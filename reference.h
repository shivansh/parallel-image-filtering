#ifndef CONVOLUTE_H_
#define CONVOLUTE_H_

#include <vector>
using namespace std;

typedef vector<vector<int>> vvi;
typedef vector<vector<float>> vvf;

int convolute(const vvi&, const vvf&, vvi&, int);
vvi convolute_ref(const vvi&, const vvf&);

#endif  // CONVOLUTE_H_
