#include <databank.hpp>
#include "benchmarkSenOne.hpp"
#include "benchmarkSenTwo.hpp"
#include <boost/format.hpp>
#include <iostream> 
#include <chrono>
#include <ctime> 
#include <thread>



void CalculateTime();

int main(int argc, char **argv) {
try
{
    std::cout << "begin bench 1" << std::endl;
    const std::string senonePath = "/home/sspa/Desktop/dev/sqlite_bench/db/sehmafirst.db";
    BenchMarkSenOne benchOne(senonePath);

    benchOne.CreateTable();
    benchOne.WriteSingleData();
    benchOne.WriteBulkData();
    benchOne.QueryDataWithinTime();

    std::cout << "end bench 1" << std::endl;


    std::cout << "begin bench 2" << std::endl;
    const std::string sentwoPath = "/home/sspa/Desktop/dev/sqlite_bench/db/sehmasecond.db";
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