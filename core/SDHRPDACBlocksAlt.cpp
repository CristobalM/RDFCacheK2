//
// Created by cristobal on 28-10-21.
//

#include "SDHRPDACBlocksAlt.hpp"


#include "parallel/Worker.hpp"
#include <algorithm>
#include <condition_variable>
#include <cppUtils.h>
#include <iostream>
#include <mutex>
#include <string_view>

#include "iterators/IteratorDictStringHRPDACBlocks.h"
#include "SDHRPDACWrapper.hpp"

template <class T, class U>
size_t binary_search_before_index(std::vector<T> &v, const U &target) {
    auto lbound_it = std::lower_bound(v.begin(), v.end(), target);
    if (lbound_it == v.end())
        return v.size() - 1;

    auto pos = lbound_it - v.begin();

    if (pos > 0) {
        if (v[pos - 1] <= target && target < v[pos]) {
            return pos - 1;
        }
    }
    return pos;
}

SDHRPDACBlocksAlt::SDHRPDACBlocksAlt(
        IteratorDictStringPlainBuilder *it, unsigned long len, int overhead)
        : SDHRPDACBlocksAlt(it, len, overhead, DEFAULT_CUT_SIZE) {}

StringDictionary *process_iterator_work(IteratorDictStringPlain *it,
                                        int overhead) {
    return new StringDictionaryHASHRPDAC(it, 0, overhead);
}

SDHRPDACBlocksAlt::SDHRPDACBlocksAlt(
        IteratorDictStringPlainBuilder *it, unsigned long len, int overhead,
        unsigned long cut_size)
        : SDHRPDACBlocksAlt(it, len, overhead, cut_size, 1, 100'000'000) {}

SDHRPDACBlocksAlt::SDHRPDACBlocksAlt(
        IteratorDictStringPlainBuilder *it, unsigned long, int overhead,
        unsigned long cut_size, int thread_count, unsigned long memory_threshold)
        : cut_size(cut_size), strings_qty(0) {

    this->type = HASHRPDACBlocks;
    this->maxlength = 0;

    WorkerPool wpool(thread_count);

    std::mutex m;
    std::condition_variable cv;
    uint parts_done = 0;

    int current_it_parts = 0;
    unsigned long accumulated_memory = 0;
    std::vector<ItPlainWrapperWInfo> it_parts;
    it_parts.reserve(thread_count);
    maxlength = 0;
    while(it->next_it_available()){
        auto sub_it_wrappper = it->next_it();
        current_it_parts++;

        uint next_str_sz;
        uchar *next_str = sub_it_wrappper.get_it()->next(&next_str_sz);
        std::string sample_str(reinterpret_cast<char *>(next_str), next_str_sz);
        cut_samples.push_back(std::move(sample_str));
        sub_it_wrappper.get_it()->restart();

        unsigned long next_part_index;
        {
            std::lock_guard lg(m);
            accumulated_memory += sub_it_wrappper.get_it()->size();
            next_part_index = parts.size();
            parts.push_back(nullptr);
        }
        wpool.add_task(
                [this, next_part_index, &sub_it_wrappper, overhead, &m, &parts_done, &cv, &accumulated_memory]() {
                    StringDictionary *sd =
                            new SDHRPDACWrapper(sub_it_wrappper, 0, overhead);
                    {
                        std::lock_guard lg(m);
                        parts[next_part_index] = sd;
                        parts_done++;
                        accumulated_memory -= sub_it_wrappper.get_it()->size();
                        maxlength = std::max(maxlength, sd->maxLength());
                        strings_qty += sd->numElements();
                    }
                    cv.notify_all();
                });
        std::unique_lock<std::mutex> ul(m);
        cv.wait(ul, [&accumulated_memory, memory_threshold]() { return accumulated_memory < memory_threshold; });
    }

    std::unique_lock<std::mutex> ul(m);
    cv.wait(ul, [this, &parts_done]() { return parts_done == parts.size(); });
    wpool.stop_all_workers();
    wpool.wait_workers();
    delete it;
    this->elements = strings_qty;
}

unsigned long SDHRPDACBlocksAlt::locate(uchar *str,
                                                      uint str_length) {
    auto sview = std::string_view(reinterpret_cast<char *>(str), str_length);

    unsigned long result_pos = binary_search_before_index(cut_samples, sview);

    auto locate_result = parts[result_pos]->locate(str, str_length);
    return locate_result > 0 ? locate_result + starting_indexes[result_pos] : 0;
}

uchar *SDHRPDACBlocksAlt::extract(size_t id, uint *str_length) {
    if (id > strings_qty) {
        *str_length = 0;
        return nullptr;
    }
    unsigned long result_pos =
            binary_search_before_index(starting_indexes, id - 1);

    auto sindex = starting_indexes[result_pos];
    auto eindex = id - sindex;

    return parts[result_pos]->extract(eindex, str_length);
}

IteratorDictString *SDHRPDACBlocksAlt::extractTable() {
    return new IteratorDictStringHRPDACBlocks(parts, starting_indexes,
                                              strings_qty);
}

size_t SDHRPDACBlocksAlt::getSize() {
    size_t result = 0;
    for (auto *part : parts) {
        result += part->getSize();
    }
    result += parts.size() * sizeof(StringDictionaryHASHRPDAC *);
    result += starting_indexes.size() * sizeof(unsigned long);
    for (auto &s : cut_samples) {
        result += s.size();
    }
    return result;
}

void SDHRPDACBlocksAlt::save(std::ostream &out) {
    saveValue<uint32_t>(out, HASHRPDACBlocks);
    saveValue<uint32_t>(out, this->maxlength);
    saveValue<uint64_t>(out, cut_size);
    saveValue<uint64_t>(out, strings_qty);
    saveValue<uint32_t>(out, parts.size());

    for (uint i = 0; i < cut_samples.size(); i++) {
        auto &s = cut_samples[i];
        saveValue<uint32_t>(out, s.size());
        saveValue<char>(out, s.c_str(), s.size());
    }

    for (uint i = 0; i < starting_indexes.size(); i++) {
        saveValue<uint64_t>(out, starting_indexes[i]);
    }

    for (auto *part : parts) {
        part->save(out);
    }
}

StringDictionary *SDHRPDACBlocksAlt::load(std::istream &in,
                                                        uint technique) {
    auto _type = loadValue<uint32_t>(in);
    if (_type != HASHRPDACBlocks)
        return nullptr;
    auto _maxlength = loadValue<uint32_t>(in);
    auto _cut_size = loadValue<uint64_t>(in);
    auto _strings_qty = loadValue<uint64_t>(in);
    auto parts_sz = loadValue<uint32_t>(in);
    std::vector<std::string> _cut_samples;
    std::vector<unsigned long> _starting_indexes;
    std::vector<StringDictionary *> _parts;
    for (uint i = 0; i < parts_sz; i++) {
        auto cut_sample_sz = loadValue<uint32_t>(in);
        auto *cut_sample_buf = loadValue<char>(in, cut_sample_sz);
        _cut_samples.emplace_back(cut_sample_buf, cut_sample_sz);
        delete[] cut_sample_buf;
    }
    for (uint i = 0; i < parts_sz; i++) {
        _starting_indexes.push_back(loadValue<uint64_t>(in));
    }

    for (uint i = 0; i < parts_sz; i++) {
        _parts.push_back(StringDictionaryHASHRPDAC::load(in, technique));
    }

    StringDictionary *result = new SDHRPDACBlocksAlt(
            _cut_size, _strings_qty, _maxlength, std::move(_parts),
            std::move(_cut_samples), std::move(_starting_indexes));

    return result;
}

SDHRPDACBlocksAlt::SDHRPDACBlocksAlt(
        unsigned long cut_size, unsigned long strings_qty, unsigned int maxlength,
        std::vector<StringDictionary *> &&parts,
        std::vector<std::string> &&cut_samples,
        std::vector<unsigned long> &&starting_indexes)
        : cut_size(cut_size), strings_qty(strings_qty), parts(std::move(parts)),
          cut_samples(std::move(cut_samples)),
          starting_indexes(std::move(starting_indexes)) {
    this->elements = strings_qty;
    this->maxlength = maxlength;
}

SDHRPDACBlocksAlt::~SDHRPDACBlocksAlt() {
    for (auto *sd : parts) {
        delete sd;
    }
}

IteratorDictID *SDHRPDACBlocksAlt::locatePrefix(uchar *, uint) {
    std::cout << "This dictionary does not provide prefix location" << std::endl;
    return NULL;
}

IteratorDictID *SDHRPDACBlocksAlt::locateSubstr(uchar *, uint) {
    std::cout << "This dictionary does not provide substring location"
              << std::endl;
    return NULL;
}

uint SDHRPDACBlocksAlt::locateRank(uint) {
    std::cout << "This dictionary does not provide rank location" << std::endl;
    return 0;
}

IteratorDictString *SDHRPDACBlocksAlt::extractPrefix(uchar *,
                                                                   uint) {
    std::cout << "This dictionary does not provide prefix extraction"
              << std::endl;
    return NULL;
}

IteratorDictString *SDHRPDACBlocksAlt::extractSubstr(uchar *,
                                                                   uint) {
    std::cout << "This dictionary does not provide substring extraction"
              << std::endl;
    return NULL;
}

uchar *SDHRPDACBlocksAlt::extractRank(uint, uint *) {
    std::cout << "This dictionary does not provide rank extraction" << std::endl;
    return NULL;
}
