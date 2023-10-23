#include <benchmarkSenOne.hpp>

BenchMarkSenOne::BenchMarkSenOne(const std::string& dbPath) : BenchMark(dbPath)
{
}


void BenchMarkSenOne::CreateTable()
{
    std::string sql = "CREATE TABLE IF NOT EXISTS RECORD("
    "datapoint_key	INT NOT NULL, "
    "timestamp	INT NOT NULL,"
    "value BLOB,"
    "PRIMARY KEY(datapoint_key,timestamp));";

    handle.ExecuteSql(sql);

    // create index
    sql = "CREATE INDEX record_inx ON RECORD ( "
                "datapoint_key	ASC, "
                "timestamp	ASC);";

    handle.ExecuteSql(sql);

    // create table metadata
    sql = "CREATE TABLE IF NOT EXISTS METADATA("
        "datapoint_key	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "datapoint_name TEXT,"
        "sw_id	INT NOT NULL,"
        "ecu_uuid INT NOT NULL,"
        "ts	INT,"
        "FOREIGN KEY(datapoint_key, ts) REFERENCES RECORD(datapoint_key,timestamp));";
    
    handle.ExecuteSql(sql);
}

void BenchMarkSenOne::WriteSingleData()
{
    //first write record than meta data
    std::string insert = "INSERT INTO RECORD"
                        "(datapoint_key, timestamp, value)"
                        "VALUES (1,100000000,20);";

    handle.ExecuteSql(insert);

    insert = "INSERT INTO METADATA"
            "(datapoint_key, datapoint_name, sw_id, ecu_uuid, ts)"
            "VALUES (1, 'flow_temp_circult_1', 10, 100, 100000000);";

    handle.ExecuteSql(insert);

}

void BenchMarkSenOne::WriteBulkData()
{
    std::string bulk = "INSERT INTO RECORD "
                        "(datapoint_key, timestamp, value) "
                        "VALUES";

    // write record
    int datapoint_key = 1;
    int timestamp = time(nullptr);
    int data = 10;

    for(int i = 0; i < 10; i++){
        int cur = timestamp;
        for(int j = 0; j < 100; j++){
            cur++;
            data++;
            bulk += " (" + std::to_string(datapoint_key) + "," + std::to_string(cur) + "," + std::to_string(data) + "),";
        }

        datapoint_key++;
    }
    
    bulk.pop_back();

    bulk += ";";
    handle.ExecuteSql(bulk);


    // write data
    bulk = "INSERT INTO METADATA "
                        "(datapoint_key, datapoint_name, sw_id, ecu_uuid, ts) "
                        "VALUES";

    int swid = 10;
    int uuid = 1000;
    datapoint_key = 2;
    timestamp++;
    std::string dataPointName = "flow_temp_circult_";
    for(int i = 0; i < 10; i++){
        swid += datapoint_key;
        uuid += datapoint_key;
        bulk += " (" + std::to_string(datapoint_key) + "," + "'" + dataPointName + std::to_string(datapoint_key) + "'" + "," + std::to_string(swid) + 
                "," + std::to_string(uuid) + "," + std::to_string(timestamp) + "),";

        datapoint_key++;
    }

    bulk.pop_back();

    bulk += ";";
    handle.ExecuteSql(bulk);
}

void BenchMarkSenOne::QueryWithTime(){}

void BenchMarkSenOne::QueryDataWithinTime(int from, int to)
{
    std::string select = (boost::format("SELECT * FROM RECORD WHERE myNumber >= %1% AND myNumber <= %2%;") %(std::to_string(from)) %std::to_string(to)).str();

    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
            for (int i = 0; i < argc; ++i) {
                std::cout << colName[i] << " " << (argv[i] ? argv[i] : "NULL") << std::endl;
            }

            return SQLITE_OK;
        };

    handle.ExecuteSql(select, callback);
}

void BenchMarkSenOne::DeletDataOlderThan(){}
