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

        void SetPragma()
        {
            std::string sql = "pragma journal_mode = WAL;";
            handle.ExecuteSql(sql);
            sql = "pragma synchronous = normal;";
            handle.ExecuteSql(sql);
            sql = "pragma temp_store = memory;";
            handle.ExecuteSql(sql);
            sql = "pragma mmap_size = 30000000000;";
            handle.ExecuteSql(sql);
        }

        virtual void CreateTable() = 0;

        virtual void WriteSingleData(int dataPointNum, int timeRange)= 0;

        virtual void WriteBulkData(int dataPointNum, int timeRange)= 0;

        virtual void QueryWithTime()= 0;

        virtual void QueryDataWithinTime(int from, int to)= 0;

        virtual void DeletDataOlderThan(int time)= 0;

    protected:
        DataRecorder::Data::DataBank handle;

};
