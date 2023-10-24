#include <benchmarkSenTwo.hpp>

BenchMarkSenTwo::BenchMarkSenTwo(const std::string& dbPath) : BenchMark(dbPath)
{
}


void BenchMarkSenTwo::CreateTable()
{
    // create table metadata
    std::string sql = "CREATE TABLE IF NOT EXISTS METADATA("
        "datapoint_key	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "datapoint_name TEXT,"
        "sw_id	INT NOT NULL,"
        "ecu_uuid INT NOT NULL);";
    
    handle.ExecuteSql(sql);
}

void BenchMarkSenTwo::WriteSingleData(int dataPointNum, int timeStampNum)
{
    //each datapoint key has a table
    std::string tableName = "RECORD_";
    std::string datapoint_name = "flow_temp_circult_";

    const std::string createRecordPattern = "CREATE TABLE IF NOT EXISTS %1%("
                                            "timestamp INT NOT NULL,"
                                            "value BLOB,"
                                            "PRIMARY KEY(timestamp));";

    const std::string insertRecordPattern = "INSERT INTO %1%" 
                                            "(timestamp, value)"
                                            "VALUES (%2% ,%3%);";
    
    const std::string insertMetaPattern = "INSERT INTO METADATA "
                                            "(datapoint_key, datapoint_name, sw_id, ecu_uuid) "
                                            "VALUES (%1%, %2% ,%3%, %4%);";


    // just some raw value
    int datapoint_key = 1;
    int timestamp = 100000000; 
    int value = 20;
    int sw_id = 10;
    int ecu_uuid = 100;

    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    std::string insert = "";
    std::string sql = "";
    for(int i = 0; i < dataPointNum; i++)
    {
        start = std::chrono::system_clock::now();

        int cur = timestamp;
        sql = (boost::format(createRecordPattern) %(tableName + std::to_string(datapoint_key))).str();
        handle.ExecuteSql(sql);

        for(int j = 0; j < timeStampNum; j++)
        {
            insert = (boost::format(insertRecordPattern) %(tableName + std::to_string(datapoint_key)) %cur %value).str();

            handle.ExecuteSql(insert);
            cur++;
        }


        insert = (boost::format(insertMetaPattern) % datapoint_key %("'" + datapoint_name + std::to_string(datapoint_key) + "'") 
                                                    %sw_id  %ecu_uuid).str();

        handle.ExecuteSql(insert);

        datapoint_key++;
        timestamp++;

        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout << "write computation " << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

void BenchMarkSenTwo::WriteBulkData(int dataPointNum, int timeStampNum)
{
    // for each data a table 
    int datapoint_key = 2;
    std::string tableName = "RECORD_";
    const std::string createTable = "CREATE TABLE IF NOT EXISTS  %1% ("
                                    "timestamp INT NOT NULL,"
                                    "value BLOB,"
                                    "PRIMARY KEY(timestamp));";

    //write timestamp to datapoint table
    const std::string insertTimestamp = "INSERT INTO %1% " 
                                        "(timestamp, value) "
                                        "VALUES ";
    
    int timeStamp = time(nullptr);
    int value = 10;

    auto start = std::chrono::system_clock::now();

    for(int i = 0; i < dataPointNum; i++){
        std::string curTable = tableName + std::to_string(datapoint_key);
        std::string sql = (boost::format(createTable) % curTable).str();
        handle.ExecuteSql(sql);

        //insert row
        std::string bulk = (boost::format(insertTimestamp) %curTable).str();
        int cur = timeStamp;
        for(int j = 0; j < timeStampNum; j++){
            bulk += " (" + std::to_string(cur) + "," + std::to_string(value) + "),";;
            cur++;
        }

        bulk.pop_back();
        bulk += ";";
        handle.ExecuteSql(bulk);

        datapoint_key++;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "bulk write computation " << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
    
    // write data
    std::string metaBulk = "INSERT INTO METADATA "
                            "(datapoint_key, datapoint_name, sw_id, ecu_uuid) "
                            "VALUES";

    int swid = 10;
    int uuid = 1000;
    datapoint_key = 2;
    std::string dataPointName = "flow_temp_circult_";
    for(int i = 0; i < timeStampNum; i++){
        swid += datapoint_key;
        uuid += datapoint_key;
        metaBulk += " (" + std::to_string(datapoint_key) + "," + "'" + dataPointName + std::to_string(datapoint_key) + "'" + "," + std::to_string(swid) + 
                "," + std::to_string(uuid) + "),";

        datapoint_key++;
    }

    metaBulk.pop_back();

    metaBulk += ";";
    handle.ExecuteSql(metaBulk);
}

void BenchMarkSenTwo::QueryWithTime(){}

void BenchMarkSenTwo::QueryDataWithinTime(int from, int to)
{
    //get datapoint_keys from meta table 
    static std::vector<std::string> datapoints;

    auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
            for (int i = 0; i < argc; ++i) {
                //std::cout << colName[i] << " " << (argv[i] ? argv[i] : "NULL") << std::endl;

                if(argv[i]){
                    datapoints.push_back(std::string(argv[i]));
                }
            }

            return SQLITE_OK;
        };


    auto start = std::chrono::system_clock::now();

    std::string select = "SELECT datapoint_key FROM METADATA";

    handle.ExecuteSql(select, callback);

    const std::string selectPattern = "SELECT * FROM %1% WHERE timestamp >= %2% AND timestamp <= %3%;";
    std::string tableName = "RECORD_";

    auto selectCallback = [](void* data, int argc, char** argv, char** colName) -> int {
            for (int i = 0; i < argc; ++i) {
                //std::cout << colName[i] << " " << (argv[i] ? argv[i] : "NULL") << std::endl;
            }

            return SQLITE_OK;
        };

    for(int i = 0; i < datapoints.size(); i++)
    {
        select = (boost::format(selectPattern) %(tableName + datapoints[i]) %(std::to_string(from)) %(std::to_string(to))).str();
        handle.ExecuteSql(select, callback);
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "query computation " << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
    datapoints.clear();
}

void BenchMarkSenTwo::DeletDataOlderThan(int time){}
