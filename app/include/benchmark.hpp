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

        virtual void WriteSingleData()= 0;

        virtual void WriteBulkData()= 0;

        virtual void QueryWithTime()= 0;

        virtual void QueryDataWithinTime()= 0;

        virtual void DeletDataOlderThan()= 0;

    protected:
        DataRecorder::Data::DataBank handle;

};
