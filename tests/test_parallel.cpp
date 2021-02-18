#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <queue>
#include <random>
#include <thread>

#include <cstdio>
#include <omp.h>
#include <unistd.h>

// Some repetition only to stress each thread
static const std::size_t rip = 1000;

void maxOfRow(const std::vector<std::vector<int>> &vec, int row, int *max,
              int *index) {
  for (unsigned j = 0; j != vec[row].size(); ++j) {
    auto currValue = vec[row][j];
    if (currValue > *max) {
      *max = vec[row][j];
      *index = j;
    }
  }
}

std::pair<int, std::size_t>
maxOfRowWithFuture(const std::vector<std::vector<int>> &matrix, int row) {
  int max = 0;
  std::size_t maxIndex = 0;

  for (std::size_t r = 0; r < rip; r++) {
    max = 0;
    maxIndex = 0;
    for (std::size_t j = 0; j != matrix[row].size(); ++j) {
      const auto currValue = matrix[row][j];
      if (currValue > max) {
        max = matrix[row][j];
        maxIndex = j;
      }
    }
  }

  return std::make_pair(max, maxIndex);
}

void calcMaxValueAndIndex(std::size_t n) {
  const unsigned seed = 17;
  std::mt19937 generator(seed);
  const unsigned rows = 8;
  const unsigned bound = 1000;
  std::vector<std::vector<int>> matrix;
  for (unsigned i = 0; i != rows; ++i) {
    std::vector<int> vecOfRandomNums(n);
    std::generate(vecOfRandomNums.begin(), vecOfRandomNums.end(),
                  [&generator]() { return generator() % bound; });
    //    for (unsigned j = 0; j != n; ++j)
    //      std::cout << vecOfRandomNums[j] << " ";
    //    std::cout << '\n';
    matrix.push_back(vecOfRandomNums);
  }
  std::cout << "Finished the matrix construction\n";

  int maxValue = 0;
  std::size_t maxIndexI = 0;
  std::size_t maxIndexJ = 0;
  std::size_t i, j;

  auto start = std::chrono::steady_clock::now();

  for (i = 0; i < rows; ++i) {
    int maxValueI = 0;
    std::size_t maxIndexSubJ = 0;
    // do rip repetitions
    for (std::size_t r = 0; r < rip; ++r) {
      maxValueI = 0;
      maxIndexSubJ = 0;
      for (j = 0; j < n; ++j) {
        if (maxValueI < matrix[i][j]) {
          maxValueI = matrix[i][j];
          maxIndexSubJ = j;
        }
      }
    }

    std::cout << "maxValue on the row = " << maxValueI
              << ", maxIndexJ = " << maxIndexSubJ << std::endl;
    if (maxValue < maxValueI) {
      maxValue = maxValueI;
      maxIndexI = i;
      maxIndexJ = maxIndexSubJ;
    }
  }
  auto end = std::chrono::steady_clock::now();
  std::cout << "Elapsed time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
                .count()
            << ". maxValue = " << maxValue << ", maxIndexI = " << maxIndexI
            << ", maxIndexJ = " << maxIndexJ << std::endl;

  int n_threads = 2;

  // openMP approach but it did not work properly
  //  {
  //    maxValue = 0;
  //    maxIndexI = 0;
  //    maxIndexJ = 0;
  //
  //    omp_set_num_threads(n_threads);
  //    start = std::chrono::steady_clock::now();
  //#pragma omp parallel for shared(matrix)
  //    for (i = 0; i < rows; ++i) {
  //      for (j = 0; j < n; ++j) {
  //        if (maxValue < matrix[i][j]) {
  //#pragma omp critical
  //          {
  //            maxValue = matrix[i][j];
  //            maxIndexI = i;
  //            maxIndexJ = j;
  //          }
  //        }
  //      }
  //    }
  //    end = std::chrono::steady_clock::now();
  //    std::cout << "Elapsed time: "
  //              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
  //                                                                       start)
  //                     .count()
  //              << ". maxValue = " << maxValue << ", maxIndexI = " <<
  //              maxIndexI
  //              << ", maxIndexJ = " << maxIndexJ << std::endl;
  //  }

  // threads approach
  {
    start = std::chrono::steady_clock::now();
    maxValue = 0;
    maxIndexI = 0;
    maxIndexJ = 0;
    std::size_t processedRows = 0;
    std::size_t chunck = 0;

    while (processedRows < rows) {
      std::vector<std::future<std::pair<int, std::size_t>>> f;

      for (int i = 0; i < n_threads - 1 && processedRows < rows; ++i) {
        f.push_back(std::async(std::launch::async, maxOfRowWithFuture, matrix,
                               processedRows));
        processedRows++;
      }

      // Use the main thread if needed
      if (processedRows < rows) {
        std::promise<std::pair<int, std::size_t>> prom;
        f.push_back(prom.get_future());
        prom.set_value(maxOfRowWithFuture(matrix, processedRows));
        processedRows++;
      }

      // Reduce result
      std::cout << "Reducing results:\n";
      for (auto &fut : f) {
        const auto [maxValueI, maxIndex] = fut.get();
        std::cout << "maxValue = " << maxValueI
                  << ", max j index = " << maxIndex << std::endl;
        if (maxValueI > maxValue) {
          maxValue = maxValueI;
          maxIndexJ = maxIndex;
        }
      }

      ++chunck;
    }
    end = std::chrono::steady_clock::now();
    std::cout << "Elapsed time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                  .count()
              << ". maxValue = " << maxValue << ", maxIndexJ = " << maxIndexJ
              << std::endl;
  }
}

int main(int argc, char **argv) {

  calcMaxValueAndIndex((std::size_t)std::stoi(argv[1]));

  return 0;
}
