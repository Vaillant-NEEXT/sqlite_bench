
#include <filesystem>
#include <memory>

#include "gtest/gtest.h"
#include "neext/bsw/persistency/persistency.hpp"

namespace neext {
namespace bsw {
namespace persistency {

// *********************************************
// Constants
// *********************************************

constexpr uint32_t kTestSwcId = 42;
constexpr uint32_t kInitialStorageVer = 1;
constexpr int kAllowEnvToBeOverwritten = 1;
const std::string kIniFilePathEnv = "NEEXT_PERSISTENCY_CONFIG_FILE";
const std::string kStorageFileExtension = ".db";
const std::filesystem::path kTestIniDirPath = TEST_INI_DIR;
const std::string kTestIniFile = kTestIniDirPath / "test.ini";

// *********************************************
// Test setup/teardown
// *********************************************

class PersistencyApiTest : public ::testing::Test {
    void SetUp() override {
        // set config file to use our test ini file
        setenv(kIniFilePathEnv.c_str(), kTestIniFile.c_str(),
               kAllowEnvToBeOverwritten);
        api_ = std::make_unique<PersistencyAPI>();
    }

    void TearDown() override {
        // remove any created test storage file
        std::filesystem::remove(std::to_string(kTestSwcId) +
                                kStorageFileExtension);

        // reset config env
        unsetenv(kIniFilePathEnv.c_str());
    }

   public:
    std::unique_ptr<PersistencyAPI> api_;
};

// *********************************************
// Test fixtures
// *********************************************

TEST_F(PersistencyApiTest, SimpleKeyValueAccess) {
    api_->Init(kTestSwcId, kInitialStorageVer);

    // write value for the first time
    EXPECT_NO_THROW(api_->Write("key", "4242"));
    EXPECT_EQ(api_->Read("key"), "4242");

    // overwrite value
    EXPECT_NO_THROW(api_->Write("key", "424242"));
    EXPECT_EQ(api_->Read("key"), "424242");
};

TEST_F(PersistencyApiTest, SimpleJsonAccess) {
    api_->Init(kTestSwcId, kInitialStorageVer);
    EXPECT_NO_THROW(api_->Write("key", "{\"val\": 4242}"));

    // read tree node
    EXPECT_EQ(api_->Read("key"), "{\"val\":4242}");

    // read leaf node
    EXPECT_EQ(api_->Read("key.val"), "4242");

    // read non-existing key, should return empty string
    EXPECT_EQ(api_->Read("dummy"), "");
    EXPECT_EQ(api_->Read("key.value"), "");
};

TEST_F(PersistencyApiTest, InvalidFormatExceptionOnInvalidInput) {
    api_->Init(kTestSwcId, kInitialStorageVer);

    // missing curl bracket
    EXPECT_THROW(api_->Write("key", "{\"val\":42"), InvalidFormatException);

    // missing key's quote
    EXPECT_THROW(api_->Write("key", "{42:\"0\"}"), InvalidFormatException);
}

TEST_F(PersistencyApiTest, AddNewJsonKey) {
    api_->Init(kTestSwcId, kInitialStorageVer);

    // write value for the first time
    EXPECT_NO_THROW(api_->Write("key", "{\"val\": 4242}"));

    // add new key
    EXPECT_NO_THROW(api_->Write("temp", "{\"value\": \"foo\"}"));

    // read tree node of the first key
    EXPECT_EQ(api_->Read("key"), "{\"val\":4242}");

    // read leaf node of the first key
    EXPECT_EQ(api_->Read("key.val"), "4242");

    // read tree node of the new key
    EXPECT_EQ(api_->Read("temp"), "{\"value\":\"foo\"}");

    // read lead node of the new key
    EXPECT_EQ(api_->Read("temp.value"), "foo");
};

TEST_F(PersistencyApiTest, ReplaceJsonKey) {
    api_->Init(kTestSwcId, kInitialStorageVer);

    // write value for the first time
    EXPECT_NO_THROW(api_->Write("key", "{\"value\": \"foo\"}"));

    // overwrite key with another JSON value
    EXPECT_NO_THROW(api_->Write("key", "{\"temp\": 4242}"));

    // read tree node of the replaced key
    EXPECT_EQ(api_->Read("key"), "{\"temp\":4242}");

    // read leaf node of the replaced key
    EXPECT_EQ(api_->Read("key.temp"), "4242");
};

TEST_F(PersistencyApiTest, IndexedArrayAccess) {
    api_->Init(kTestSwcId, kInitialStorageVer);
    api_->Write("key", "[42, {\"temp\":4242}, {\"foo\":\"bar\"}]");

    // access data using forward index
    EXPECT_EQ(api_->Read("key[0]"), "42");
    EXPECT_EQ(api_->Read("key[1]"), "{\"temp\":4242}");
    EXPECT_EQ(api_->Read("key[1].temp"), "4242");
    EXPECT_EQ(api_->Read("key[2]"), "{\"foo\":\"bar\"}");
    EXPECT_EQ(api_->Read("key[2].foo"), "bar");

    // access data using backward index
    EXPECT_EQ(api_->Read("key[#-1]"), "{\"foo\":\"bar\"}");
    EXPECT_EQ(api_->Read("key[#-1].foo"), "bar");
    EXPECT_EQ(api_->Read("key[#-2]"), "{\"temp\":4242}");
    EXPECT_EQ(api_->Read("key[#-2].temp"), "4242");
    EXPECT_EQ(api_->Read("key[#-3]"), "42");
};

TEST_F(PersistencyApiTest, StorageStateExceptionOnUnitializedAccess) {
    // throw any accesses on uninitialized API
    EXPECT_THROW(api_->Read("key"), StorageStateException);
    EXPECT_THROW(api_->Write("key", "2"), StorageStateException);
};

TEST_F(PersistencyApiTest, StorageVersionExceptionOnMismatchedStorageVer) {
    // init with version 1 for the first time
    api_->Init(kTestSwcId, kInitialStorageVer);

    // re-init with version 2
    EXPECT_THROW(api_->Init(kTestSwcId, kInitialStorageVer + 1),
                 StorageVersionException);
};

TEST_F(PersistencyApiTest, MultipleConnections) {
    api_->Init(kTestSwcId, kInitialStorageVer);
    api_->Write("key", "42");
    api_->Write("foo", "\"bar\"");

    // open another backend connection
    auto another_api = std::make_unique<PersistencyAPI>();
    EXPECT_NO_THROW(another_api->Init(kTestSwcId, kInitialStorageVer));

    // replace previous key using the new connection
    EXPECT_EQ(another_api->Read("key"), "42");
    EXPECT_NO_THROW(another_api->Write("key", "4242"));

    // add new key using the new connection
    EXPECT_NO_THROW(another_api->Write("foo", "bar2"));

    // read updated and new keys using the old connection
    EXPECT_EQ(api_->Read("key"), "4242");
    EXPECT_EQ(api_->Read("foo"), "bar2");
};

}  // namespace persistency
}  // namespace bsw
}  // namespace neext
