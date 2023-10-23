#include <databank.hpp>


namespace DataRecorder::Data
{

    static constexpr int kSqliteFlag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_NOFOLLOW;
    DataBank::DataBank(const std::string& dbpath, const int flags)
    {
        sqlite3* sqliteRawPtr;
        int result = sqlite3_open_v2(dbpath.c_str(), &sqliteRawPtr, flags, "unix");

        sqlite.reset(sqliteRawPtr);
    }

    DataBank::DataBank(const std::string& dbpath) : DataBank(dbpath, kSqliteFlag)
    {
    }

    int DataBank::ExecuteSql(const std::string& query)
    {
        int result = sqlite3_exec(sqlite.get(), query.c_str(), nullptr, 0, nullptr);

        return result;
    }      

    int DataBank::ExecuteSql(const std::string& query, QueryCallBack callback)
    {
        int result = sqlite3_exec(sqlite.get(), query.c_str(), callback, 0, nullptr);

        return result;
    }
    
    bool DataBank::TableExist(const std::string& tableName)
    {
        std::string sql = "SELECT count(type) FROM sqlite_master WHERE type='table' AND name=" + tableName + ";";

        return ExecuteStep(sql) > 0;
    }

/*
    int DataBank::ExecuteStep(std::shared_ptr<sqlite3_stmt> sqlStmt)
    {
        int result = sqlite3_step(sqlStmt.get());

        return result;
    }   
*/

    int DataBank::ExecuteStep(const std::string& query)
    {
        sqlite3_stmt *stmt;
        int result = sqlite3_prepare_v2(sqlite.get(), query.c_str(), static_cast<int>(query.size()), &stmt, NULL);

        return sqlite3_step(stmt);
    }

    void DataBank::Deleter::operator()(sqlite3* sqlitePtr)
    {
        sqlite3_close_v2(sqlitePtr);    
    }

    /*
        DataBank::~DataBank()
        {
            int result = sqlite3_close_v2(sqlite.get());
        }
    */
} 