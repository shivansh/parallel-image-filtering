#include <mpi.h>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

typedef vector<vector<int>> vvi;
typedef vector<vector<float>> vvf;

// NOTE: index is computed in row-major order, and starts from 0.
int convolute(const vvi& image, const vvf& kernel, int index) {
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
    return round(accumulator);
}

void update(vvi& output, int index, int n, int result) {
    // evaluate the row and column from index
    int row = index / n, col = index % n;
    output[row][col] = result;
}

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // We are assuming at least 2 processes for this task
    if (world_size < 2) {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int m, n, k;
    cin >> m >> n >> k;
    vvi image(m, vector<int>(n));
    vvf kernel(k, vector<float>(k));
    vvi output(m, vector<int>(n));  // only master modifies output
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

    int num_iter = m * n + world_size - 1;
    int process_rank = 0;
    for (int i = 1; i <= num_iter; ++i) {
        int result;
        process_rank = (process_rank + 1) % world_size;
        if (process_rank == 0) {
            ++process_rank;
        }

        if (world_rank == process_rank && process_rank != 0) {
            // slave process
            if (i >= world_size) {
                MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }

            if (i <= m * n) {
                result = convolute(image, kernel, i - 1);
            }
        }

        if (world_rank == 0 && i > world_size) {
            // The slave was revisited, and hence sent its computation
            MPI_Recv(&result, 1, MPI_INT, process_rank, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            int index = i - 1;  // offset into the matrix
            update(output, index, n, result);
        }
    }

    if (world_rank == 0) {
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                cout << output[i][j] << " ";
            }
            cout << endl;
        }
    }
    MPI_Finalize();

    return 0;
}
