#include <benchmark.hpp>

class BenchMarkSenTwo : public BenchMark{
    public:

        BenchMarkSenTwo(const std::string& dbPath);

        void CreateTable();

        void WriteSingleData();

        void WriteBulkData();

        void QueryWithTime();

        void QueryDataWithinTime();

        void DeletDataOlderThan();
};
