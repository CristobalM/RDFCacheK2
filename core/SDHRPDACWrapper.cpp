//
// Created by cristobal on 26-10-21.
//

#include "SDHRPDACWrapper.hpp"
#include "ItPlainWrapperWInfo.hpp"


unsigned long SDHRPDACWrapper::locate(uchar *str, uint strLen) {
    auto buffer = std::make_unique<uchar[]>(strLen+3);
    std::copy(str, str + strLen, buffer.get());
    static std::string last_characters = "$$";
    std::copy(last_characters.begin(), last_characters.end(), buffer.get() + strLen);
    buffer[strLen+2] = '\0';
    return internal_sd->locate(buffer.get(), strLen + 2);
}

uchar *SDHRPDACWrapper::extract(size_t id, uint *strLen) {
    uint internal_str_len;
    auto *raw_extracted = internal_sd->extract(id, &internal_str_len);
    auto *extracted = new uchar[internal_str_len-1];
    std::copy(raw_extracted, raw_extracted+internal_str_len-2, extracted);
    extracted[internal_str_len-2] = '\0';
    *strLen = internal_str_len-2;
    delete[] raw_extracted;
    return extracted;
}

IteratorDictID *SDHRPDACWrapper::locatePrefix(uchar *str, uint strLen) {
    return internal_sd->locatePrefix(str, strLen);
}

IteratorDictID *SDHRPDACWrapper::locateSubstr(uchar *str, uint strLen) {
    return internal_sd->locateSubstr(str, strLen);
}

uint SDHRPDACWrapper::locateRank(uint rank) {
    return internal_sd->locateRank(rank);
}

IteratorDictString *SDHRPDACWrapper::extractPrefix(uchar *str, uint strLen) {
    return internal_sd->extractPrefix(str, strLen);
}

IteratorDictString *SDHRPDACWrapper::extractSubstr(uchar *str, uint strLen) {
    return internal_sd->extractSubstr(str, strLen);
}

uchar *SDHRPDACWrapper::extractRank(uint rank, uint *strLen) {
    return internal_sd->extractRank(rank, strLen);
}

class IteratorDictStringSDBlocksWrapper : public IteratorDictString{

    std::unique_ptr<IteratorDictString> internal_it;
public:
    explicit IteratorDictStringSDBlocksWrapper(IteratorDictString *input_it) : internal_it(input_it){}

    bool hasNext() override {
        return internal_it->hasNext();
    }

    unsigned char *next(uint *str_length) override {
        uint str_length_internal;
        uchar * internal_next = internal_it->next(&str_length_internal);
        assert(str_length_internal >= 2);
        auto * result = new uchar[str_length_internal-1];
        std::copy(internal_next, internal_next + str_length_internal - 2, result);
        result[str_length_internal-2] = '\0';
        *str_length = str_length_internal - 2;
        return result;
    }

};

IteratorDictString *SDHRPDACWrapper::extractTable() {
    return new IteratorDictStringSDBlocksWrapper(internal_sd->extractTable());
}

size_t SDHRPDACWrapper::getSize() {
    return internal_sd->getSize();
}

void SDHRPDACWrapper::save(std::ostream &out) {
    internal_sd->save(out);
}

SDHRPDACWrapper::SDHRPDACWrapper(ItPlainWrapperWInfo itw, unsigned long len, int overhead) {

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

    internal_sd = std::make_unique<StringDictionaryHASHRPDAC>(it, len, overhead);
}

