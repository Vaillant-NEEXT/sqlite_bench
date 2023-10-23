
#include <sqlite3.h>

#include <filesystem>
#include <memory>

#include "backend/sqlite3/query.hpp"
#include "gtest/gtest.h"
#include "neext/bsw/persistency/persistency.hpp"

namespace neext {
namespace bsw {
namespace persistency {

// *********************************************
// Constants
// *********************************************

const std::string kStorageFile = "./42.db";
static const std::string kSqlite3VfsModule = "unix";
constexpr auto kSqlite3OpenFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                                   SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_NOFOLLOW;

// *********************************************
// Test setup/teardown
// *********************************************

class PersistencyBackendTest : public ::testing::Test {
    void SetUp() override {
        // open database connection
        auto result =
            sqlite3_open_v2(kStorageFile.c_str(), &storage_, kSqlite3OpenFlags,
                            kSqlite3VfsModule.c_str());

        if (result != SQLITE_OK) {
            sqlite3_close(storage_);
            throw StorageDataException("Can't open database: " + kStorageFile);
        }

        // init storage
        query_ = std::make_unique<backend::sqlite3::Query>(*storage_);
        query_->CreateTable();
        query_->CreateRootNode();
    }

    void TearDown() override {
        // close connection
        if (storage_) {
            sqlite3_close(storage_);
        }

        // remove any created test storage file
        std::filesystem::remove(kStorageFile);
    }

   public:
    std::unique_ptr<backend::sqlite3::MetaData> PopulateDummyMetaData() const;

    void CompareMetaData(backend::sqlite3::MetaData& a,
                         backend::sqlite3::MetaData& b) const;

   public:
    ::sqlite3* storage_;
    std::unique_ptr<backend::sqlite3::Query> query_;
};

// *********************************************
// Test fixtures
// *********************************************

TEST_F(PersistencyBackendTest, SimpleLowLevelKeyValueAccess) {
    EXPECT_NO_THROW(query_->WriteKey("key", "4242"));
    EXPECT_EQ(query_->ReadKey("key"), "4242");
};

TEST_F(PersistencyBackendTest, SimpleLowLevelJsonAccess) {
    EXPECT_NO_THROW(query_->WriteKey("key", "{\"foo\":\"bar\"}"));
    EXPECT_EQ(query_->ReadKey("key"), "{\"foo\":\"bar\"}");
    EXPECT_EQ(query_->ReadKey("key.foo"), "bar");
};

TEST_F(PersistencyBackendTest, SimpleMetaDataAccess) {
    auto metadata = PopulateDummyMetaData();
    EXPECT_NO_THROW(query_->SetMetaData(*metadata));

    // compare metadata
    CompareMetaData(*query_->GetMetaData(), *metadata);
}

TEST_F(PersistencyBackendTest, StorageVersionStore) {
    // populate version
    auto metadata = PopulateDummyMetaData();
    metadata->storage_version = 99;

    // compare version
    EXPECT_NO_THROW(query_->SetMetaData(*metadata));
    EXPECT_EQ(query_->GetStorageVersion(), metadata->storage_version);
}

TEST_F(PersistencyBackendTest, StorageExceptionOnAccessAfterDeleteTable) {
    // delete table
    EXPECT_NO_THROW(query_->DeleteTable());

    // should throw
    EXPECT_THROW(query_->GetMetaData(), StorageDataException);

    // create table back
    EXPECT_NO_THROW(query_->CreateTable());

    // storage version must be zero, because it is not initialized yet
    EXPECT_EQ(query_->GetStorageVersion(), 0);

    // metadata should be nullptr because not initialized yet
    EXPECT_EQ(query_->GetMetaData(), nullptr);

    // now create the root node
    EXPECT_NO_THROW(query_->CreateRootNode());

    // set metadata with dummy values
    auto metadata = PopulateDummyMetaData();
    EXPECT_NO_THROW(query_->SetMetaData(*metadata));

    // compare metadata
    CompareMetaData(*query_->GetMetaData(), *metadata);
}

// *********************************************
// Helper functions
// *********************************************

std::unique_ptr<backend::sqlite3::MetaData>
PersistencyBackendTest::PopulateDummyMetaData() const {
    // populate versions and filename
    auto metadata = std::make_unique<backend::sqlite3::MetaData>();
    metadata->api_version = "v1.0.0";
    metadata->storage_filename = kStorageFile;
    metadata->storage_version = 7;

    // populate create date
    using sc = std::chrono::system_clock;
    auto now = sc::to_time_t(sc::now());
    struct std::tm* ptm = std::localtime(&now);
    std::stringstream ss;
    ss << std::put_time(ptm, "%F %X %Z");
    metadata->create_date = ss.str();

    return metadata;
}

void PersistencyBackendTest::CompareMetaData(
    // compare each field of the struct
    backend::sqlite3::MetaData& a, backend::sqlite3::MetaData& b) const {
    EXPECT_EQ(a.api_version, b.api_version);
    EXPECT_EQ(a.storage_filename, b.storage_filename);
    EXPECT_EQ(a.storage_version, b.storage_version);
    EXPECT_EQ(a.create_date, b.create_date);
}

}  // namespace persistency
}  // namespace bsw
}  // namespace neext
