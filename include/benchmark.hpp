#pragma once

#include <databank.hpp>
#include <boost/format.hpp>
#include <iostream> 
#include <chrono>
#include <ctime> 
#include <thread>


class BenchMark{
    public:

        BenchMark(const std::string& dbPath): handle(std::move(DataRecorder::Data::DataBank(dbPath)))
        {
        }

        BenchMark(BenchMark const& db) = delete;

        BenchMark& operator =(BenchMark const& db) = delete;

        ~BenchMark() = default;

        virtual void CreateTable() = 0;

        virtual void WriteSingleData(int dataPointNum, int timeInterval, int timeRange)= 0;

        virtual void WriteBulkData(int dataPointNum, int timeInterval)= 0;

        virtual void QueryWithTime()= 0;

        virtual void QueryDataWithinTime(int from, int to)= 0;

        virtual void DeletDataOlderThan(int time)= 0;

    protected:
        DataRecorder::Data::DataBank handle;

};
