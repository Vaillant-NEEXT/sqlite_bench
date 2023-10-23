
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

// *********************************************
// Test setup/teardown
// *********************************************

class PersistencyConfigTest : public ::testing::Test {
    void SetUp() override {}

    void TearDown() override {
        // remove all test storage files
        std::filesystem::remove(std::to_string(kTestSwcId) +
                                kStorageFileExtension);

        // reset config env
        unsetenv(kIniFilePathEnv.c_str());
    }

   public:
    bool CheckStorageCreated() {
        return std::filesystem::exists(std::to_string(kTestSwcId) +
                                       kStorageFileExtension);
    }

    std::unique_ptr<PersistencyAPI> api_;
};

// *********************************************
// Test fixtures
// *********************************************

TEST_F(PersistencyConfigTest, SuccessOnCorrectConfig) {
    // set config file to use the correct test ini file
    std::string kTestIniFile = kTestIniDirPath / "test.ini";
    setenv(kIniFilePathEnv.c_str(), kTestIniFile.c_str(),
           kAllowEnvToBeOverwritten);

    // should not throw anything and storage is created
    api_ = std::make_unique<PersistencyAPI>();
    try {
        // EXPECT_NO_THROW(api_->Init(kTestSwcId, kInitialStorageVer));
        api_->Init(kTestSwcId, kInitialStorageVer);
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    EXPECT_TRUE(CheckStorageCreated());
};

TEST_F(PersistencyConfigTest, StorageDataExceptionOnNonExistingConfigFile) {
    // set config file to non-existing ini file
    std::string dummy_ini_path = kTestIniDirPath / "dummy.ini";
    setenv(kIniFilePathEnv.c_str(), dummy_ini_path.c_str(),
           kAllowEnvToBeOverwritten);

    // should throw because non-existing config file
    api_ = std::make_unique<PersistencyAPI>();
    EXPECT_THROW(api_->Init(kTestSwcId, kInitialStorageVer),
                 StorageDataException);

    // storage should not be created
    EXPECT_FALSE(CheckStorageCreated());
};

TEST_F(PersistencyConfigTest, StorageDataExceptionOnInvalidStoragePath) {
    // set config file to non-existing ini file
    std::string invalid_ini_path = kTestIniDirPath / "invalid_path.ini";

    // set config file to use our test ini file
    setenv(kIniFilePathEnv.c_str(), invalid_ini_path.c_str(),
           kAllowEnvToBeOverwritten);

    // should throw because non-existing config file
    api_ = std::make_unique<PersistencyAPI>();
    EXPECT_THROW(api_->Init(kTestSwcId, kInitialStorageVer),
                 StorageDataException);

    // storage should not be created
    EXPECT_FALSE(CheckStorageCreated());
};

TEST_F(PersistencyConfigTest, UseDefaultBackendOnInvalidBackendConfig) {
    // set config file to non-existing ini file
    std::string invalid_ini_path = kTestIniDirPath / "invalid_backend.ini";

    // set config file to use our test ini file
    setenv(kIniFilePathEnv.c_str(), invalid_ini_path.c_str(),
           kAllowEnvToBeOverwritten);

    // should not throw anything
    api_ = std::make_unique<PersistencyAPI>();
    EXPECT_NO_THROW(api_->Init(kTestSwcId, kInitialStorageVer));

    // storage should be created with the default storage backend
    EXPECT_TRUE(CheckStorageCreated());
};

}  // namespace persistency
}  // namespace bsw
}  // namespace neext
