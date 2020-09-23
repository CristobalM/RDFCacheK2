#include "EmptyISDManager.hpp"

void EmptyISDManager::save(const std::string &sub_fname, const std::string &pred_fname,
                           const std::string &obj_fname)
{
}

uint64_t EmptyISDManager::blanks_index(std::string &blank)
{
    return 0;
}
uint64_t EmptyISDManager::literals_index(std::string &literal)
{
    return 0;
}
uint64_t EmptyISDManager::iris_index(std::string &iri)
{
    return 0;
}

bool EmptyISDManager::has_blanks_index(std::string &blank)
{
    return false;
}
bool EmptyISDManager::has_literals_index(std::string &literal)
{
    return false;
}
bool EmptyISDManager::has_iris_index(std::string &iri)
{
    return false;
}

std::string EmptyISDManager::get_blank(uint64_t _blanks_index)
{
    throw std::runtime_error("Not implemented");
}
std::string EmptyISDManager::get_literal(uint64_t _literals_index)
{
    throw std::runtime_error("Not implemented");
}
std::string EmptyISDManager::get_iri(uint64_t _iris_index)
{
    throw std::runtime_error("Not implemented");
}
RDFResource EmptyISDManager::get_resource(uint64_t index)
{
    throw std::runtime_error("Not implemented");
}

unsigned long EmptyISDManager::last_iri_id() { return 0; }
unsigned long EmptyISDManager::last_blank_id() { return 0; }
unsigned long EmptyISDManager::last_literal_id() { return 0; }
unsigned long EmptyISDManager::last_id() { return 0; }
