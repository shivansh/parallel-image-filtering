#include <cmath>
#include <iostream>
#include <stdexcept>

#include "convolute.h"

// NOTE: index is computed in row-major order, and starts from 0.
int convolute(const vvi& image, const vvf& kernel, vvi& output, int index) {
    if (image.size() == 0) {
        throw invalid_argument("convolute_ref: empty image");
    }
    if (kernel.size() == 0) {
        throw invalid_argument("convolute_ref: empty kernel");
    }

    int m = image.size(), n = image[0].size();
    int k = kernel.size();

    // evaluate the row and column from index
    int row = index / n, col = index % n;

    if (row < 0 || row >= m || col < 0 || col >= n) {
        throw invalid_argument("convolute: invalid value of index");
    }

    // evaluate coordinates of the top-left corner of the kernel
    int p = row - k / 2, q = col - k / 2;

    float accumulator = 0;
    for (int i = p; i < p + k; ++i) {
        for (int j = q; j < q + k; ++j) {
            // NOTE: edge computations are handled via wrap.
            int k_i = i - p, k_j = j - q;  // kernel indices
            int m_i = i, m_j = j;          // matrix indices
            if (i < 0) m_i += m;
            if (j < 0) m_j += n;
            if (i >= m) m_i -= m;
            if (j >= n) m_j -= n;
            accumulator += image[m_i][m_j] * kernel[k_i][k_j];
        }
    }
    output[p + k / 2][q + k / 2] = round(accumulator);
    return round(accumulator);
}

vvi convolute_ref(const vvi& image, const vvf& kernel) {
    if (image.size() == 0) {
        throw invalid_argument("convolute_ref: empty image");
    }
    if (kernel.size() == 0) {
        throw invalid_argument("convolute_ref: empty kernel");
    }

    int m = image.size(), n = image[0].size();
    int k = kernel.size();
    vvi output(m, vector<int>(n, 0));
    int p = -(k / 2), q = -(k / 2);  // top-left corner of the kernel
    while (p + k / 2 < m) {
        float accumulator = 0;
        for (int i = p; i < p + k; ++i) {
            for (int j = q; j < q + k; ++j) {
                // NOTE: edge computations are handled via wrap.
                int k_i = i - p, k_j = j - q;  // kernel indices
                int m_i = i, m_j = j;          // matrix indices
                if (i < 0) m_i += m;
                if (j < 0) m_j += n;
                if (i >= m) m_i -= m;
                if (j >= n) m_j -= n;
                accumulator += image[m_i][m_j] * kernel[k_i][k_j];
            }
        }
        output[p + k / 2][q + k / 2] = round(accumulator);
        if (++q >= n - (k / 2)) {
            q = -(k / 2);
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

    // Test for convolute_ref.
    vvi output = convolute_ref(image, kernel);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << output[i][j] << " ";
        }
        cout << endl;
    }

    // Test for convolute.
    vvi new_output(m, vector<int>(n, 0));
    int index = 6;
    cout << convolute(image, kernel, new_output, index) << endl;
    return 0;
}
