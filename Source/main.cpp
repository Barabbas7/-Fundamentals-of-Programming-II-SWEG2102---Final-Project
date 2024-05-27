#include <iostream>
#include <iomanip>
#include <string>
#include <sqlite3.h>
using namespace std;

struct usrOrder
{
    int food_id;
    int quantity;
    string fname, lname, address;
    int phone;
};

static int createDB(const char *s);
static int menuSelect(const char *s, string &lounge);
static int orderSelect(const char *s, usrOrder &tempOrder);
static int callback(void *NotUsed, int argc, char **argv, char **azColName);
static int confirm(void *data, int argc, char **argv, char **azColName);
static int insertData(const char *s, usrOrder &tempOrder);

int main()
{
    const char *dir = R"(../Database/Agelgil.db)";
    createDB(dir);

    string temp, lounges[4] = {"KK", "Kibnesh", "Teachers", "Workers"};
    cout << "\n\n\nWellcome to Agelgil!!\nAgelgil is a platform that connects all lounges and students in AASTU.\nOrder the food you like from one of the lounge available in our University.\nAll that is expected from you is to order and relax from wherever you are in AASTU, because we will be the ones to care about your food and deliver it to your doors in just a few minutes.\n";
    cout << "\n\nHere are the available foods in AASTU:\n";
    for (int i = 0; i < 4; i++)
    {
        const char *underline = "\033[4m";
        cout << endl;
        cout << underline << lounges[i] << "\033[0m";
        if (lounges[i] == "Teachers")
        {
            cout << left << setw(10) << setfill(' ') << "No";
            goto Label;
        }
        cout << "\t" << left << setw(10) << setfill(' ') << "No";
    Label:
        cout << left << setw(35) << setfill(' ') << "Food Name";
        cout << left << setw(10) << setfill(' ') << "Lounge";
        cout << left << setw(10) << setfill(' ') << "Price(birr)\033[0m\n";
        menuSelect(dir, lounges[i]);
    }
    cout << endl
         << "Please enter the food number and the quantity you want separated by space: ";
    usrOrder tempOrder;
    cin >> tempOrder.food_id >> tempOrder.quantity;
    orderSelect(dir, tempOrder);
    insertData(dir, tempOrder);
    return 0;
}

static int createDB(const char *s)
{
    sqlite3 *DB;
    char *messageError;

    string clear = "DELETE FROM FOOD_MENU;"
                   "DELETE FROM `sqlite_sequence` WHERE `name` = 'FOOD_MENU';",
           FOOD_MENU = "CREATE TABLE IF NOT EXISTS FOOD_MENU("
                       "FOOD_ID     INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "NAME        TEXT NOT NULL, "
                       "LOUNGE      CHAR(50) NOT NULL, "
                       "PRICE       INTEGER NOT NULL);",
           ORDERS = "CREATE TABLE IF NOT EXISTS ORDERS("
                    "ORDER_ID    INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "FOOD_ID     INTEGER NOT NULL, "
                    "QUANTITY    INTEGER NOT NULL, "
                    "FNAME       TEXT NOT NULL, "
                    "LNAME       TEXT NOT NULL, "
                    "ADDRESS     CHAR(50), "
                    "PHONE_NO    INTEGER ,"
                    "FOREIGN KEY (FOOD_ID) REFERENCES FOOD_MENU(FOOD_ID));",
           insert = "INSERT INTO FOOD_MENU (NAME, LOUNGE, PRICE) VALUES"
                    "('Pasta with sauce', 'KK', 43),"
                    "('Pasta with vegitable', 'KK', 40),"
                    "('Rice with sauce', 'KK', 35),"
                    "('Rice with vegitable', 'KK', 30),"
                    "('Enkulal firfir', 'KK', 50),"
                    "('Ertib', 'KK', 40),"
                    "('KK special', 'KK', 100),"
                    "('Ertib', 'Kibnesh', 30),"
                    "('Shiro', 'Kibnesh', 38),"
                    "('Beyaynet', 'Kibnesh', 50),"
                    "('Firfir', 'Kibnesh', 30),"
                    "('Enkulal firfir', 'Kibnesh', 40),"
                    "('Kibnesh special', 'Kibnesh', 70),"
                    "('Avocado juice', 'Teachers', 38),"
                    "('Tibs', 'Teachers', 90),"
                    "('Pizza', 'Teachers', 120),"
                    "('Fetira', 'Teachers', 80),"
                    "('Tegabino', 'Teachers', 50),"
                    "('Ertib', 'Teachers', 45),"
                    "('Teachers special', 'Teachers', 120),"
                    "('Key wot', 'Workers', 60),"
                    "('Dulet', 'Workers', 55),"
                    "('Tegabino', 'Workers', 45),"
                    "('Beyaynet', 'Workers', 50),"
                    "('Workers special', 'Workers', 80);";

    try
    {
        int exit = 0;
        exit = sqlite3_open(s, &DB);
        sqlite3_exec(DB, clear.c_str(), NULL, 0, &messageError);
        // Create  FOOD_MENU Table
        {
            /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
            exit = sqlite3_exec(DB, FOOD_MENU.c_str(), NULL, 0, &messageError);
            if (exit != SQLITE_OK)
            {
                cerr << "Error creating FOOD_MENU table." << endl;
                sqlite3_free(messageError);
            }
            else
                cout << "FOOD_MENU table created Successfully!" << endl;
        }
        // Inserts available foods on FOOD_MENU
        {
            exit = sqlite3_exec(DB, insert.c_str(), NULL, 0, &messageError);
            if (exit != SQLITE_OK)
            {
                cerr << "Error in record insertion." << endl;
                sqlite3_free(messageError);
            }
            else
                cout << "Records inserted Successfully!" << endl;
        }
        // Create ORDERS Table
        {
            exit = sqlite3_exec(DB, ORDERS.c_str(), NULL, 0, &messageError);
            if (exit != SQLITE_OK)
            {
                cerr << "Error creating ORDERS table." << endl;
                sqlite3_free(messageError);
            }
            else
                cout << "ORDERS table created Successfully!" << endl;
        }

        sqlite3_close(DB);
    }
    catch (const exception &e)
    {
        cerr << e.what();
    }
    return 0;
}
static int menuSelect(const char *s, string &lounge)
{
    sqlite3 *DB;
    char *messageError;

    string sql = "SELECT * FROM FOOD_MENU WHERE LOUNGE = '" + lounge + "';";

    int exit = sqlite3_open(s, &DB);
    /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here*/
    exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);

    if (exit != SQLITE_OK)
    {
        cerr << "Error in selectData function." << endl;
        sqlite3_free(messageError);
    }

    return 0;
}
// retrieve contents of database used by selectData()
/* argc: holds the number of results, argv: holds each value in array, azColName: holds each column returned in array, */
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    const int width[4] = {10, 35, 10, 10};
    cout << "\t";
    for (int i = 0; i < argc; i++)
    {
        // column name and value
        cout << left << setw(width[i]) << setfill(' ') << argv[i];
    }

    cout << endl;

    return 0;
}
static int orderSelect(const char *s, usrOrder &tempOrder)
{
    sqlite3 *DB;
    char *messageError;

    string sql = "SELECT NAME, LOUNGE, PRICE FROM FOOD_MENU WHERE FOOD_ID = '" + to_string(tempOrder.food_id) + "';";

    int exit = sqlite3_open(s, &DB);
    /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here*/
    exit = sqlite3_exec(DB, sql.c_str(), confirm, &tempOrder, &messageError);

    if (exit != SQLITE_OK)
    {
        cerr << "Error in selectData function." << endl;
        sqlite3_free(messageError);
    }

    return 0;
}
static int confirm(void *data, int argc, char **argv, char **azColName)
{
    char choice;
    int price = std::atoi(argv[2]);
    usrOrder *tempOrder = static_cast<usrOrder *>(data);
    cout << "You've chosen " << tempOrder->quantity << " " << argv[0] << " from " << argv[1] << ", your total price is " << price * tempOrder->quantity << ". Do you confirm your order? (y/n) : ";
    cin.ignore();
    choice = getchar();
    switch (choice)
    {
    case 'y':
        cout << "Please enter your First name: ";
        cin >> tempOrder->fname;
        cout << "Please enter your Last name: ";
        cin >> tempOrder->lname;
        cout << "Please enter your block and dorm number: ";
        cin.ignore();
        getline(cin, tempOrder->address);
        cout << "And finaly your Phone number so that we can call you: ";
        cin >> tempOrder->phone;
        break;
    case 'n':
        break;
    default:
        cout << "Invalid input, We hope to see you again ;)";
        exit(0);
        break;
    }

    return 0;
}
static int insertData(const char *s, usrOrder &tempOrder)
{
    sqlite3 *DB;
    char *messageError;

    string sql = "INSERT INTO ORDERS (FOOD_ID, QUANTITY, FNAME, LNAME, ADDRESS, PHONE_NO) VALUES('" + to_string(tempOrder.food_id) + "', '" + to_string(tempOrder.quantity) + "', '" + tempOrder.fname + "', '" + tempOrder.lname + "', '" + tempOrder.address + "', '" + to_string(tempOrder.phone) + "');";

    int exit = sqlite3_open(s, &DB);
    /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK)
    {
        cerr << "Error in insertData function." << endl;
        sqlite3_free(messageError);
    }
    else
        cout << "Your Order is recorded Successfully!\nWe'll be there in a minute..." << endl;

    return 0;
}