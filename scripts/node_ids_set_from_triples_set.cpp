#include <stdexcept>
#include <string>
#include <filesystem>

#include <getopt.h>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>
#include <external_sort.hpp>
#include <UnsignedLongSortConnector.hpp>

struct parsed_options {
    std::string input_file;
    std::string output_file;
};

parsed_options parse_cmline(int argc, char **argv);

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    auto parsed = parse_cmline(argc, argv);

    std::ifstream ifs(parsed.input_file, std::ios::in | std::ios::binary);
    std::ofstream ofs(parsed.output_file, std::ios::out | std::ios::binary | std::ios::trunc);
    FileData filedata{};
    filedata.size = read_u64(ifs);
    filedata.current_triple = 0;
    uint64_t total_nodes =    filedata.size * 3;
    write_u64(ofs,total_nodes);
    while (!filedata.finished()) {
        auto triple = filedata.read_triple(ifs);
        // ofs << triple.first << "," << triple.second << "," << triple.third << "\n";
        write_u64(ofs, triple.first);
        write_u64(ofs, triple.second);
        write_u64(ofs, triple.third);
    }
    ifs.close();
    ofs.close();

    auto dir_path = fs::path("tmpdir");
    if(!fs::exists(dir_path)){
        fs::create_directory(dir_path);
    }

    auto tmp_sorted = parsed.output_file + "-sorted";

    ExternalSort::ExternalSort<
            ExternalSort::UnsignedLongSortConnector,
            ExternalSort::DATA_MODE::BINARY>::sort(parsed.output_file,
                                                   tmp_sorted,
                                                   dir_path.string(), 4, 10,
                                                   1'000'000'000, 4096,
                                                   true);
    fs::remove(parsed.output_file);
    fs::rename(tmp_sorted, parsed.output_file);

    return 0;   
}

parsed_options parse_cmline(int argc, char **argv) {
    const char short_options[] = "i:o:";
    struct option long_options[] = {
            {"input-file", required_argument, nullptr, 'i'},
            {"output-file", required_argument, nullptr, 'o'},
    };

    int opt, opt_index;

    bool has_input = false;
    bool has_output = false;
    parsed_options out{};

    while ((
            opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
        if (opt == -1) {
            break;
        }
        switch (opt) {
            case 'i':
                out.input_file = optarg;
                has_input = true;
                break;
            case 'o':
                out.output_file = optarg;
                has_output = optarg;
                break;
            default:
                break;
        }
    }

    if (!has_input)
        throw std::runtime_error("input-file (i) argument is required");
    if (!has_output)
        throw std::runtime_error("output-file (o) argument is required");

    return out;
}