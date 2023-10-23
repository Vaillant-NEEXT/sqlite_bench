#include <benchmark.hpp>

class BenchMarkSenTwo : public BenchMark{
    public:

        BenchMarkSenTwo(const std::string& dbPath);

        void CreateTable();

        void WriteSingleData(int dataPointNum, int timeStampNum);

        void WriteBulkData(int dataPointNum, int timeStampNum);

        void QueryWithTime();

        void QueryDataWithinTime(int from, int to);

        void DeletDataOlderThan(int time);
};
