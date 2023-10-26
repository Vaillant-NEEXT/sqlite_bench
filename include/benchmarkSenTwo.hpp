#include <benchmark.hpp>

class BenchMarkSenTwo : public BenchMark{
    public:

        BenchMarkSenTwo(const std::string& dbPath);

        void CreateTable();

        void WriteSingleData(int dataPointNum, int timeRange);

        void WriteBulkData(int dataPointNum, int timeRange);

        void QueryWithTime();

        void QueryDataWithinTime(int from, int to);

        void DeletDataOlderThan(int time);
};
