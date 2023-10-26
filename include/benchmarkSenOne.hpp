#include <benchmark.hpp>

class BenchMarkSenOne : public BenchMark{
    public:

        BenchMarkSenOne(const std::string& dbPath);

        void CreateTable();

        void WriteSingleData(int dataPointNum, int timeRange);

        void WriteBulkData(int dataPointNum, int timeRange);

        void QueryWithTime();

        void QueryDataWithinTime(int from, int to);

        void DeletDataOlderThan(int time);
};
