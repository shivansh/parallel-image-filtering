#include <mpi.h>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

typedef vector<vector<int>> vvi;
typedef vector<vector<float>> vvf;

// NOTE: `offset` is computed in row-major order, starting from 0.
// convolute computes the result of convolution at a particular matrix cell.
// The row and column number is computed using `offset`.
float convolute(const vvi& image, const vvf& kernel, int offset) {
    if (image.size() == 0) {
        throw invalid_argument("convolute: empty image");
    }
    if (kernel.size() == 0) {
        throw invalid_argument("convolute: empty kernel");
    }

    int m = image.size(), n = image[0].size();
    int k = kernel.size();

    // Evaluate the row and column using offset.
    int row = offset / n, col = offset % n;
    if (row < 0 || row >= m || col < 0 || col >= n) {
        throw invalid_argument("convolute: invalid value of offset");
    }

    // Evaluate coordinates of the top-left corner of the kernel.
    int p = row - k / 2, q = col - k / 2;

    float accumulator = 0;
    for (int i = p; i < p + k; ++i) {
        for (int j = q; j < q + k; ++j) {
            int k_i = i - p, k_j = j - q;  // kernel indices
            int m_i = i, m_j = j;          // matrix indices
            // NOTE: edge computations are handled via wrap.
            if (i < 0) m_i += m;
            if (j < 0) m_j += n;
            if (i >= m) m_i -= m;
            if (j >= n) m_j -= n;
            accumulator += image[m_i][m_j] * kernel[k_i][k_j];
        }
    }
    return accumulator;
}

int main(int argc, char** argv) {
    int m, n, k;
    cin >> m >> n >> k;
    vvi image(m, vector<int>(n));
    vvf kernel(k, vector<float>(k));
    vvf output(m, vector<float>(n));  // only master modifies output
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

    // Initialize the MPI environment.
    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // At least 2 processes (1 master and 1 slave) are required for this task.
    if (world_size < 2) {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int num_iter = m * n + world_size;  // number of iterations
    int slave_rank = 0;
    int master_rank = 0;
    float result;
    for (int i = 1; i <= num_iter; ++i) {
        // The slaves are visited in round-robin manner.
        slave_rank = (slave_rank + 1) % world_size;
        if (slave_rank == 0) {
            ++slave_rank;
        }

        if (world_rank == slave_rank) {
            // Slave process.
            if (i >= world_size) {
                // Each slave will send its computed result when it's revisited.
                MPI_Send(&result, 1, MPI_FLOAT, master_rank, 0, MPI_COMM_WORLD);
            }
            if (i <= m * n) {
                result = convolute(image, kernel, i - 1);
            }
        }

        if (world_rank == master_rank && i > world_size) {
            // Master process.
            int offset = (i - 1) - world_size;  // offset into the matrix
            int num_slaves = world_size - 1;    // slave process count
            int rank = offset % num_slaves + 1;
            MPI_Recv(&result, 1, MPI_FLOAT, rank, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

            // Evaluate {row, column} from the matrix offset and update output.
            int row = offset / n, col = offset % n;
            output[row][col] = result;
        }
    }

    // Master prints the updated output to stdout before exiting.
    if (world_rank == 0) {
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                // Round off output to 3 decimal places.
                cout << setprecision(3) << fixed << output[i][j] << " ";
            }
            cout << endl;
        }
    }
    MPI_Finalize();

    return 0;
}
