#pragma once

#include "sorty.hpp"

template<typename RandomIt>
void insertion_sort(RandomIt begin, RandomIt end) {
    for (auto it = begin; it != end; ++it) {
        auto current = *it;
        auto hole = it;

        while (hole != begin && *(hole - 1) > current) {
            *hole = *(hole - 1);
            --hole;
        }

        *hole = current;
    }
}

template<typename RandomIt>
void selection_sort(RandomIt begin, RandomIt end) {
    for (auto it = begin; it != end; ++it) {
        auto minElement = std::min_element(it, end);
        if (minElement != it) {
            std::iter_swap(it, minElement);
        }
    }
}

template<typename RandomIt>
void bubble_sort(RandomIt begin, RandomIt end) {
    bool swapped = true;
    while (swapped) {
        swapped = false;
        for (auto it = begin; it != end - 1; ++it) {
            if (*(it + 1) < *it) {
                std::iter_swap(it, it + 1);
                swapped = true;
            }
        }
        --end;
    }
}


void merge_sort(sal::sorty_vector::iterator begin, sal::sorty_vector::iterator end) {
    if (std::distance(begin, end) <= 1) {
      return;
    }

    // Find the middle iterator
    auto mid = begin + std::distance(begin, end) / 2;

    // Sort the left and right subvectors recursively
    merge_sort(begin, mid);
    merge_sort(mid, end);

    // Merge the two sorted subvectors
    std::vector<typename std::iterator_traits<sal::sorty_vector::iterator>::value_type> merged;
    std::merge(begin, mid, mid, end, std::back_inserter(merged));

    // Copy the merged vector back to the original vector
    std::copy(merged.begin(), merged.end(), begin);
}

template<typename RandomIt>
RandomIt partition(RandomIt begin, RandomIt end) {
    RandomIt pivot = end - 1; // Tomamos el último elemento como pivote
    RandomIt i = begin - 1;

    for (RandomIt j = begin; j < end - 1; ++j) {
        if (*j <= *pivot) {
            ++i;
            std::iter_swap(i, j);
        }
    }

    std::iter_swap(i + 1, end - 1);
    return i + 1;
}

template<typename RandomIt>
void quick_sort(RandomIt begin, RandomIt end) {
    if (begin < end) {
        RandomIt pivot = partition(begin, end);
        quick_sort(begin, pivot);
        quick_sort(pivot + 1, end);
    }
}

template<typename RandomIt>
void shell_sort(RandomIt begin, RandomIt end) {
    int n = std::distance(begin, end);
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            auto temp = *(begin + i);
            int j;
            for (j = i; j >= gap && *(begin + j - gap) > temp; j -= gap) {
                *(begin + j) = *(begin + j - gap);
            }
            *(begin + j) = temp;
        }
    }
}


template<typename RandomIt>
void counting_sort(RandomIt begin, RandomIt end) {
    auto maxElement = *std::max_element(begin, end);
    auto minElement = *std::min_element(begin, end);

    int range = (maxElement - minElement).get_value() + 1;
    std::vector<int> count(range, 0);

    for (auto it = begin; it != end; ++it) {
        ++count[(*it - minElement).get_value()];
    }

    for (int i = 1; i < range; ++i) {
        count[i] += count[i - 1];
    }

    std::vector<typename std::iterator_traits<RandomIt>::value_type> output(std::distance(begin, end));
    for (auto it = end - 1; it >= begin; --it) {
        output[count[(*it - minElement).get_value()] - 1] = *it;
        --count[(*it - minElement).get_value()];
    }

    std::copy(output.begin(), output.end(), begin);
}

int getDigit(int number, int position) {
    while (position--) {
        number /= 10;
    }
    return number % 10;
}

template<typename RandomIt>
void radix_sort(RandomIt begin, RandomIt end) {
    int maxDigits = 0;
    for (auto it = begin; it != end; ++it) {
        maxDigits = std::max(maxDigits, (int)std::to_string(it->get_value()).length());
    }

    std::vector<std::vector<typename std::iterator_traits<RandomIt>::value_type>> buckets(10);

    for (int digitPos = 0; digitPos < maxDigits; ++digitPos) {
        for (auto it = begin; it != end; ++it) {
            int digit = getDigit(it->get_value(), digitPos);
            buckets[digit].push_back(it->get_value());
        }

        auto outputIt = begin;
        for (auto& bucket : buckets) {
            for (const auto& num : bucket) {
                *outputIt++ = num;
            }
            bucket.clear();
        }
    }
}