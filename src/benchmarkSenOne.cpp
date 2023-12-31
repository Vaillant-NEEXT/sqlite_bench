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

void BenchMarkSenOne::WriteSingleData(int dataPointNum, int timeRange)
{
    //first write record than meta data
    const std::string insertRecordPattern = "INSERT INTO RECORD"
                                            "(datapoint_key, timestamp, value)"
                                            "VALUES (%1% ,%2% ,%3%);";

    const std::string insertMetaPattern = "INSERT INTO METADATA"
                                        "(datapoint_key, datapoint_name, sw_id, ecu_uuid, ts)"
                                        "VALUES (%1% ,%2% ,%3% ,%4% ,%5%);";


    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    std::string insert = "";

    // just some raw value
    int datapoint_key = 1;
    int timestamp = time(nullptr);
    int value = 20;
    int sw_id = 10;
    int ecu_uuid = 100;
    std::string datapoint_name = "flow_temp_circult_";

    for(int t = 0; t < timeRange; t++)
    {
        for(int i = 0; i < dataPointNum; i++)
        {
            insert = (boost::format(insertRecordPattern) % datapoint_key % (timestamp+t) % value).str();

            handle.ExecuteSql(insert);

            insert = (boost::format(insertMetaPattern) % datapoint_key %(datapoint_name + std::to_string(datapoint_key)) 
                                                        %sw_id  %ecu_uuid % timestamp).str();

            handle.ExecuteSql(insert);

            datapoint_key++;
        }

        if(t%100 == 0){
            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            std::cout << "write " << t << " timestamp computation " << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
            start = std::chrono::system_clock::now();
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void BenchMarkSenOne::WriteBulkData(int dataPointNum, int timeRange)
{
    std::string bulk = "INSERT INTO RECORD "
                        "(datapoint_key, timestamp, value) "
                        "VALUES";

    // write record
    int datapoint_key = 1;
    int timestamp = time(nullptr);
    int data = 10;
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    for(int t = 0; t < timeRange; t++)
    {
        for(int i = 0; i < dataPointNum; i++)
        {
            bulk += " (" + std::to_string(datapoint_key) + "," + std::to_string(timestamp+t) + "," + std::to_string(data) + "),";
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
        for(int i = 0; i < dataPointNum; i++){
            swid += datapoint_key;
            uuid += datapoint_key;
            bulk += " (" + std::to_string(datapoint_key) + "," + "'" + dataPointName + std::to_string(datapoint_key) + "'" + "," + std::to_string(swid) + 
                    "," + std::to_string(uuid) + "," + std::to_string(timestamp) + "),";

            datapoint_key++;
        }

        bulk.pop_back();

        bulk += ";";
        handle.ExecuteSql(bulk);

        if(t%100 == 0){
            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            std::cout << "write " << t << " timestamp computation " << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
            start = std::chrono::system_clock::now();
        }
    }
}

void BenchMarkSenOne::QueryWithTime(){}

void BenchMarkSenOne::QueryDataWithinTime(int from, int to)
{
    std::string select = (boost::format("SELECT * FROM RECORD WHERE timestamp >= %1% AND timestamp <= %2%;") %(std::to_string(from)) %std::to_string(to)).str();

    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
            for (int i = 0; i < argc; ++i) {
                //std::cout << colName[i] << " " << (argv[i] ? argv[i] : "NULL") << std::endl;
            }

            return SQLITE_OK;
        };

    auto start = std::chrono::system_clock::now();

    handle.ExecuteSql(select, callback);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "query computation " << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
}

void BenchMarkSenOne::DeletDataOlderThan(int time)
{
    auto start = std::chrono::system_clock::now();

    std::string remove = (boost::format("DELETE FROM RECORD WHERE timestamp < %1%;") %(std::to_string(time))).str();
    handle.ExecuteSql(remove);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "delete computation " << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
}
