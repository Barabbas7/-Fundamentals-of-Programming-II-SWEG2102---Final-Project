#include <iostream>
#include <string>
#include <sqlite3.h> // Include the SQLite header
using namespace std;
// Custom structure to hold user-specific data
struct MyUserData
{
    int userId;
    string userName;
    // Add any other relevant fields here
};

// Example callback function
static int my_special_callback(void *data, int argc, char **argv, char **azColName)
{
    // Cast the 'data' pointer back to your custom structure type
    MyUserData *userData = static_cast<MyUserData *>(data);

    // Access the user-specific data within the callback function
    cout << "User ID: " << userData->userId << endl;
    cout << "User Name: " << userData->userName << endl;

    // Process the result rows (if needed)
    // for (int i = 0; i < argc; i++)
    // {
    //     std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
    // }

    return 0; // Return 0 to continue processing rows
}

int main()
{
    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db); // Open an in-memory database

    if (rc != SQLITE_OK)
    {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    // Create an instance of your custom structure
    MyUserData myData;
    myData.userId = 123;
    myData.userName = "John Doe";

    // Execute a sample SQL statement
    const char *sql = "SELECT 1 AS Column1, 'Hello' AS Column2";
    rc = sqlite3_exec(db, sql, my_special_callback, &myData, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Error executing SQL: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_close(db);
    return 0;
}
