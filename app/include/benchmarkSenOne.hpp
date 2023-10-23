#include <benchmark.hpp>

class BenchMarkSenOne : public BenchMark{
    public:

        BenchMarkSenOne(const std::string& dbPath);

        void CreateTable();

        void WriteSingleData(int dataPointNum, int timeStampNum);

        void WriteBulkData(int dataPointNum, int timeStampNum);

        void QueryWithTime();

        void QueryDataWithinTime(int from, int to);

        void DeletDataOlderThan(int time);
};
