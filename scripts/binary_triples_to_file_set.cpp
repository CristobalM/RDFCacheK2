#include <stdexcept>
#include <string>

#include <getopt.h>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>
#include <BinaryTriplesToSingleStringFile.hpp>
#include <FileIStream.hpp>
#include <FileOStream.hpp>
#include <external_sort.hpp>
#include <LightStringSortConnector.hpp>
#include <system_info.hpp>

struct parsed_options {
    std::string input_file;
    std::string output_file;
    std::string tmp_dir;
    int workers;
    unsigned long max_memory;
    bool tmpdir_generated;
};

parsed_options parse_cmline(int argc, char **argv);

int main(int argc, char **argv) {
    auto parsed = parse_cmline(argc, argv);

    auto unsorted_file = parsed.output_file + ".unsorted";

    {
        FileIStream file_istream(parsed.input_file, std::ios::in | std::ios::binary);
        FileOStream file_ostream(unsorted_file, std::ios::out | std::ios::trunc);
        BinaryTriplesToSingleStringFile::run(file_ostream, file_istream);
    }

    ExternalSort::ExternalSort<ExternalSort::LightStringSortConnector>::sort(
            unsorted_file, parsed.output_file, parsed.tmp_dir, parsed.workers, 10,
            parsed.max_memory, 4096, true);

    if(parsed.tmpdir_generated){
        std::filesystem::remove(parsed.tmp_dir);
    }
}

parsed_options parse_cmline(int argc, char **argv) {
    const char short_options[] = "i:o:";
    struct option long_options[] = {
            {"input-file", required_argument, nullptr, 'i'},
            {"output-file", required_argument, nullptr, 'o'},
            {"tmp-dir", optional_argument, nullptr, 't'},
            {"max-memory", optional_argument, nullptr, 'm'},
            {"workers", optional_argument, nullptr, 'w'},    };

    int opt, opt_index;

    bool has_input = false;
    bool has_output = false;

    bool has_tmp_dir = false;
    bool has_max_mem = false;
    bool has_workers = false;

    parsed_options out{};

    out.tmpdir_generated = false;

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
            case 't':
                if (optarg) {
                    out.tmp_dir = optarg;
                    has_tmp_dir = true;
                }
                break;
            case 'm':
                if (optarg) {
                    out.max_memory = std::stoul(std::string(optarg));
                    has_max_mem = true;
                }
                break;
            case 'w':
                if (optarg) {
                    out.workers = std::stoi(std::string(optarg));
                    has_workers = true;
                }
                break;
            default:
                break;
        }
    }

    if (!has_input)
        throw std::runtime_error("input-file (i) argument is required");
    if (!has_output)
        throw std::runtime_error("output-file (o) argument is required");
    if (!has_tmp_dir) {
        auto tmp_base = std::filesystem::current_path();
        auto fname_template = (
                               std::filesystem::path("tmpsort_XXXXXXXXXX"))
                .string();
        auto mut_fname_template =
                std::make_unique<char[]>(fname_template.size() + 1);
        std::copy(fname_template.begin(), fname_template.end(),
                  mut_fname_template.get());
        mut_fname_template[fname_template.size()] = '\0';
        char *tmp_dir = mkdtemp(mut_fname_template.get());
        if (!tmp_dir)
            throw std::runtime_error("Couldn't generate tmp dir");
        auto resulting_dir = tmp_base / std::filesystem::path(std::string(tmp_dir));
        out.tmp_dir = resulting_dir;
        out.tmpdir_generated = true;

    }

    if (!has_max_mem) {
        auto mem_total = get_mem_total();
        if (mem_total > 0) {
            out.max_memory = mem_total / 2;
        } else {
            out.max_memory = 1'000'000'000; // 1GB
        }
    }

    if (!has_workers) {
        out.workers = 1;
    }
    return out;
}