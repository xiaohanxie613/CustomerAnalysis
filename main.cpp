#include "sqlite3.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iomanip>

using namespace std;

void runQuery(sqlite3* db, const string& label, const string& query, bool addIndex = false) {
    cout << "\n=== " << label << " ===\n";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        int cols = sqlite3_column_count(stmt);
        int index = 1;

        // Print header
        if (addIndex) {
            cout << left << setw(6) << "Rank" << "| ";
        }
        for (int i = 0; i < cols; ++i) {
            cout << left << setw(20) << sqlite3_column_name(stmt, i);
            if (i < cols - 1) cout << "| ";
        }
        cout << "\n";

        // Print rows
        bool hasRow = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasRow = true;
            if (addIndex) cout << left << setw(6) << index++ << "| ";
            for (int i = 0; i < cols; ++i) {
                const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                cout << left << setw(20) << (text ? text : "NULL");
                if (i < cols - 1) cout << "| ";
            }
            cout << "\n";
        }
        if (!hasRow) {
            cout << "No results found.\n";
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Query failed: " << sqlite3_errmsg(db) << endl;
    }
}

void waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void showMenu(sqlite3* db) {
    int choice = 0;
    while (true) {
        cout << "\n=== Marketing Analysis Menu ===" << endl;
        cout << "1. Top Customer Spenders" << endl;
        cout << "2. Spending Analysis" << endl;
        cout << "3. Campaign Effectiveness" << endl;
        cout << "4. Predictive Spending Behavior" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            int n;
            cout << "\nEnter number of top spenders to display: ";
            cin >> n;
            cin.ignore();
            runQuery(db, "Top " + to_string(n) + " Customers by Total Spend",
                "SELECT ID, (MntWines + MntFruits + MntMeatProducts + MntFishProducts + MntSweetProducts + MntGoldProds) AS total_spent "
                "FROM marketing ORDER BY total_spent DESC LIMIT " + to_string(n) + ";", true);
            waitForEnter();
        } else if (choice == 2) {
            runQuery(db, "Spending by Income Group",
                "SELECT CASE WHEN Income < 20000 THEN 'Low' WHEN Income BETWEEN 20000 AND 60000 THEN 'Mid' ELSE 'High' END AS income_group, "
                "AVG(MntWines + MntMeatProducts + MntGoldProds) AS avg_spending FROM marketing WHERE Income IS NOT NULL GROUP BY income_group "
                "ORDER BY CASE income_group WHEN 'Low' THEN 3 WHEN 'Mid' THEN 2 WHEN 'High' THEN 1 END;");

            runQuery(db, "Spending by Family Size",
                "SELECT (Kidhome + Teenhome) AS family_size, "
                "AVG(MntWines + MntFruits + MntMeatProducts) AS avg_spending FROM marketing GROUP BY family_size ORDER BY family_size;");

            runQuery(db, "Spending by Marital Status",
                "SELECT Marital_Status, AVG(MntWines + MntGoldProds + MntSweetProducts) AS avg_spending "
                "FROM marketing GROUP BY Marital_Status ORDER BY CASE Marital_Status WHEN 'Single' THEN 1 WHEN 'Divorced' THEN 2 WHEN 'Widow' THEN 3 WHEN 'Together' THEN 4 WHEN 'Married' THEN 5 END;");
            waitForEnter();
        } else if (choice == 3) {
            runQuery(db, "Campaign Effectiveness",
                "SELECT 'Cmp1' AS Campaign, COUNT(*) AS Responses, "
                "AVG(MntWines + MntMeatProducts + MntGoldProds + MntSweetProducts) AS avg_spending FROM marketing WHERE AcceptedCmp1 = 1 "
                "UNION ALL "
                "SELECT 'Cmp2', COUNT(*), AVG(MntWines + MntMeatProducts + MntGoldProds + MntSweetProducts) FROM marketing WHERE AcceptedCmp2 = 1 "
                "UNION ALL "
                "SELECT 'Cmp3', COUNT(*), AVG(MntWines + MntMeatProducts + MntGoldProds + MntSweetProducts) FROM marketing WHERE AcceptedCmp3 = 1 "
                "UNION ALL "
                "SELECT 'Cmp4', COUNT(*), AVG(MntWines + MntMeatProducts + MntGoldProds + MntSweetProducts) FROM marketing WHERE AcceptedCmp4 = 1 "
                "UNION ALL "
                "SELECT 'Cmp5', COUNT(*), AVG(MntWines + MntMeatProducts + MntGoldProds + MntSweetProducts) FROM marketing WHERE AcceptedCmp5 = 1;");
            waitForEnter();
        } else if (choice == 4) {
            runQuery(db, "Predicted Spending Behavior",
                "SELECT PredictedSpending, AVG(MntWines + MntFruits + MntMeatProducts + MntFishProducts + MntGoldProds + MntSweetProducts) AS avg_spending "
                "FROM marketing WHERE PredictedSpending IS NOT NULL "
                "GROUP BY PredictedSpending "
                "ORDER BY CASE PredictedSpending WHEN 'High' THEN 1 WHEN 'Mid' THEN 2 WHEN 'Low' THEN 3 END;");
            waitForEnter();
        } else if (choice == 5) {
            break;
        } else {
            cout << "Invalid choice. Try again." << endl;
        }
    }
}

int main() {
    sqlite3* db;
    if (sqlite3_open("marketing.db", &db)) {
        cerr << "Failed to open DB\n";
        return 1;
    }

    const char* createSQL =
        "CREATE TABLE IF NOT EXISTS marketing ("
        "ID INTEGER PRIMARY KEY,"
        "Year_Birth INTEGER,"
        "Education TEXT,"
        "Marital_Status TEXT,"
        "Income REAL,"
        "Kidhome INTEGER,"
        "Teenhome INTEGER,"
        "Dt_Customer TEXT,"
        "Recency INTEGER,"
        "MntWines INTEGER,"
        "MntFruits INTEGER,"
        "MntMeatProducts INTEGER,"
        "MntFishProducts INTEGER,"
        "MntSweetProducts INTEGER,"
        "MntGoldProds INTEGER,"
        "NumDealsPurchases INTEGER,"
        "NumWebPurchases INTEGER,"
        "NumCatalogPurchases INTEGER,"
        "NumStorePurchases INTEGER,"
        "NumWebVisitsMonth INTEGER,"
        "AcceptedCmp3 INTEGER,"
        "AcceptedCmp4 INTEGER,"
        "AcceptedCmp5 INTEGER,"
        "AcceptedCmp1 INTEGER,"
        "AcceptedCmp2 INTEGER,"
        "Complain INTEGER,"
        "Z_CostContact INTEGER,"
        "Z_Revenue INTEGER,"
        "Response INTEGER,"
        "PredictedSpending TEXT"
        ");";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, createSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Create table error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return 1;
    }

    ifstream file("marketing_campaign.csv");
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    string line;
    bool skipHeader = true;

    while (getline(file, line)) {
        if (skipHeader) { skipHeader = false; continue; }

        stringstream ss(line);
        string fields[29];

        for (int i = 0; i < 29; i++) {
            getline(ss, fields[i], '\t');
        }

        if (fields[3] == "Yolo" || fields[3] == "YOLO" || fields[3] == "Absurd") {
            continue;
        } else if (fields[3] == "Alone") {
            fields[3] = "Single";
        }

        double totalSpend = stod(fields[9]) + stod(fields[10]) + stod(fields[11]) + stod(fields[12]) + stod(fields[13]) + stod(fields[14]);
        string spendingCategory = totalSpend > 1200 ? "High" : (totalSpend > 600 ? "Mid" : "Low");

        const char* insertSQL =
            "INSERT OR REPLACE INTO marketing VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, stoi(fields[0]));
            sqlite3_bind_int(stmt, 2, stoi(fields[1]));
            sqlite3_bind_text(stmt, 3, fields[2].c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, fields[3].c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 5, fields[4].empty() ? 0 : stod(fields[4]));
            for (int i = 5; i <= 27; ++i) {
                sqlite3_bind_int(stmt, i + 1, stoi(fields[i]));
            }
            sqlite3_bind_text(stmt, 30, spendingCategory.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                cerr << "Insert failed: " << sqlite3_errmsg(db) << endl;
            }
            sqlite3_finalize(stmt);
        } else {
            cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        }
    }

    file.close();
    sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, nullptr);

    showMenu(db);
    sqlite3_close(db);
    return 0;
}
