/////////////////////////////////////////////////////////////////////////////
//
//  LychrelData.h
//
//  An helper data class for encapsulating a collection of Lychrel values
//    stored in two files:
//
//    - a binary data file containing the concatenation of all 
//        digit-representation numbers being processed.  This file is
//        nominally called "numbers.bin" by default.
//
//    - a binary file containing the offsets (indices) for each sequence of
//        digits in the data file.  This file is nominally called
//        "index.bin" by default.
//
#include <sys/mman.h>
#include <cassert>
#include <cstdio>
#include <atomic>
#include <iostream>
#include <mutex>
#include <vector>

#include "Number.h"

class LychrelData {
    // File handles for index and data files
    FILE* _indexFile = nullptr;
    FILE* _digitsFile = nullptr;

    // Arrays (pointers) to number data
    const size_t* _indices = nullptr;
    const char*   _digits = nullptr;

    // Total number of data values
    size_t _size;

    // Index to next available data item.
    std::atomic<size_t> _current = 0;

    // Critical region exclusion lock to prevent multiple threads
    //   modifying the _current value simultaneously
    std::mutex _mutex;

  public:
    //
    // --- Constructor ---
    //
    //  Accepts index and data filenames, and after successfully opening
    //    and determining their length, memory map (mmap) them into the
    //    processes address space.
    //
    LychrelData(const char* indexFile = "index.bin", 
      const char* dataFile = "numbers.bin") {
        _indexFile = fopen(indexFile, "rb");
        if (!_indexFile) {
            std::cerr << "Unable to open the index file '" << indexFile
                 << "' ... exiting\n";
            exit(EXIT_FAILURE);
        }

        _digitsFile = fopen(dataFile, "rb");
        if (!_digitsFile) {
            std::cerr << "Unable to open the data file '" << dataFile
                 << "' ... exiting\n";
            exit(EXIT_FAILURE);
        }

        fseek(_indexFile, 0, SEEK_END);
        auto bytes = ftell(_indexFile);

        _indices = (size_t*) mmap(nullptr, bytes, PROT_READ, MAP_PRIVATE,
            fileno(_indexFile), 0);

        if (reinterpret_cast<long int>(_indices) < 0) {
            std::cerr << "Unable to memory map the index file ... exiting\n";
            exit(EXIT_FAILURE);
        }

        _size = *_indices++;

        fseek(_digitsFile, 0, SEEK_END);
        bytes = ftell(_digitsFile);

        _digits = (const char*) mmap(nullptr, bytes, PROT_READ, MAP_PRIVATE,
            fileno(_digitsFile), 0);

        if (reinterpret_cast<long int>(_digits) < 0) {
            std::cerr << "Unable to memory map the index file ... exiting\n";
            exit(EXIT_FAILURE);
        }
    }

    //
    // --- Destructor ---
    //
    ~LychrelData() {
        fclose(_indexFile);
        fclose(_digitsFile);
    }

    // Return the number total number of data values    
    size_t size() const
        { return _size; }

    // Return the available (i.e., not already distributed) data values
    size_t available() const
        { return _size - _current; }

    // (readonly) Array indexing operator
    Number operator[] (size_t index) const {
        assert(index < _size);

        auto start = _indices[index];
        auto end   = _indices[index+1];

        auto len = end - start;

        return Number(&_digits[start], len);
    }

    // Thread-safe single number retrieval function.  Returns true, storing
    //   more recent available value in <n>; or returns false,  indicating
    //   no more data is available
    bool getNext(Number& n) {
        size_t index;
        {
            std::lock_guard{_mutex};
            index = _current++;
            if (index >= _size) { return false; }
        }

        auto start = _indices[index];
        auto end   = _indices[index+1];

        n.resize(end - start);
        n.assign(&_digits[start], &_digits[end]);

        return true;
    }

    // Thread-safe multiple number retrieval function.  Returns true, storing
    //   as many available values (up to the requested value) in the <numbers>
    //   vector; or returns false, indicating no more data is available
    bool getNext(size_t count, std::vector<Number>& numbers) {
        size_t index;
        {
            std::lock_guard lock{_mutex};
            index = _current;
            
            if (index >= _size) { return false; }

            if (index + count >= _size) {
                count = _size - index;
            }
            _current += count;
        }

        numbers.resize(count);
        for (auto& number : numbers) {
            auto start = _indices[index];
            auto end   = _indices[++index];

            number.resize(end - start);
            number.assign(&_digits[start], &_digits[end]);
        }

        return true;
    }
};