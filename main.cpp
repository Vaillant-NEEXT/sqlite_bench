#include <databank.hpp>
#include "benchmarkSenOne.hpp"
#include "benchmarkSenTwo.hpp"
#include <boost/format.hpp>
#include <iostream> 
#include <chrono>
#include <ctime> 
#include <thread>


void CalculateTime();

char* GetCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool CmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}


int main(int argc, char **argv) {
try
{
    if(!CmdOptionExists(argv, argv+argc, "-p") || !CmdOptionExists(argv, argv+argc, "-r"))
    {
        std::cout << "no path defined or no range defined" << std::endl;
        return 0;
    }

    std::string dbPath(GetCmdOption(argv, argv + argc, "-p"));
    std::string range(GetCmdOption(argv, argv + argc, "-r"));

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "begin bench 1" << std::endl;
    const std::string senonePath = dbPath + "/sehmafirst.db";
    BenchMarkSenOne benchOne(senonePath);
    benchOne.SetPragma();
    benchOne.CreateTable();

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "WriteSingleData" << std::endl;
    benchOne.WriteSingleData(300, std::stoi(range));

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "WriteBulkData" << std::endl;
    benchOne.WriteBulkData(300, std::stoi(range));

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "begin query 1" << std::endl;
    if (CmdOptionExists(argv, argv+argc, "-q")) 
    {
        int queryfrom = 0;
        int queryto = 0;
        if (CmdOptionExists(argv, argv+argc, "-f") && CmdOptionExists(argv, argv+argc, "-t")) 
        {
            std::string fromStr(GetCmdOption(argv, argv + argc, "-f"));
            queryfrom = std::stoi(fromStr);

            std::string toStr(GetCmdOption(argv, argv + argc, "-t"));
            queryto = std::stoi(toStr);

            std::cout << "query from " << queryfrom << std::endl;
            std::cout << "query to " << queryto << std::endl;
        }

        for(int i = 0; i < 10; i++)
        {
            benchOne.QueryDataWithinTime(queryfrom,queryto);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    std::cout << "end query 1" << std::endl;

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "begin delete 1" << std::endl;
    benchOne.DeletDataOlderThan(time(nullptr) + 10);

    std::cout << "end bench 1" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    std::cout << "begin bench 2" << std::endl;
    const std::string sentwoPath = dbPath + "/sehmasecond.db";
    BenchMarkSenTwo benchTwo(sentwoPath);

    benchTwo.CreateTable();

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "WriteSingleData" << std::endl;
    benchTwo.WriteSingleData(300, std::stoi(range));

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "WriteBulkData" << std::endl;
    benchTwo.WriteBulkData(300, std::stoi(range));

    std::cout << "begin query 2" << std::endl;
    if (CmdOptionExists(argv, argv+argc, "-q")) 
    {
        int queryfrom = 0;
        int queryto = 0;
        if (CmdOptionExists(argv, argv+argc, "-f") && CmdOptionExists(argv, argv+argc, "-t")) 
        {
            std::string fromStr(GetCmdOption(argv, argv + argc, "-f"));
            queryfrom = std::stoi(fromStr);

            std::string toStr(GetCmdOption(argv, argv + argc, "-t"));
            queryto = std::stoi(toStr);

            std::cout << "query from " << queryfrom << std::endl;
            std::cout << "query to " << queryto << std::endl;
        }

        for(int i = 0; i < 10; i++)
        {
            benchTwo.QueryDataWithinTime(queryfrom,queryto);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    std::cout << "end query 1" << std::endl;

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "begin delete 2" << std::endl;
    benchTwo.DeletDataOlderThan(time(nullptr) + 10);

    std::cout << "end bench 2" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}
catch(const std::exception& e)
{
    std::cerr << e.what() << '\n';
}

    return (0);
}
