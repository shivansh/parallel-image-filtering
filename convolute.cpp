#include <cmath>
#include <iostream>
#include <vector>
using namespace std;

typedef vector<vector<int>> vvi;
typedef vector<vector<float>> vvf;

// TODO: update the signature as follows -
//      convolute(const vvi& image, const vvf& kernel, int index);

vvi convolute(const vvi& image, const vvf& kernel) {
    // TODO: handle invalid input
    int m = image.size(), n = image[0].size();
    int k = kernel.size();
    vvi output(m, vector<int>(n, 0));
    int p = 0, q = 0;  // coordinates of the top-left corner of the kernel
    while (p + k <= m) {
        float accumulator = 0;
        for (int i = p; i < p + k; ++i) {
            for (int j = q; j < q + k; ++j) {
                accumulator += image[i][j] * kernel[i - p][j - q];
            }
        }
        output[p + k / 2][q + k / 2] = round(accumulator);
        if (++q > n - k) {
            q = 0;
            ++p;
        }
    }
    return output;
}

int main() {
    int m, n, k;
    cin >> m >> n >> k;
    vvi image(m, vector<int>(n));
    vvf kernel(k, vector<float>(k));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> image[i][j];
        }
    }
    // NOTE: the kernel is flipped (rows and columns) while reading from stdin.
    for (int i = k - 1; i >= 0; --i) {
        for (int j = k - 1; j >= 0; --j) {
            cin >> kernel[i][j];
        }
    }
    vvi output = convolute(image, kernel);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << output[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}
