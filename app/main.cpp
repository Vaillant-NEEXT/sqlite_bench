#include <databank.hpp>
#include "benchmarkSenOne.hpp"
#include "benchmarkSenTwo.hpp"
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream> 
#include <chrono>
#include <ctime> 
#include <thread>


namespace po = boost::program_options;
void CalculateTime();

po::variables_map parse_args(int argc, char **argv) {
    po::options_description desc("Allowed options");

    desc.add_options()
                    ("help, h", "produce help message")
                    ("path,p", po::value<std::string>(),"path of db file")
                    ("query,q", po::value<uint32_t>(), "do query")
                    ("query_from,f", po::value<int>(), "select data during time upper bound")
                    ("query_to,t", po::value<int>(), "select data during time lower bound");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    // handle help
    if (vm.count("help")) {
        // print help and exit
        std::cout << desc << std::endl;
        exit(0);
    }

    po::notify(vm);
    return vm;
}

int main(int argc, char **argv) {
try
{
    auto vm = parse_args(argc, argv);

    if (!vm.count("path"))
    {
        std::cout << "no path defined" << std::endl;
        return 0;
    }

    std::string dbPath = vm["path"].as<std::string>();
    std::cout << "begin bench 1" << std::endl;
    const std::string senonePath = dbPath + "/sehmafirst.db";
    BenchMarkSenOne benchOne(senonePath);

    benchOne.CreateTable();
    benchOne.WriteSingleData();
    benchOne.WriteBulkData();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    if (vm.count("query")) 
    {
        int queryfrom = 0;
        int queryto = 0;
        if (vm.count("query_from") && vm.count("query_to")) 
        {
            queryfrom = vm["query_from"].as<int>();
            queryto = vm["query_to"].as<int>();

            std::cout << "query from " << queryfrom << std::endl;
            std::cout << "query to " << queryto << std::endl;
        }

        benchOne.QueryDataWithinTime(queryfrom,queryto);
    }

    std::cout << "end bench 1" << std::endl;


    std::cout << "begin bench 2" << std::endl;
    const std::string sentwoPath = dbPath + "/sehmasecond.db";
    BenchMarkSenTwo benchTwo(sentwoPath);

    benchTwo.CreateTable();
    benchTwo.WriteSingleData();
    benchTwo.WriteBulkData();

    std::cout << "end bench 2" << std::endl;
}
catch(const std::exception& e)
{
    std::cerr << e.what() << '\n';
}

    return (0);
}


void CalculateTime()
{
    auto start = std::chrono::system_clock::now();

    auto end = std::chrono::system_clock::now();
 
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;
}