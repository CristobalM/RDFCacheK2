//
// Created by cristobal on 26-10-21.
//

#include "SDBlocksWrapper.hpp"
#include "ItPlainWrapperWInfo.hpp"


unsigned long SDBlocksWrapper::locate(uchar *str, uint strLen) {
    auto buffer = std::make_unique<uchar[]>(strLen+3);
    std::copy(str, str + strLen, buffer.get());
    static std::string last_characters = "$$";
    std::copy(last_characters.begin(), last_characters.end(), buffer.get() + strLen);
    buffer[strLen+2] = '\0';
    return internal_sd->locate(buffer.get(), strLen + 2);
}

uchar *SDBlocksWrapper::extract(size_t id, uint *strLen) {
    uint internal_str_len;
    auto *raw_extracted = internal_sd->extract(id, &internal_str_len);
    auto *extracted = new uchar[internal_str_len-1];
    std::copy(raw_extracted, raw_extracted+internal_str_len-2, extracted);
    extracted[internal_str_len-2] = '\0';
    *strLen = internal_str_len-2;
    delete[] raw_extracted;
    return extracted;
}

IteratorDictID *SDBlocksWrapper::locatePrefix(uchar *str, uint strLen) {
    return internal_sd->locatePrefix(str, strLen);
}

IteratorDictID *SDBlocksWrapper::locateSubstr(uchar *str, uint strLen) {
    return nullptr;
}

uint SDBlocksWrapper::locateRank(uint rank) {
    return internal_sd->locateRank(rank);
}

IteratorDictString *SDBlocksWrapper::extractPrefix(uchar *str, uint strLen) {
    return internal_sd->extractPrefix(str, strLen);
}

IteratorDictString *SDBlocksWrapper::extractSubstr(uchar *str, uint strLen) {
    return internal_sd->extractSubstr(str, strLen);
}

uchar *SDBlocksWrapper::extractRank(uint rank, uint *strLen) {
    return internal_sd->extractRank(rank, strLen);
}

IteratorDictString *SDBlocksWrapper::extractTable() {
    return nullptr;
}

size_t SDBlocksWrapper::getSize() {
    return internal_sd->getSize();
}

void SDBlocksWrapper::save(std::ostream &out) {
    internal_sd->save(out);
}

SDBlocksWrapper::SDBlocksWrapper(ItPlainWrapperWInfo itw, unsigned long len, int overhead, unsigned long cut_size,
                                 int thread_count) {

    size_t new_buffer_size =
            itw.get_it()->size() + itw.get_num_elements() * 2; // we are adding 2 terminating characters per character
    auto new_buffer = new unsigned char[new_buffer_size]();

    size_t pos = 0;
    auto *it = itw.get_it();

    static std::string last_characters = "$$";
    while(it->hasNext()){
        uint str_len;
        auto *str = it->next(&str_len);
        std::copy(str, str + str_len ,new_buffer+pos);
        std::copy(last_characters.begin(), last_characters.end(), new_buffer+pos+str_len);
        pos += str_len + last_characters.size() + 1;
    }
    delete it;

    it = new IteratorDictStringPlain(new_buffer, new_buffer_size);

    internal_sd = std::make_unique<StringDictionaryHASHRPDACBlocks>(it, len, overhead, cut_size, thread_count);
}
