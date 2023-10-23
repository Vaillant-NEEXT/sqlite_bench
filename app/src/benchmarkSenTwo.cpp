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

void BenchMarkSenTwo::WriteSingleData()
{
    //each datapoint key has a table
    std::string tableName = "RECORD_1";
    std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + "("
                        "timestamp	INT NOT NULL,"
                        "value BLOB,"
                        "PRIMARY KEY(timestamp));";
    

    handle.ExecuteSql(sql);
    std::string insert = "INSERT INTO " + tableName + 
                        "(timestamp, value)"
                        "VALUES (100000000,20);";

    handle.ExecuteSql(insert);

    insert = "INSERT INTO METADATA"
            "(datapoint_key, datapoint_name, sw_id, ecu_uuid)"
            "VALUES (1, 'flow_temp_circult_1', 10, 100);";

    handle.ExecuteSql(insert);

}

void BenchMarkSenTwo::WriteBulkData()
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
    for(int i = 0; i < 10; i++){
        std::string curTable = tableName + std::to_string(datapoint_key);
        std::string sql = (boost::format(createTable) % curTable).str();
        handle.ExecuteSql(sql);

        //insert row
        std::string bulk = (boost::format(insertTimestamp) %curTable).str();
        int cur = timeStamp;
        for(int j = 0; j < 100; j++){
            bulk += " (" + std::to_string(cur) + "," + std::to_string(value) + "),";;
            cur++;
        }

        bulk.pop_back();
        bulk += ";";
        handle.ExecuteSql(bulk);

        datapoint_key++;
    }

    // write data
    std::string metaBulk = "INSERT INTO METADATA "
                            "(datapoint_key, datapoint_name, sw_id, ecu_uuid) "
                            "VALUES";

    int swid = 10;
    int uuid = 1000;
    datapoint_key = 2;
    std::string dataPointName = "flow_temp_circult_";
    for(int i = 0; i < 10; i++){
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

}

void BenchMarkSenTwo::DeletDataOlderThan(){}
